# Screen Reader Project

> [!NOTE]
> This project in active development. It's not cross-platform yet (linux only, but it's a start). There are not many possibilities, and the documentation is incomplete. Any contributions will be appreciated! I have the motivation to develop it.

A cross-platform, native screen reader designed for accessibility and performance. Built with C++ and a modular architecture to support multiple platforms and accessibility APIs.

## 🌍 Language Note

*Note: I am not a native English speaker. All documentation, including this README, has been written with AI assistance to ensure clarity and proper grammar while maintaining the technical accuracy of the project descriptions.*

## 🌟 Overview

This project aims to create a modern, open-source screen reader that provides:
- **Native performance** through C++ implementation
- **Cross-platform support** (only Linux)
- **Modular architecture** for easy extensibility

## 🏗️ Architecture

### Core Design Principles
- **Interface/variant-based abstraction** - All platform-specific code wrapped in single or variant types
- **Minimum dependencies** - Only essential components, all abstracted
- **Extensible design** - Easy to add new features and platform support

## 🛠️ Building

### Prerequisites
- C++26 compatible compiler (only LLVM/clang supported)
- CMake 3.28+
- Ninja
- Platform-specific accessibility libraries (AT-SPI 2 core)

### Build Instructions
```sh
cmake -B build -G Ninja
cmake --build build
```

## 🤝 Contributing

This project is currently in active development. Contributions are welcome!

## 📄 License

[License will be specified when repository is made public]

