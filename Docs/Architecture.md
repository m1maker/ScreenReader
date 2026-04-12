# Screen Reader Architecture

## Core Concept

The most important thing to understand is that this project is highly abstract in how it processes all incoming data.

The screen reader has internal proxies, structs, enums for object types/states, keycodes/modifiers, audio formats, etc. This means all incoming data from platform-dependent objects must be converted into internal types declared by the core.

This is what makes the project cross-platform.

## Proxying

Unlike the traditional approach to creating interfaces, the screen reader uses `std::variant` proxies, which are also wrapped in classes and look like regular interfaces. This means we must know all object implementations at compile time, but it allows the compiler to better optimize the code and improve runtime performance without vtable jumps.

## Core Principles

The core must not know anything about the internal implementations of platform-dependent objects. It only works with the data/proxies they provide.

## How It Works

### 1. Application Startup

When the screen reader starts, its main module `Core.App` launches one instance of `PlatformDependentWorker`, which is implemented once per platform. No `std::variant` proxies are created here.

### 2. Platform Worker Loop

The worker runs a `Loop` method that blocks the thread until the program exits. An atomic flag `g_running` is used for stopping, which the platform-dependent implementation must set to `false` when the user requests it (e.g., on Linux, this is an interrupt signal).

The `Loop` function's behavior varies by platform. For example:
- Linux keyboard input may run in a separate thread with an event listener
- The worker loop might call `atspi_event_main`, which monitors object events

### 3. Core Event Handler

The application also starts a core event handler, which:
- Launches a separate thread
- Listens to the event queue

The event handler class has a member for a platform-dependent event listener, implemented once per platform.

### 4. Event Listener Responsibilities

The event listener must:
- Subscribe to events the screen reader needs for proper operation
- Send already-converted events (in a common format) containing `std::variant` with the specific event data
- Implement methods for pushing functions to the main thread
- Implement device listening to subscribe/unsubscribe from different device events

`PushToMainThread` is used to move event handling to the main thread, but the core handles this coordination — the implementer only needs to implement these functions.

### 5. Main Program Cycle

This completes the basic program cycle.

## Required Components

For the screen reader to work correctly, at least these components must be implemented:
- One UI object class
- An audio engine

## Core

The core is the heart of the screen reader. It is completely platform-agnostic — it knows nothing about platform-specific implementations. All platform dependencies are injected through interfaces, proxies, and the platform worker.

The core handles event dispatching and routing, keyboard processing and action binding, focus management, output management (speech, braille, etc.), and application lifecycle.

---

## 1. Application Startup

When the screen reader launches, the main application module performs the following steps:

- Starts a single instance of the platform-dependent worker on the main thread. This worker blocks the thread and monitors system events.
- Launches the event handler on a separate thread.
- The event handler initializes the platform event listener, which subscribes to accessibility events from the operating system.
- The program continues running until the global running flag is set to false.

---

## 2. Event Flow Architecture

Events flow through the system in a unidirectional manner:

1. A physical event occurs (key press, focus change, value update)
2. The platform event listener receives a callback from the OS
3. The event is converted to an internal, platform-independent event type
4. The event is pushed into a thread-safe event queue
5. The event handler pulls events from the queue in a separate thread
6. Depending on event type, it is routed to either the output manager or the keyboard handler

---

## 3. Event Handler Loop

The event handler runs continuously in its own thread. It blocks waiting for events to arrive in the queue. When an event is received, it checks the event type:

- **Object events** (focus changes, value changes, state changes) are pushed to the main thread for processing. This is required because accessibility APIs typically need main thread affinity.
- **Keyboard events** are processed directly on the event handler thread, as they do not require main thread interaction.

This separation ensures responsiveness while maintaining compatibility with platform requirements.

---

## 4. Object Event Processing

When an object event arrives, the event handler examines the event type:

**Focus gained events** trigger a focus change sequence:
- First, the handler checks if focus actually changed. If the same object is already focused, nothing happens.
- Current speech and output are stopped immediately.
- The focus manager updates its record of the currently focused object.
- The output manager announces the new location.

**Other object events** (value changed, state changed, text changed) are only announced if the object that generated the event currently has focus. This prevents the screen reader from speaking irrelevant background changes.

---

## 5. Focus Manager

The focus manager tracks the currently focused UI object. It maintains two key pieces of information:

- The current focus object itself
- The context chain — the complete hierarchy from the application root down to the focused object

The context chain allows the screen reader to understand where the user is within the application structure. When focus moves, the focus manager can determine which parts of the hierarchy are new and which are unchanged.

Objects are compared by identity, not by name or position, ensuring accurate focus tracking even when multiple objects have similar names.

---

## 6. Output Manager

The output manager broadcasts events to all registered output proxies. Each output proxy handles events according to its medium:

- **Speech output** converts object information into spoken text using message builder and speech system

The output manager provides three main operations:

- **Output** sends an object event to all proxies
- **Stop** immediately halts all current output (speech, braille rendering)
- **WhereAmI** announces the user's current position without an external event

