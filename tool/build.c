#include "../build.h"

$cc("gcc -O2 -std=c11")
$ld("gcc -O2 -std=c11")

$bindir("bin")
$bin("sbs")

$csrc("src/main.c")

$outdir("out")

start
  clean
  mkdirs
  ccompile
  link
  install
end

