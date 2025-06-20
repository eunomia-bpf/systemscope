// SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)
/*
 * profile    Profile CPU usage by sampling stack traces at a timed interval.
 * Copyright (c) 2022 LG Electronics
 *
 * Based on profile from BCC by Brendan Gregg and others.
 * 28-Dec-2021   Eunseon Lee   Created this.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <argp.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif


#include "profile.skel.h"
#include "profile.hpp"
#include <algorithm>
#include <unordered_map>
#include <thread>

#define SYM_INFO_LEN			2048

/*
 * -EFAULT in get_stackid normally means the stack-trace is not available,
 * such as getting kernel stack trace in user mode
 */
#define STACK_ID_EFAULT(stack_id)	(stack_id == -EFAULT)

#define STACK_ID_ERR(stack_id)		((stack_id < 0) && !STACK_ID_EFAULT(stack_id))

/* hash collision (-EEXIST) suggests that stack map size may be too small */
#define CHECK_STACK_COLLISION(ustack_id, kstack_id)	\
	(kstack_id == -EEXIST || ustack_id == -EEXIST)

#define MISSING_STACKS(ustack_id, kstack_id, config)	\
	(!(config).user_stacks_only && STACK_ID_ERR(kstack_id)) + (!(config).kernel_stacks_only && STACK_ID_ERR(ustack_id))

/* This structure combines sample_key_t and count which should be sorted together */
struct key_ext_t {
	struct sample_key_t k;
	__u64 v;
};

static int nr_cpus;

// Custom deleter implementations
void ProfileBPFDeleter::operator()(struct profile_bpf* obj) const {
    if (obj) {
        profile_bpf__destroy(obj);
    }
}

void BPFLinkDeleter::operator()(struct bpf_link* link) const {
    if (link) {
        bpf_link__destroy(link);
    }
}

// Registry to map thread IDs to collector instances for libbpf output capture
static std::unordered_map<std::thread::id, ProfileCollector*> profile_collector_registry;

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    // Find the collector for the current thread
    auto thread_id = std::this_thread::get_id();
    auto it = profile_collector_registry.find(thread_id);
    if (it != profile_collector_registry.end() && it->second) {
        // Capture all libbpf output into the collector's buffer
        char buffer[4096];
        int len = vsnprintf(buffer, sizeof(buffer), format, args);
        if (len > 0) {
            it->second->append_libbpf_output(std::string(buffer, std::min(len, (int)sizeof(buffer)-1)));
        }
    }
    return 0; // Don't print to stderr
}

// ProfileCollector implementation
ProfileCollector::ProfileCollector() : obj(nullptr), running(false) {
    nr_cpus = 0;
}

ProfileCollector::~ProfileCollector() {
    // Clean up registry entry if it exists
    auto thread_id = std::this_thread::get_id();
    profile_collector_registry.erase(thread_id);
}

std::string ProfileCollector::get_name() const {
    return "profile";
}

bool ProfileCollector::start() {
    if (running) {
        return true;
    }
    
    // Set current collector for libbpf output capture
    auto thread_id = std::this_thread::get_id();
    profile_collector_registry[thread_id] = this;
    libbpf_output_buffer_.clear();  // Clear any previous output
    
    int err, i;
    __u8 val = 0;

    libbpf_set_print(libbpf_print_fn);

    nr_cpus = libbpf_num_possible_cpus();
    if (nr_cpus < 0) {
        printf("failed to get # of possible cpus: '%s'!\n",
               strerror(-nr_cpus));
        return false;
    }
    if (nr_cpus > MAX_CPU_NR) {
        fprintf(stderr, "the number of cpu cores is too big, please "
            "increase MAX_CPU_NR's value and recompile");
        return false;
    }

    obj.reset(profile_bpf__open());
    if (!obj) {
        fprintf(stderr, "failed to open BPF object\n");
        return false;
    }

    /* initialize global data (filtering options) */
    obj->rodata->user_stacks_only = config.user_stacks_only;
    obj->rodata->kernel_stacks_only = config.kernel_stacks_only;
    obj->rodata->include_idle = config.include_idle;
    if (!config.pids.empty())
        obj->rodata->filter_by_pid = true;
    else if (!config.tids.empty())
        obj->rodata->filter_by_tid = true;

    bpf_map__set_value_size(obj->maps.stackmap,
                config.perf_max_stack_depth * sizeof(unsigned long));
    bpf_map__set_max_entries(obj->maps.stackmap, config.stack_storage_size);

    err = profile_bpf__load(obj.get());
    if (err) {
        fprintf(stderr, "failed to load BPF programs\n");
        goto cleanup;
    }

    if (!config.pids.empty()) {
        int pids_fd = bpf_map__fd(obj->maps.pids);
        for (size_t i = 0; i < config.pids.size() && i < MAX_PID_NR; i++) {
            if (bpf_map_update_elem(pids_fd, &(config.pids[i]), &val, BPF_ANY) != 0) {
                fprintf(stderr, "failed to init pids map: %s\n", strerror(errno));
                goto cleanup;
            }
        }
    }
    else if (!config.tids.empty()) {
        int tids_fd = bpf_map__fd(obj->maps.tids);
        for (size_t i = 0; i < config.tids.size() && i < MAX_TID_NR; i++) {
            if (bpf_map_update_elem(tids_fd, &(config.tids[i]), &val, BPF_ANY) != 0) {
                fprintf(stderr, "failed to init tids map: %s\n", strerror(errno));
                goto cleanup;
            }
        }
    }

    err = open_and_attach_perf_event(obj->progs.do_perf_event);
    if (err)
        goto cleanup;

    running = true;
    // Clear the current collector pointer since we're done with libbpf initialization
    profile_collector_registry.erase(thread_id);
    return true;

cleanup:
    profile_collector_registry.erase(thread_id);  // Clear pointer on failure
    obj.reset();
    return false;
}