### Context Announcement

When the user moves to a new object, the output manager determines what to announce by comparing the old and new context chains. It finds the point where the chains diverge and announces only the new objects in reverse order (from the outermost changed object to the focused one). This creates natural-sounding announcements like "Dialog, OK Button" instead of repeating "Window, Dialog, OK Button" every time.

---

## 7. Keyboard Handler

The keyboard manager processes all keyboard input and executes actions associated with hotkeys.

### Key State Tracking

The handler maintains an array tracking which keys are currently pressed and the current state of all modifiers (Control, Alt, Shift, and the generic screen reader modifier key).

### Hotkey System

Actions are registered with specific key combinations. Each registration specifies:
- The keycode and required modifiers
- Which action to execute when the combination is pressed
- Whether the key combination should be hooked (intercepted) or passed through to applications

A special "any key" registration allows actions to be triggered regardless of which key is pressed.

### Hooked vs Pass-Through Keys

When a hooked key combination is pressed, the screen reader consumes the event and does not forward it to applications. This allows the screen reader to use key combinations without interfering with normal application use.

When an unhooked key is pressed, the screen reader processes it normally but also forwards it through a virtual input device so the active application receives it. This allows the screen reader to monitor all keyboard activity without blocking normal typing.

### Action Execution Flow

When a key press arrives:
- The handler updates the key state and modifier tracking
- It looks up whether the current key combination has a registered action
- If an action exists, it executes the action
- If the combination is hooked, processing stops
- If not hooked, the key is forwarded to the virtual device

---

## 8. Event Queue

The event queue is a thread-safe, blocking queue that mediates communication between the platform listener and the event handler.

The platform listener thread pushes events into the queue. The event handler thread pops events from the queue, blocking if no events are available.

The queue uses custom memory management with memory pools to reduce allocation overhead and prevent memory fragmentation during high-frequency event processing.

---

## 9. Core Singletons

The core is built around several singleton components, each with a single responsibility:

Each singleton is globally accessible but carefully controlled to prevent race conditions.

---

## 10. Error Handling Strategy

The core uses a consistent error handling approach across all components:

**Platform errors** are converted from platform-specific error codes to a unified error enum. When a platform error occurs, it is pushed to a central error handler that decides whether to log, retry, or exit.

**Object errors** use an expected/result pattern, returning either a valid value or an error code. Common object errors include:
- Object no longer exists (defunct)
- Operation not supported by this object type
- General failure

**Audio engine errors** follow the same pattern, with additional error types for device unavailability.

All errors are logged with appropriate severity levels for debugging and monitoring.

---

## Summary: Core Characteristics

| Aspect | Description |
|--------|-------------|
| Platform Agnostic | The core contains no platform-specific code |
| Thread-Safe | Event queue safely handles cross-thread communication |
| Proxy-Based | Objects are accessed through proxy interfaces |
| Singleton Pattern | Major components have single, globally accessible instances |
| Extensible | New outputs and actions can be added without core changes |
| Event-Driven | Everything is triggered by events flowing through the queue |

# Speech System, Audio System, Utterance, and Message Builder

## Overview

The speech and audio systems work together to convert UI information into spoken audio. The Message Builder constructs correct sentences from object properties, the Utterance system formats these sentences with SSML (Speech Synthesis Markup Language), the Speech System manages the speech queue and engine communication, and the Audio System handles low-level audio playback.

---

## Speech System

### Purpose

The speech system is the high-level manager for text-to-speech operations. It receives text messages, manages a queue of pending speech requests, and communicates with the underlying speech engine.

### Architecture

The speech system runs on its own dedicated thread. This prevents speech synthesis from blocking the main event handling thread and allows multiple speech requests to be queued and processed in order.

### Queue Management

When a speech request arrives, it is placed in a thread-safe queue. The speech thread waits on this queue, waking up when new messages arrive. Each message contains:

- The text content to speak
- An interrupt flag indicating whether current speech should stop
- An SSML flag indicating whether the message contains SSML markup

### Interrupt Behavior

When the interrupt flag is set, the speech system calls the engine stop function before speaking the new message. This allows urgent announcements (like focus changes) to cut off less important speech (like long document reading).

The speech system also provides a direct interrupt method that pushes an interrupt message to the queue and calls the audio system to stop playback.

### Thread Safety

All queue operations are protected by mutex locks. The condition variable ensures efficient waiting without busy-looping.

---

## Audio System

### Purpose

The audio system is the low-level component that actually plays audio data to the sound card. It receives raw audio data from the speech engine, buffers it, and writes it to the platform audio device.

### Audio Parameters

The audio system uses a standard set of parameters:
- Audio format (16-bit signed integer, 32-bit float, etc.)
- Number of channels (mono, stereo)
- Sample rate (typically 44100 Hz or 48000 Hz)
- Period size (buffer chunk size)

### Chunk-Based Processing

Audio data is processed in fixed-size chunks. When the speech engine produces audio samples, they are collected into chunks. When a chunk reaches the maximum size, it is pushed into the audio queue.

