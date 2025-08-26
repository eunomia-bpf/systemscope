# Zero-Instrument Profiler Tool - UI/UX Design

## 🎯 MVP-First Design Philosophy

### **Core Principles**
- **Progressive Disclosure**: Start with essential features, reveal complexity gradually
- **Minimal Cognitive Load**: Show only what users need for their current task
- **Extensible Interface**: UI framework supports easy addition of new features
- **Data as a View Type**: Data display is just another view type, not separate
- **Single Source of Truth**: One analyzer, one view, clear focus

## 👥 **Simplified User Personas (MVP)**

### **Primary MVP User: Performance Developer**
- **Background**: Developer investigating performance issues
- **Goals**: Quick flame graph analysis, identify hot functions, export findings
- **Workflow**: Start profiling → View 3D flame graph → Explore data → Export/share
- **Pain Points**: Complex UIs, information overload, slow setup

### **Secondary Users (Post-MVP)**
- **Systems Engineer**: Multi-analyzer workflows
- **Performance Expert**: Cross-analyzer correlation
- **Team Lead**: Session sharing and collaboration

## 🖥️ **MVP Layout Design**

### **Simplified Application Structure**

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│ menu          [Help] [Export] [Settings]                                            │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────────┐ ┌─────────────────────────────────────────────────────────┐ │
│ │ Control Panel       │ │ Viewport                                                │ │
│ │ ┌─────────────────┐ │ │ ┌───────────────────────────────────────────────────┐ │ │
│ │ │ Analyzer        │ │ │ │                                                   │ │ │
│ │ │ [Start] [Stop]  │ │ │ │           Current View                            │ │ │
│ │ │ Status: Ready   | | | |                                                   | | |
| | |  config...      │ │ │ │(2D/3D Flame Graph OR line/plot charts OR Data Table)    │ │ │
│ │ └─────────────────┘ │ │ │                                                   │ │ │
│ │ ┌─────────────────┐ │ │ │                                                   │ │ │
│ │ │ View Selector   │ │ │ │                                                   │ │ │
│ │ │ ○ 3D Flame      │ │ │ │                                                   │ │ │
│ │ │ ○ Data Table    │ │ │ │                                                   │ │ │
│ │ └─────────────────┘ │ │ └───────────────────────────────────────────────────┘ │ │
│ │                     │ │ ┌───────────────────────────────────────────────────┐ │ │
│ │                     │ │ │ View-Specific Controls                            │ │ │
│ │                     │ │ │ (Only for active view)                            │ │ │
│ │                     │ │ │                                                   │ │ │
│ │                     │ │ └───────────────────────────────────────────────────┘ │ │
│ └─────────────────────┘ └─────────────────────────────────────────────────────────┘ │
├─────────────────────────────────────────────────────────────────────────────────────┤
│ Status: Ready │ Samples: 0 │ Duration: 00:00 │ View: 3D Flame Graph                │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### **Progressive Enhancement Path**

#### **MVP (Phase 1): Single Analyzer, Single View**
```
Simple Header
├── Analyzer Controls (FlameGraph only)
├── View Selector (3D Flame OR Data Table)  
├── Current View Display
└── Basic Status Bar
```

#### **Phase 2: Multi-Analyzer Support**
```
Enhanced Header
├── Analyzer Tabs (Flame | Trace | Metrics)
├── View Selector (matched to active analyzer)
├── Current View Display
└── Enhanced Status Bar
```

#### **Phase 3: Multi-Viewport Support**
```
Full Header + Session Tabs
├── Analyzer Management Panel
├── Viewport Layout Controls
├── Multi-Viewport Display
└── Comprehensive Status Bar
```

## 🎨 **MVP Component Design**

### **1. Simplified Header**
```
┌─────────────────────────────────────────────────────────────────────────────┐
│ 🔥 Zero-Instrument Profiler                       [📤 Export] [⚙️ Settings] │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Features**:
- Clean branding
- Essential actions only
- No session tabs (MVP = single session)

### **2. Streamlined Control Panel**

#### **Analyzer Control (MVP)**
```
┌─────────────────────────────────────────────────────────────┐
│ Flame Graph Analyzer                                        │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ Status: ● Ready                                         │ │
│ │                                                         │ │
│ │ [▶️ Start Profiling]  [⚙️ Configure]                     │ │
│ │                                                         │ │
│ │ Duration: 30s  [○ Continuous]                          │ │
│ │ Frequency: 99Hz                                        │ │
│ │                                                         │ │
│ │ [⏹️ Stop] (when running)                                │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

