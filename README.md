# Screen Reader Project

> [!NOTE]
> This project in active development. It's not cross-platform yet (linux only, but it's a start). There are not many possibilities, and the documentation is incomplete. Any contributions will be appreciated! I have the motivation to develop it.

A cross-platform, native screen reader.

## Why?

Of course, we already have screen readers. However, they are united by the fact that each of them was created for a specific OS/accessibility interface and they have almost no abstractions.
I'm trying to make sure that I abstract the SR core as much as possible from platform-dependent libraries in order to run it not only on Linux in the future.
Of course, I'm not saying that this is a production ready project and I still have a lot of work to do, but still I hope it's not in vain.

## Architecture

In general, the project can be divided into two parts: the core and the platform implementation.
Please note that platform implementations interact with the core in different ways: someone just uses interfaces/`std::variant` abstractions, but the main accessibility object, or some parts of the event listener implementations uses an asynchronous request queue.

## Building and running

### Prerequisites
- C++26 compatible compiler (only LLVM/clang supported).
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

### Just using

Please note that this is not production ready and do not be surprised if something does not work at all or has stopped working. The project has gone through a lot of rewrites, and still some components are either completely changed or removed altogether. As long as you see this message in the README, it means that this SR cannot be used every day yet and it is very, very, very unfinished.

## License

GPL-3.0