ProfileData ProfileCollector::collect_data() {
    ProfileData data {"profile"};
    
    if (!running || !obj) {
        return data;
    }
    
    struct sample_key_t empty = {};
    struct sample_key_t *lookup_key = &empty;
    __u64 count;
    int err;
    int counts_fd = bpf_map__fd(obj->maps.counts);
    int stack_fd = bpf_map__fd(obj->maps.stackmap);
    
    // Collect all entries from the counts map
    std::vector<key_ext_t> items;
    
    struct sample_key_t key;
    while (bpf_map_get_next_key(counts_fd, lookup_key, &key) == 0) {
        err = bpf_map_lookup_elem(counts_fd, &key, &count);
        if (err < 0) {
            fprintf(stderr, "failed to lookup counts: %d\n", err);
            break;
        }

        if (count == 0) {
            lookup_key = &key;
            continue;
        }

        key_ext_t item;
        item.k = key;
        item.v = count;
        items.push_back(item);
        
        lookup_key = &key;
    }
    
    // Sort by count (descending)
    std::sort(items.begin(), items.end(), [](const key_ext_t& a, const key_ext_t& b) {
        return a.v > b.v;
    });
    
    // Convert to SamplingEntry format with stack traces
    for (const auto& item : items) {
        SamplingEntry entry;
        entry.key = item.k;
        entry.value = item.v;  // Changed from count to value
        entry.has_kernel_stack = !STACK_ID_EFAULT(item.k.kern_stack_id);
        entry.has_user_stack = !STACK_ID_EFAULT(item.k.user_stack_id);
        
        // Collect stack traces
        entry.user_stack.resize(config.perf_max_stack_depth);
        entry.kernel_stack.resize(config.perf_max_stack_depth);
        
        if (entry.has_user_stack) {
            if (bpf_map_lookup_elem(stack_fd, &item.k.user_stack_id, entry.user_stack.data()) != 0) {
                entry.has_user_stack = false;
                entry.user_stack.clear();
            }
        }
        
        if (entry.has_kernel_stack) {
            if (bpf_map_lookup_elem(stack_fd, &item.k.kern_stack_id, entry.kernel_stack.data()) != 0) {
                entry.has_kernel_stack = false;
                entry.kernel_stack.clear();
            }
        }
        
        data.entries.push_back(std::move(entry));
    }
    
    return data;
}


std::unique_ptr<CollectorData> ProfileCollector::get_data() {
    if (!running || !obj) {
        return std::make_unique<SamplingData>("profile", false);
    }
    
    // Collect the data from BPF maps
    ProfileData data = collect_data();
    
    // Create and return SamplingData directly without printing
    auto result = std::make_unique<SamplingData>("profile", true);
    result->entries = std::move(data.entries);
    
    return result;
}

int ProfileCollector::open_and_attach_perf_event(struct bpf_program *prog) {
	int i, fd;

    // Resize links vector to accommodate all CPUs
    links.resize(nr_cpus);

	for (i = 0; i < nr_cpus; i++) {
		if (config.cpu != -1 && config.cpu != i)
			continue;

		fd = syscall(__NR_perf_event_open, &config.attr, -1, i, -1, 0);
		if (fd < 0) {
			/* Ignore CPU that is offline */
			if (errno == ENODEV)
				continue;

			fprintf(stderr, "failed to init perf sampling: %s\n",
				strerror(errno));
			return -1;
		}

		links[i].reset(bpf_program__attach_perf_event(prog, fd));
		if (!links[i]) {
			fprintf(stderr, "failed to attach perf event on cpu: "
				"%d\n", i);
			close(fd);
			return -1;
		}
	}

	return 0;
} 