#### **View Selector (MVP)**
```
┌─────────────────────────────────────────────────────────────┐
│ View Type                                                   │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ ● 3D Flame Graph                                        │ │
│ │   Interactive 3D visualization                          │ │
│ │                                                         │ │
│ │ ○ Data Table                                            │ │
│ │   Raw data exploration                                  │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

#### **Dynamic View Controls (MVP)**
```
// When 3D Flame Graph is selected:
┌─────────────────────────────────────────────────────────────┐
│ 3D Controls                                                 │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ Z-Spacing: [──●────] 25                                 │ │
│ │ Min Count: [●──────] 10                                 │ │
│ │ Max Depth: [────●──] 8                                  │ │
│ │                                                         │ │
│ │ Color: [Hot/Cold ▼]                                     │ │
│ │ Threads: [All ▼]                                        │ │
│ │                                                         │ │
│ │ [🎯 Reset Camera]  [📐 Fit All]                         │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘

// When Data Table is selected:
┌─────────────────────────────────────────────────────────────┐
│ Table Controls                                              │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ Search: [function name...] [🔍]                         │ │
│ │                                                         │ │
│ │ Sort by: [Time ▼]  Order: [Desc ▼]                     │ │
│ │                                                         │ │
│ │ Show: [Top 100 ▼]  [📊 Export CSV]                     │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### **3. Main Viewport (Single View)**

#### **3D Flame Graph View**
```
┌─────────────────────────────────────────────────────────────────────────────┐
│ 3D Flame Graph                                                              │
├─────────────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────────────────────┐ │
│ │                                                                         │ │
│ │                    3D Flame Stack Visualization                        │ │
│ │                                                                         │ │
│ │                         [🎮 3D Scene]                                   │ │
│ │                                                                         │ │
│ │  [Hover info overlay when hovering over function blocks]               │ │
│ │                                                                         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
│ ┌─────────────────────────────────────────────────────────────────────────┐ │
│ │ Selection Info: function_name() | 234ms (12.3%) | 1,247 calls         │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### **Data Table View** (Data as a View Type)
```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Data Table                                                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────────────────────┐ │
│ │ Function Name          │ Self Time │ Total Time │ Call Count │ Thread   │ │
│ │ ───────────────────────┼───────────┼────────────┼────────────┼───────── │ │
│ │ main()                 │ 45ms      │ 1,234ms    │ 1          │ main     │ │
│ │ ├─ processData()       │ 234ms     │ 567ms      │ 3          │ main     │ │
│ │ │  ├─ parseInput()     │ 123ms     │ 123ms      │ 15         │ main     │ │
│ │ │  └─ validateData()   │ 89ms      │ 210ms      │ 15         │ main     │ │
│ │ ├─ compute()           │ 345ms     │ 456ms      │ 1          │ worker_1 │ │
│ │ └─ optimizeAlgo()      │ 456ms     │ 456ms      │ 5          │ worker_2 │ │
│ └─────────────────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

### **4. Simplified Status Bar**
```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Status: ● Running │ Samples: 4,567 │ Duration: 00:15:32 │ View: 3D Flame   │
└─────────────────────────────────────────────────────────────────────────────┘
```

## 🔄 **Progressive UI Enhancement**

### **Phase 1 → Phase 2: Adding Multi-Analyzer**

#### **Enhanced Control Panel**
```
┌─────────────────────────────────────────────────────────────┐
│ Active Analyzer: [Flame Graph ▼]                           │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ Flame Graph Analyzer    [● Running]                     │ │
│ │ [⏸️ Pause] [⏹️ Stop] [⚙️ Configure]                       │ │
│ │                                                         │ │
│ │ Trace Analyzer          [○ Stopped]                     │ │
│ │ [▶️ Start] [⚙️ Configure]                                │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

#### **Context-Aware View Selector**
```
┌─────────────────────────────────────────────────────────────┐
│ Available Views (for Flame Graph data):                    │
├─────────────────────────────────────────────────────────────┤
│ ● 3D Flame Graph                                           │
│ ○ Data Table                                               │
│ ○ 2D Flame Graph       [Available in Phase 2]             │
│                                                             │
│ Views for Trace data:                                      │
│ ○ Timeline Chart        [Start Trace Analyzer first]       │
│ ○ Data Table            [Available for all data types]     │
└─────────────────────────────────────────────────────────────┘
```

### **Phase 2 → Phase 3: Adding Multi-Viewport**

#### **Layout Controls**
```
┌─────────────────────────────────────────────────────────────┐
│ Layout: ● Single ○ Split ○ Grid                            │
├─────────────────────────────────────────────────────────────┤
│ Active Views:                                               │
│ ┌─────────────────────────────────────────────────────────┐ │
│ │ View 1: 3D Flame Graph (Flame data)     [×]            │ │
│ │ View 2: Timeline Chart (Trace data)     [×]            │ │
│ │ [+ Add View]                                            │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## 🎯 **Information Hierarchy & Reduction**