This chunk-based approach provides several benefits:
- Reduces the number of write operations to the audio device
- Allows smooth interruption by discarding incomplete chunks
- Enables efficient memory usage

### Channel Separation

The audio system supports multiple independent audio channels. Each channel has its own stop flag. This allows different sound sources to be controlled separately.

When a stop request arrives for a specific channel, the audio system:
- Sets the channel's stop flag to prevent new chunks from playing
- Filters the queue, removing any pending chunks from that channel
- Leaves chunks from other channels untouched

### Thread Model

The audio system runs on its own thread, separate from both the main thread and the speech thread. It waits on a condition variable for new audio chunks to arrive. When a chunk is available, it writes the data to the platform audio device.

### Audio Engine Abstraction

The audio system uses a platform-dependent audio engine interface. Concrete implementations exist for different platforms:
- ALSA on Linux

The audio engine provides basic operations:
- Initialize with specific parameters
- Write audio data
- Wait for playback completion
- Drop current playback
- Uninitialize and clean up

---

## Utterance System

### Purpose

The utterance system is responsible for building SSML (Speech Synthesis Markup Language) documents. SSML allows precise control over how text is spoken, including prosody, pauses, and pronunciation.

### SSML Generation

When SSML mode is enabled, the utterance system wraps text content in proper SSML tags. It provides methods for:

- Adding plain text with automatic escaping
- Inserting pauses (breaks) of specified duration
- Adjusting speaking rate
- Modifying pitch
- Changing volume

The utterance system builds the SSML document incrementally. When content generation is complete, it properly closes all open tags.

### Integration with Message Builder

The message builder creates an utterance object for each message when SSML is enabled. As content is appended, the utterance system transparently wraps it with appropriate SSML markup.

---

## Message Builder

### Purpose

The message builder is responsible for converting UI object properties into human-readable text announcements. It transforms raw accessibility data into natural sentences appropriate for spoken output.

### Memory Management

The message builder uses a monotonic buffer resource for memory allocation. This pool allocator provides fast allocations and automatic cleanup when the message is destroyed. A fixed-size buffer array provides the backing storage, eliminating dynamic heap allocations for typical messages.

### Message Creation

The message builder creates message objects that encapsulate:
- A content string (plain text or SSML)
- An utterance object for SSML generation
- Memory pool reference for allocations

Messages support appending formatted text using format string syntax, similar to printf or std::format.

### Announcement Types

The message builder provides specialized methods for different announcement types:

**Focus Announcement** announces that an object has received keyboard focus. This typically includes the object's name, role, and any relevant state information.

**State Announcement** announces changes in object state, such as checked/unchecked, expanded/collapsed, or enabled/disabled.

**Selection Announcement** announces when an item is selected in a combobox.

**Value Announcement** announces the current value of a slider, spin box, or other value-based control.

**Name Announcement** announces just the object's name, useful when other properties haven't changed.

**Description Announcement** announces the object's description, providing additional context.

**Text Announcement** announces text content, such as the contents of a text area or document.

**Text Selection Announcement** announces which text is currently selected.

**Cursor Announcement** announces the current cursor position within text.

### Hierarchical Announcement Discovery

The message builder can traverse the object hierarchy to find meaningful content to announce. When an object itself has no name or description, it walks up the parent chain until it finds an object that does. This prevents announcing empty or useless information.

### Text Position Announcement

For text objects, the message builder can announce the current cursor position. It determines the appropriate granularity (character, word, line, paragraph) and retrieves the text at that position.

---

## Complete Speech Flow

When an object event occurs and the output manager determines it should be spoken, the following sequence happens:

1. The output manager calls the speech output proxy with the object event.

2. The speech output proxy uses the message builder to create a new message.

3. Depending on the event type, the message builder calls appropriate announcement methods:
   - Focus event → BuildFocusAnnouncement
   - State change → BuildStateAnnouncement
   - Value change → BuildValueAnnouncement
   - etc.

4. Each announcement method appends relevant information to the message:
   - Object name
   - Object role (button, checkbox, etc.)
   - Current state
   - Current value
   - Additional context

5. The message is converted to a string (plain text or SSML).

6. The speech system receives the string with appropriate flags:
   - Interrupt flag set to true for focus changes
   - SSML flag set if configured

7. The speech system queues the message and notifies its worker thread.

8. The speech thread processes the queue, calling the speech engine to synthesize the text.

9. The speech engine produces raw audio data.

10. The audio system receives the audio data, chunks it, and queues it.

11. The audio thread writes chunks to the platform audio device.

12. Sound is produced through the speakers.

---

## Summary

| Component | Responsibility |
|-----------|---------------|
| Speech System | Queues speech requests, manages speech thread, communicates with speech engine |
| Audio System | Queues audio chunks, manages audio thread, writes to platform audio device |
| Utterance | Builds SSML documents for precise speech control |
| Message Builder | Converts UI object properties into readable text announcements |
| Speech Engine | Converts text to raw audio data |
| Audio Engine | Writes raw audio to sound card (platform-dependent) |

