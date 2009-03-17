//
// build it like this:
// gcc -ggdb -Wall -ldl -fPIC -o tester alloc_tester.c
//


#define  _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>

#define MAXLINE   255

void **blocks = NULL;

int main(int argc, char **argv) {
  const char * trace_file_path = argv[1];
  const char * malloc_lib = argv[2];
  int num_blocks = 0;
  FILE * tracefile = NULL;
  char curr_op;
  size_t curr_size = 0, curr_nelms = 0;
  int idx = 0;
  int block_to_realloc;
  int use_default = 0;
  void    *handle;
  unsigned long n_mallocs = 0, n_callocs = 0, n_reallocs = 0, n_frees = 0,
                n_dfrees = 0, n_dfrees_null = 0;

  printf("Using trace file: %s\n", trace_file_path);
  if (strcmp(argv[2], "default") == 0) {
    use_default = 1;
    printf("Using  DEFAULT  malloc\n");
  } else {
    printf("Using malloc lib: %s\n", malloc_lib);
  }

  if ((tracefile = fopen(trace_file_path, "r")) == NULL) {
    fprintf(stderr, "Unable to open: %s, error: %s\n", trace_file_path, strerror(errno));
    exit(1); 
  }
 
  fscanf(tracefile, "%d", &(num_blocks));
  printf("%d unique memory blocks\n", num_blocks);
 
  fscanf(tracefile, "\n");

  if ((blocks = malloc(sizeof(void *) * num_blocks)) == NULL) {
    printf("Failed trying to allocate ops structures: %s\n", strerror(errno));
    exit(1);
  }

  blocks[0] = NULL;

  if (!use_default) {
    handle = dlopen(malloc_lib, RTLD_LOCAL | RTLD_LAZY);
  }

  while (fscanf(tracefile, "%c", &curr_op) != EOF) {
    fscanf(tracefile, "%d", &idx);  
    switch(curr_op) {
      case 'm':
        fscanf(tracefile, "%zu\n", &curr_size);
        //printf("malloc(%zu) = %d\n", curr_size, idx);
        blocks[idx] = malloc(curr_size);
        n_mallocs ++;
        break;
      case 'r':
        fscanf(tracefile, "%d %lu\n", &block_to_realloc, &curr_size);
        //printf("realloc(%d, %zu) = %d\n", block_to_realloc, curr_size, idx);
        blocks[idx] = realloc(blocks[block_to_realloc], curr_size);
        n_reallocs ++;
        break;
      case 'c':
        fscanf(tracefile, "%lu %lu\n", &curr_nelms, &curr_size);
        //printf("calloc(%zu, %zu) = %d\n", curr_nelms, curr_size, idx);
        blocks[idx] = calloc(curr_nelms, curr_size);
        n_callocs ++;
        break;
      case 'f':
        fscanf(tracefile, "\n");
        if (blocks[idx] == NULL) {
          n_dfrees ++;
        } else if (idx == 0) {
          n_dfrees_null ++;
        } else {
          //printf("free(%d)\n", idx);
          free(blocks[idx]);
          blocks[idx] = NULL;
          n_frees ++;
        }
        break;
      default:
        printf("unknown char: %c -- skipping\n", curr_op);
        break;
    }
  }

  printf("Finished processing!\n");
  printf("m: %lu, c: %lu, r: %lu, f: %lu, double frees: %lu, "
         "double frees of NULL: %lu\n",
         n_mallocs, n_callocs, n_reallocs, n_frees, n_dfrees, n_dfrees_null);

  if (!use_default) {
    dlclose(handle);
  }

  return 0;
}
