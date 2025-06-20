#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void cmdhelp() {
  printf(
    "Available commands:\n"
    "   help - print this\n"
    "   install - add to current dir build.h and build.c\n"
    "   build <-Darg> - compile(via gcc) and run build.c\n"
    "   buildd <-Darg> - compile(via gcc) and run build/build.c\n"
  );  
}

void cmdinstall() {
  system("cp ~/.sbs/build.h .");
}

void cmdbuild(char *darg) {
  char cmd[256] = {0};
  if (darg != NULL) {
    snprintf(cmd, sizeof(cmd), "gcc -D%s build.c -o build", darg);
    system(cmd);
    system("./build");
  } else {
    system("gcc build.c -o build");
    system("./build");
  }
}

void cmdbuildd(char *darg) {
  char cmd[256] = {0};
  if (darg != NULL) {
    snprintf(cmd, sizeof(cmd), "gcc -D%s build/build.c -o build/build", darg);
    system(cmd);
    system("./build/build");
  } else {
    system("gcc build/build.c -o build/build");
    system("./build/build");
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
  } else if (strcmp(argv[1], "buildd") == 0) {
    cmdbuildd(argv[2]);
  } else {
    printf("Unknown command.");
    exit(1);
  }

  return 0;
}