### **Primary Information (Always Visible)**
- Analyzer status (running/stopped)
- Current view type
- Basic analyzer controls (start/stop)

### **Secondary Information (On-Demand)**
- Detailed analyzer configuration
- Advanced view controls
- Data statistics

### **Tertiary Information (Progressive Disclosure)**
- Export options
- Sharing features
- Advanced settings

### **Information Reduction Strategies**

#### **1. Smart Defaults**
```typescript
// Default configuration that works for 80% of users
const defaultFlameConfig = {
  duration: 30, // seconds
  frequency: 99, // Hz
  continuous: false,
  minCount: 10,
  maxDepth: 20
}
```

#### **2. Contextual Controls**
- Show only relevant controls for active view
- Hide analyzer controls when not applicable
- Progressive disclosure of advanced options

#### **3. Visual Hierarchy**
- Primary actions: Prominent buttons (Start/Stop)
- Secondary actions: Smaller buttons (Configure)
- Tertiary actions: Menu items (Export, Settings)

## 🔌 **Data Display as View Type**

### **View Type System**
```typescript
interface ViewType {
  id: string
  name: string
  component: React.ComponentType
  supportedDataTypes: DataType[]
  icon: string
  description: string
}

const availableViews: ViewType[] = [
  {
    id: '3d-flame',
    name: '3D Flame Graph',
    component: FlameGraph3D,
    supportedDataTypes: ['flamegraph', 'stacktrace'],
    icon: '🔥',
    description: 'Interactive 3D flame stack visualization'
  },
  {
    id: 'data-table',
    name: 'Data Table',
    component: DataTable,
    supportedDataTypes: ['flamegraph', 'trace', 'metrics', 'static'],
    icon: '📊',
    description: 'Raw data exploration and analysis'
  },
  {
    id: '2d-flame',
    name: '2D Flame Graph',
    component: FlameGraph2D,
    supportedDataTypes: ['flamegraph', 'stacktrace'],
    icon: '📈',
    description: 'Traditional horizontal flame graph'
  }
]
```

### **View-Data Compatibility Matrix**
```
Data Type    │ 3D Flame │ Data Table │ 2D Flame │ Timeline │ Metrics
─────────────┼───────────┼────────────┼───────────┼──────────┼─────────
FlameGraph   │     ✅    │     ✅     │     ✅    │    ❌    │    ❌
Trace        │     ❌    │     ✅     │     ❌    │    ✅    │    ❌
Metrics      │     ❌    │     ✅     │     ❌    │    ✅    │    ✅
Static       │     ❌    │     ✅     │     ❌    │    ❌    │    ❌
```

### **Dynamic View Selection**
```typescript
// Views are filtered based on available data
const getCompatibleViews = (dataType: DataType): ViewType[] => {
  return availableViews.filter(view => 
    view.supportedDataTypes.includes(dataType)
  )
}

// Data Table is universal - supports all data types
// Other views are specialized for specific data types
```

## 🚀 **Benefits of This MVP-First Design**

### **1. Immediate Value**
- Users can start profiling with minimal learning curve
- Clear, focused interface reduces confusion
- Fast time-to-insight for basic use cases

### **2. Extensible Foundation**
- Plugin system allows easy addition of new analyzers/views
- UI framework supports progressive enhancement
- No major refactoring needed for advanced features

### **3. Reduced Cognitive Load**
- Show only what's needed for current task
- Progressive disclosure of advanced features
- Smart defaults handle common scenarios

### **4. Data-Centric Approach**
- Data Table as universal view type
- All data types can be explored consistently
- Clear separation between data and visualization

### **5. Clear Upgrade Path**
- MVP → Multi-Analyzer → Multi-Viewport → Sessions
- Each phase adds value without breaking existing workflows
- Users can adopt new features at their own pace

This MVP-first design provides immediate value while establishing a solid foundation for future enhancements. The modular view system ensures that data display is just another view type, maintaining consistency and extensibility. 