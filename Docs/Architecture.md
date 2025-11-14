# Screen Reader Project Architecture

## Project Overview

The main idea of this Screen Reader is to be native and run on different platforms. To achieve this, I follow several principles:

1. **Written in C++** for performance and system-level access
2. **Interface-based abstraction** - everything that can be abstracted is wrapped in interfaces
3. **Minimum dependencies** - implement only what's necessary for screen reader functionality

## Core Components

### Engine Interface (`Sral::Engine`)
The central speech and braille engine that provides:

- **Speech synthesis**: `Speak()`, `StopSpeech()`, `PauseSpeech()`, `ResumeSpeech()`
- **Braille output**: `Braille()` method for tactile feedback
- **Audio generation**: `SpeakToMemory()` for direct audio buffer access
- **Configuration**: `SetParameter()`, `GetParameter()` for engine settings

### Event System

#### Base Event Interface (`IEvent`)
Foundation for all screen reader events with:
- **Event types**: Focus changes, clicks, value changes, selection changes, etc.
- **Categories**: Object events and keyboard events

#### Object Events (`CObjectEvent`)
Handles UI element changes:
- Focus gained/lost
- State changes (visibility, enabled status)
- Content changes (text, value)
- Structural changes (children added/removed)

#### Keyboard Events (`CKeyboardEvent`)
Comprehensive keyboard input handling:
- **Full keycode support**: A-Z, 0-9, function keys, navigation keys, modifiers
- **Modifier tracking**: Shift, Ctrl, Alt, Super, lock keys
- **Utility methods**: Keycode naming and modifier string generation

### Event Listener (`IEventListener`)
Platform-specific event processing:
- **Event queuing**: `Post()` method for event submission
- **Queue access**: `RequestQueue()` for event retrieval
- **Platform adaptation**: Converts native events to common IEvent format

### Object Model (`IObject`)
Represents UI elements accessible to screen readers:

#### Object Types
- **Interactive controls**: Buttons, checkboxes, radio buttons, sliders
- **Text elements**: Labels, text fields
- **Containers**: Panels, windows, dialogs, menus
- **Lists**: Combo boxes, list boxes, list items

#### Object States
Bitmask-based state system:
- **Visibility & Interaction**: `VISIBLE`, `ENABLED`, `FOCUSED`
- **Selection states**: `SELECTED`, `CHECKED`, `EXPANDED`
- **Text properties**: `READONLY`, `MULTI_LINE`, `SECURE`
- **Capability flags**: `FOCUSABLE`, `SELECTABLE`, `EDITABLE`

#### Object Hierarchy
- **Parent-child relationships**: Navigation through UI tree
//- **Bounds detection**: Screen coordinates and dimensions
- **Content access**: Names, descriptions, text content, values
- **Application context**: Source application identification

### Object Utilities
- **Object comparison**: `ObjectIsSame()` prevents duplicate announcements
- **Hierarchy navigation**: `GetDesktopObject()`, `FindFocusedObject()`

### Platform Worker (`IPlatformDependentWorker`)
Main execution loop:
- **Platform-specific implementation**: Each OS provides its own worker
- **Continuous operation**: Runs while `g_running` flag is true
- **Event integration**: Coordinates with event listeners and object model

## Architectural Benefits

### Cross-Platform Strategy
- **Interface isolation**: Platform-specific code encapsulated in implementations
- **Unified event model**: Common event format across all platforms

## Implementation Notes

- **Platform requirements**: Each OS must provide concrete implementations of interfaces
- **Object lifecycle**: Automatic cleanup and validity checking

