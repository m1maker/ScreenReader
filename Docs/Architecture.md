# Screen Reader Architecture

## Core Concept

The most important thing to understand is that this project is highly abstract in how it processes all incoming data.

The screen reader has internal proxies, structs, enums for object types/states, keycodes/modifiers, audio formats, etc. This means all incoming data from platform-dependent objects must be converted into internal types declared by the core.

This is what makes the project cross-platform.

## Proxying

Unlike the traditional approach to creating interfaces, the screen reader uses `std::variant` proxies, which are also wrapped in classes and look like regular interfaces. This means we must know all object implementations at compile time, but it allows the compiler to better optimize the code and improve runtime performance without vtable jumps.

## Core Principles

The core must not know anything about the internal implementations of platform-dependent objects. It only works with the data/proxies they provide.
