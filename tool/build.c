#include "../build.h"

$cc("gcc -O2 -std=c11")
$ld("gcc -O2 -std=c11")

$bindir("bin")
$bin("sbs")

$csrc("src/main.c")

$outdir("out")

$projdir("")

start
  clean
  mkdirs
  ccompile(false)
  link
  install
  exec("mkdir -p ~/.sbs/")
  exec("cp ../build.h ~/.sbs/")
end

