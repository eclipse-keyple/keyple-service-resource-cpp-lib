# Keyple Service Resource C++ Library

## Overview

The **Keyple Service Resource C++ Library** is a Keyple add-on whose goal is to provide a profile-based card resource allocation mechanism.

## Documentation & Contribution Guide

The full documentation, including the **user guide**, **download information** and **contribution guide**, is available on the Keyple website [keyple.org](https://keyple.org).

## API documentation

API documentation & class diagram is available online: [docs.keyple.org/keyple-service-resource-cpp-lib](https://docs.keyple.org/keyple-service-resource-cpp-lib)

## Examples

Examples of implementation are available in the following repository: [github.com/eclipse/keyple-cpp-example](https://github.com/eclipse/keyple-cpp-example)

## Building with Conan

This repository is packaged as a [Conan](https://conan.io/) recipe (`conanfile.py`), providing the
`Keyple::Service::Resource` CMake target as `keyple-service-resource-cpp-lib`. It requires
`keyple-service-cpp-lib/3.4.1` and `keyple-util-cpp-lib/2.4.1` (both declared with `transitive_headers=True`; the
latter is required directly, not just inherited through `keyple-service-cpp-lib`, because this library's own sources
call `keyple-util-cpp-lib` symbols directly — Conan doesn't propagate a shared dependency's own libs transitively by
default), resolved via Conan instead of `FetchContent`. Downstream keyple recipes should consume this package the
same way, with `self.requires("keyple-service-resource-cpp-lib/3.1.1")`. The `shared` option (default `True`)
controls whether a shared or static library is built, and `fPIC` is available for static, non-Windows builds.

This library has no unit test suite, so there is no `with_tests` option.

### Local development

Use `conan build` to configure/compile in-tree, in a regular `build/` folder you can inspect, rerun and debug
against (and which generates a `CMakeUserPresets.json` for IDE integration). The build type defaults to `Release`;
pass `-s build_type=Debug` for a debug build, which lands in its own `build/Debug` subfolder alongside `build/Release`:

```bash
conan build . --build=missing
conan build . -s build_type=Debug --build=missing
```

### Packaging to the local cache

Use `conan create` to export the recipe and package to your local Conan cache, for consumption by other
recipes/repos (no remote upload involved):

```bash
# Build and export the package to the local cache
conan create . --build=missing

# Package a static build instead (default is shared)
conan create . -o shared=False --build=missing
```

### Conan profile

Builds use your default Conan profile. If you don't have one yet, create it with:

```bash
conan profile detect
```

## About the source code

The code is built with **GCC/CLang/MSVC** in order to address a wide range of applications.
