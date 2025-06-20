#include "build.h"

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

$projdir("proj")

start
    clean                               // Clean previous builds
    mkdirs                              // Create output directories
    ccompile(true)                      // Compile C sources
    ascompile                           // Assemble ASM sources
    link                                // Link object files
    install                             // Install to ~/.local/bin
    run                                 // Run the resulting binary
    exec("echo Build completed!")       // Execute custom command
end
