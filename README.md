# cpp-template

## Building

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -Bbuild
cmake --build build
./build/bin/GreeterStandalone --help
```

## Dependencies

- C++11 compiler :
  - Visual Studio 2019
  - GCC 9+ or Clang 10+
- [CMake](https://cmake.org/) for build system creation (>= 3.8)
- [Conan](https://conan.io/) for install packages (>= 1.0)
- [Doxygen](https://doxygen.org/) for generate documentation (>= 1.8, optional)
