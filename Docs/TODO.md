# Screen Reader Project TODO List

Some minor details will not be described here. I have outlined the general overview of what I plan to implement.
Items that have already been completed will be marked as checked.

## AT-SPI Input Implementation
- [x] Complete AT-SPI input event handling
- [x] Implement keyboard event capture via AT-SPI

## Event Handler Keyboard Integration
- [ ] Integrate keyboard handling into main event handler
- [ ] Implement key combination recognition
- [ ] Add modifier key state tracking

## Advanced Element Vocalization
- [ ] Implement table reading with row/column navigation
- [ ] Add list item counting and navigation
- [ ] Create cursor position tracking and announcement (read by word or line and other)
- [ ] Implement selection change detection and reading
- [ ] Add webview content extraction and reading

## AT-SPI Method Implementation
- [ ] Implement missing AT-SPI methods in object model
- [ ] Add text interface support (read by word and other)
- [ ] Add selection interface for list items

## Testing
- [ ] Test with popular Linux applications (Firefox, LibreOffice, Terminal)
- [ ] Benchmark performance with complex UI hierarchies

## Basic Configuration System
- [ ] Design configuration file format (JSON|| YAML || TOML)
- [ ] Implement configuration file parsing
- [ ] Create settings management class
- [ ] Add runtime configuration reloading

## Hotkey System
- [ ] Design hotkey configuration structure
- [ ] Implement hotkey registration system
- [ ] Create conflict detection for hotkeys

## Writable Object Interface
- [ ] Design IObjectWritable interface with setters

## Linux GUI Implementation (CObjectGtk)
- [ ] Create GTK-based IObjectWritable implementation
- [ ] Implement settings dialog
- [ ] Create settings profile management

## Enhanced Testing & Validation
- [ ] Perform extensive cross-application testing
- [ ] Validate with complex web applications
- [ ] Performance testing with large documents

## Object Navigator
- [ ] Design object navigation interface
- [ ] Implement object tree exploration
- [ ] Add object property inspection

## Plugin System & Scripting
- [ ] Design plugin architecture
- [ ] Implement AngelScript integration
- [ ] Create scripting API for screen reader actions
- [ ] Add plugin lifecycle management
- [ ] Implement plugin configuration system

## Hotkey Action Expansion
- [ ] Add comprehensive default hotkey assignments
- [ ] Implement action categories (navigation, reading, configuration)
- [ ] Create context-aware action system
- [ ] Add user-customizable action sequences

## Documentation Completion
- [ ] Finish user documentation
- [ ] Create developer API documentation
- [ ] Document plugin development process

## Repository Preparation
- [ ] Clean up codebase and remove debug artifacts
- [ ] Create proper licensing and contribution guidelines

## Community Launch
- [ ] Open source repository to public

