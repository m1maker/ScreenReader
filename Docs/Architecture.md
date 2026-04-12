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

## Core Event Processing

When an external event is received:

1. **Event Handler** detects the event category and unpacks the data variant
   - Current categories: `object event` and `keyboard event`

2. **For Object Events**:
   - The object and event type are sent to the output manager
   - The output manager invokes all its output proxies (speech, braille, etc.)
   - Events are forwarded to each output

3. **For Keyboard Events**:
   - The event handler notifies the core keyboard handler of key states
   - Calls `KeyboardHandler::Handle` with the keys/modifiers   -    -    - 

