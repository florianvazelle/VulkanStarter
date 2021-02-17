# VulkanStarter 

[Actions Status](https://github.com/florianvazelle/VulkanStarter/workflows/build/badge.svg)

This is a template to setting up a new Vulkan C++ project.

## Features

- Sources, headers and mains separated in distinct folders
- Use of modern [CMake](https://cmake.org/) for much easier compiling
- Automatically compile shaders with [`glslValidator`](https://github.com/KhronosGroup/glslang)
- Integrated test suite using [Doctest](https://github.com/onqtam/doctest)
- Continuous integration via [GitHub Actions](https://help.github.com/en/actions)
- Support [Conan](https://conan.io/) package manager to quick install your package

## Building

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -Bbuild
cmake --build build
./build/bin/VulkanStarterStandalone
```

### Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -Bbuild
cmake --build build
./build/bin/VulkanStarterTests
```

## Dependencies

- C++17 compiler :
    - Visual Studio 2017 or 2019
    - GCC 7+ or Clang 5+
- [CMake](https://cmake.org/) for build system creation (>= 3.8)
- [Conan](https://conan.io/) for install packages (>= 1.0)
- [Doxygen](https://doxygen.org/) for generate documentation (>= 1.8, optional)

## References

- To include ImGui : [with Conan](https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html), [with Vulkan](https://frguthmann.github.io/posts/vulkan_imgui/)
- Vulkan code sample : [VulkanTutorial](https://github.com/Overv/VulkanTutorial), [vkSandbox](https://github.com/tstullich/vk-sandbox)
- Inspirational templates : [CppProject](https://github.com/tweether/cpp-project), [ModernCppStarter](https://github.com/TheLartians/ModernCppStarter)
