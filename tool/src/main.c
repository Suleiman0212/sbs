#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "const.h"

void cmdhelp() {
  printf(
    "Available commands:\n"
    "   help - print this\n"
    "   install - add to current dir build.h and build.c\n"
    "   build - compile(via gcc) and run build.c\n"
    "   build <compiler> - compile and run build.c\n"
  );  
}

void cmdinstall() {
  FILE *bh = fopen("build.h", "w");
  // FILE *bc = fopen("build.c", "w");

  if (!bh) {
    printf("File open error.");
    exit(1);
  }
  
  fprintf(bh, "%s", buildh);
  // fprintf(bc, "%s", buildc);
}

void cmdbuild(char *compiler) {
  char cmd[256] = {0};
  if (compiler != NULL) {
    snprintf(cmd, sizeof(cmd), "%s build.c -o build", compiler);
    system(cmd);
  } else {
  system("gcc build.c -o build");
  system("./build");
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) return 1;

  if (strcmp(argv[1], "help") == 0) {
    cmdhelp();
  } else if (strcmp(argv[1], "install") == 0) {
    cmdinstall();
  } else if (strcmp(argv[1], "build") == 0) {
    cmdbuild(argv[2]);
  } else {
    printf("Unknown command.");
    exit(1);
  }

  return 0;
}
