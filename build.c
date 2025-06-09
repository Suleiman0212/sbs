#include "build.h"

// Set C compiler and its flags
$cc("gcc -Wall -Wextra -O2 -std=c17 -ffreestanding -fno-pie -fno-stack-protector")

// Set assembler and its flags
$as("nasm -f elf64")

// Set linker and its flags
$ld("ld -nostdlib -static -z noexecstack")

// Set the output binary name
$bin("bin/compressor")

// Add C source files
$csrc("src/file0.c", "src/file1.c")

// Add Assembly source files
$assrc("src/file2.s", "src/file3.s")

// Set output directory for object files
$outdir("out/")

start
  ccompile     // Compile C source files into .o files
  ascompile    // Assemble .s files into .o files
  link         // Link all object files into final binary
  run          // Run the resulting binary
  exec("do something")   // Executing a command
end

