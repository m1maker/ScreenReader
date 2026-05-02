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

## 🛠️ Building and running

### Prerequisites
- C++26 compatible compiler (only LLVM/clang supported)
- CMake 3.28+
- Ninja
- Platform-specific accessibility libraries (AT-SPI 2 core)

### Build Instructions
```sh
git clone https://github.com/m1maker/ScreenReader.git
cd ScreenReader
cmake -B build -G Ninja
cmake --build build
```

### Run Instructions

At the moment, you need to copy/move the espeak-ng data folder into build directory to make the built-in synthesizer work.
Then, on Linux, you need to add udev rules so that SR can intercept/handle the keyboard. To do this, you need to run this code as root.
```sh
Scripts/install-linux.sh
```

And then run the ScreenReader
```sh
build/screenreader
```

## 🤝 Contributing

This project is currently in active development. Contributions are welcome!

## 📄 License

GPL-3.0
