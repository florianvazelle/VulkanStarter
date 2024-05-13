#!/usr/bin/env -S just --justfile

# === Variables ===

# Python virtualenv
venv_dir := justfile_directory() / "venv"

# Local directories for exports
build_dir := replace(justfile_directory() / "build", "\\", "/")
dist_dir := replace(justfile_directory() / "dist", "\\", "/")

# === Commands ===

# Display all commands
@default:
    echo -e "OS: {{ os() }} - ARCH: {{ arch() }}\n"
    just --list

# === Python ===
#
# Recipes for working with Python and Python packages.
# These recipes ensure that Python packages are installed within a virtual environment,
# providing a clean and isolated environment.

export PIP_REQUIRE_VIRTUALENV := "true"

# Python virtualenv wrapper
[private]
@venv *ARGS:
    [ ! -d {{ venv_dir }} ] && python3 -m venv {{ venv_dir }} && touch {{ venv_dir }}/.gdignore || true
    . {{ venv_dir }}/bin/activate && {{ ARGS }}

# Run files formatters
fmt:
    just venv pip install pre-commit==3.*
    just venv pre-commit run -a

# === Conan ===
#
# Recipes for managing the Conan binary.

# Conan wrapper
conan *ARGS:
    just venv pip install conan==1.*
    just venv conan {{ ARGS }}

# === Build ===
#
# Recipes for building the project.

# Create directories
makedirs:
    cmake -E make_directory {{ build_dir }} {{ dist_dir }}

# Configure the build
configure: makedirs
    cmake -B {{ build_dir }} -DBUILD_TESTING="ON" -DBUILD_DOCS="ON" -DINSTALL_APPS="ON" -DCMAKE_INSTALL_PREFIX="{{ dist_dir }}"

# Build the project as debug
debug: configure
    cmake --build {{ build_dir }} --config Debug

# Build the project as release and strip it
release: configure
    cmake --build {{ build_dir }} --config Release
    cmake --install {{ build_dir }} --strip

# === Test ===
#
# Recipes for testing the project.

# Run unit tests
test:
    ctest --test-dir {{ build_dir }} --timeout 10 -C Release -j4

# === Clean ===
#
# Recipes for cleaning up the project, removing files and folders created by this Justfile.

# Remove any unnecessary files
clean:
    rm -rf {{ build_dir }} {{ dist_dir }}

# === CI ===
#
# Recipes triggered by CI steps.

[private]
ci-row-gcc version:
    #!/bin/bash
    # NOTE: g++-12 don't seems available on ubuntu-20.04
    if [[ ! "{{ version }}" == "12" ]]; then echo '{"compiler": "gcc", "version": "{{ version }}", "cxx": "g++-{{ version }}"}'; fi

[private]
@ci-row-clang version:
    echo '{"compiler": "clang", "version": "{{ version }}", "cxx": "clang++-{{ version }}"}'

# Display the compatibility matrix for compiler versions
@ci-matrix:
    # GCC (https://gcc.gnu.org/releases.html)
    for i in $(seq 7 13); do just ci-row-gcc $i; done
    # Clang (https://releases.llvm.org/)
    for i in $(seq 9 17); do just ci-row-clang $i; done
