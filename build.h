#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define start int main() { char objsrc[1024 * 10] = {0}; system("clear"); printf("SULEKO BUILD SYSTEM.\nBUILDING %s.\n\n", binname);
#define end printf("DONE.\n"); return 0; }

#define $cc(path) const char *cc_path = path;
#define $as(path) const char *as_path = path;
#define $ld(path) const char *ld_path = path;

#define $csrc(...) const char *csrc[] = { __VA_ARGS__ };
#define $assrc(...) const char *assrc[] = { __VA_ARGS__ };

#define $outdir(path) const char *outdir = path;
#define $bin(name) const char *binname = name;

#define ccompile { \
  printf("C COMPILATION STARTED.\n"); \
  for (int i = (int)(sizeof(csrc) / sizeof(csrc[0])) - 1; i >= 0; i--) { \
    char cmd[256] = {0}; \
    char objname[128] = {0}; \
    const char *filename = strrchr(csrc[i], '/'); \
    filename = filename ? filename + 1 : csrc[i]; \
    strncpy(objname, filename, sizeof(objname) - 1); \
    char *dot = strrchr(objname, '.'); \
    if (dot) *dot = '\0'; \
    snprintf(objname + strlen(objname), sizeof(objname) - strlen(objname), ".o"); \
    snprintf(cmd, sizeof(cmd), "%s -c %s -o %s%s", cc_path, csrc[i], outdir, objname); \
    printf("EXECUTING: %s\n", cmd); \
    system(cmd); \
    strcat(objsrc, outdir); \
    strcat(objsrc, objname); \
    strcat(objsrc, " "); \
  } \
  printf("C COMPILATION FINISHED.\n\n"); \
}

#define ascompile { \
  printf("ASM COMPILATION STARTED.\n"); \
  for (int i = (int)(sizeof(assrc) / sizeof(assrc[0])) - 1; i >= 0; i--) { \
    char cmd[256] = {0}; \
    char objname[128] = {0}; \
    const char *filename = strrchr(assrc[i], '/'); \
    filename = filename ? filename + 1 : assrc[i]; \
    strncpy(objname, filename, sizeof(objname) - 1); \
    char *dot = strrchr(objname, '.'); \
    if (dot) *dot = '\0'; \
    snprintf(objname + strlen(objname), sizeof(objname) - strlen(objname), ".o"); \
    snprintf(cmd, sizeof(cmd), "%s -c %s -o %s%s", as_path, assrc[i], outdir, objname); \
    printf("EXECUTING: %s\n", cmd); \
    system(cmd); \
    strcat(objsrc, outdir); \
    strcat(objsrc, objname); \
    strcat(objsrc, " "); \
  } \
  printf("ASM COMPILATION FINISHED.\n\n"); \
}

#define link { \
  printf("LINKING STARTED.\n"); \
  char cmd[1024 * 10] = {0}; \
  snprintf(cmd, sizeof(cmd), "%s %s -o %s", ld_path, objsrc, binname); \
  printf("EXECUTING: %s\n", cmd); \
  system(cmd); \
  printf("LINKING FINISHED.\n\n"); \
}

#define run { \
  char cmd[256] = {0}; \
  snprintf(cmd, sizeof(cmd), "./%s", binname); \
  system(cmd); \
}

#define exec(cmd) { \
  system(cmd); \
} 
