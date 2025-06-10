# `Suleko` Build System (`SBS`)

A lightweight **build system** designed specifically for the [`Suleko` Project](https://github.com/Suleiman0212/suleko).
> All what you need is a **C compiler** and **build header**!

## Features

- Simple declarative syntax for build configurations
- Support for C and Assembly projects
- Cross-platform compatibility
- Optional command-line tool for easier management

## Header Installation

### Manual Installation

Clone the repository or copy the necessary files:

```bash
git clone https://github.com/Suleiman0212/sbs
```

### Using `SBS` Tool (Recommended)

```bash
sbs install
```

This will **automatically** set up `build.h` in your current directory.

## Getting Started

### 1. Create Your Build Configuration

Create a `build.c` file with your project configuration.
Here's an example for a `nostd` project:

```c
# include "build.h"

// Set compiler and flags
$cc("gcc -Wall -Wextra -O2 -std=c11 -ffreestanding -fno-pie -fno-stack-protector")
$as("nasm -f elf64")                    // Assembler and flags
$ld("ld -nostdlib -static -z noexecstack")  // Linker and flags

// Set output paths
$bindir("bin")                          // Binary output directory
$bin("mybestOS")                        // Output binary name

// Add source files
$csrc("src/file0.c", "src/file1.c")     // C source files
$assrc("src/file2.s", "src/file3.s")    // Assembly source files

$outdir("out")                          // Object files directory

start
    clean                               // Clean previous builds
    mkdirs                              // Create output directories
    ccompile                            // Compile C sources
    ascompile                           // Assemble ASM sources
    link                                // Link object files
    install                             // Install to ~/.local/bin
    run                                 // Run the resulting binary
    exec("echo Build completed!")       // Execute custom command
end
```

### 2. Build Your Project

**Compile** and **run** your build configuration:

```bash
gcc build.c -o build && ./build
```

Or using the `SBS` tool:

```bash
sbs build
```

## `SBS` Command Line Tool

### Installation

```bash
git clone https://github.com/Suleiman0212/sbs
cd tool/
gcc build.c -o build
./build
```

Make sure **~/.local/bin** is in your `$PATH`.

### Usage

```bash
sbs <command>
```

Available commands:

| Command         | Description    |
|---------------  | --------------- |
| `Help`            | Show this help message         |
| `Install`         | Set up `build.h` and `build.c` in current directory   |
| `Build`           | Compile and execute `build.c` using default compiler   |
| `Build <compiler>`| Compile and execute `build.c` with specified compiler   |

A typical `SBS` project looks like:
text

```bash
project/
├── src/            # Source files
│   ├── *.c
│   └──*.s
├── out/            # Object files (generated)
├── bin/            # Output binaries (generated)
├── build.h         # SBS header
└── build.c         # Your build configuration
```

## License

`SBS` is **open-source** software released under the **MIT** License.
