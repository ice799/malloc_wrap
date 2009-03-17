//
// Build it like this:
// gcc -O2 -Wall -ldl -fPIC -o malloc_wrap.so -shared malloc_wrap.c
//

#define  _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void __attribute__ ((constructor)) init(void);
void __attribute__ ((destructor)) fini(void);

static int fd;
static void *(*callocp)(size_t count, size_t size);
static void *(*reallocp)(void *ptr, size_t size);
static void *(*mallocp)(size_t size);
static void *(*freep)(void *);


void init(void) {
  char file[256];
  snprintf(file, 256, "/tmp/malloc-footprint.%d", getpid());
  fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  printf("file: %s len: %lu fd: %d\n", file, strlen(file), fd);
  if (fd == -1)
    printf("ok: %s\n", strerror(errno));

  reallocp = dlsym(RTLD_NEXT, "realloc");
  mallocp = dlsym(RTLD_NEXT, "malloc");
  callocp = dlsym(RTLD_NEXT, "calloc");
  freep = dlsym(RTLD_NEXT, "free");
}

void fini(void) {
  close(fd);
}


void *realloc(void *ptr, size_t size)
{
    void *tmp;

    tmp = ptr;
    ptr = reallocp(ptr, size);
    dprintf(fd, "x%lx = realloc(x%lx, %zu);\n", (unsigned long)ptr, (unsigned long)tmp, size);
    return ptr;
 
}

void *calloc(size_t count, size_t size)
{
    void *ptr;

    ptr = callocp(count, size);
    dprintf(fd, "x%lx = calloc(%zu, %zu);\n", (unsigned long)ptr, count, size);
    return ptr;
}

void *malloc(size_t size)
{
    void *ptr;

    ptr = mallocp(size);
    dprintf(fd, "x%lx = malloc(%zu);\n", (unsigned long)ptr, size);
    return ptr;
}

void free(void *ptr)
{

    dprintf(fd, "free(x%lx);\n", (unsigned long)ptr);
    freep(ptr);
}
