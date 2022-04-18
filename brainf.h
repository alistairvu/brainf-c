#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUF_LEN 2048
#define STACK_LEN 128
#define MEM_LEN 30000
#define INIT_CAPACITY 1024

typedef uint8_t command;

static const command INC_PTR = 0;
static const command DEC_PTR = 1;
static const command INC_BYTE = 2;
static const command DEC_BYTE = 3;
static const command OUT_BYTE = 4;
static const command IN_BYTE = 5;
static const command START_LOOP = 6;
static const command END_LOOP = 7;

typedef struct {
  size_t capacity;
  size_t size;
  command *commands;
} command_list;

typedef struct {
  size_t size;
  int *elements;
} stack;

typedef struct {
  int program_counter;
  int mem_ptr;
  stack *program_stack;
  command_list *program_commands;
  char *memory;
  char *filename;
} program;

command_list *command_list_new(void);
void command_list_free(command_list *list);

stack *stack_new(void);
void stack_free(stack *s);
bool stack_push(stack *s, int elem);

program *program_new(void);
void program_free(program *p);

void program_read(program *p, char *filename);
void program_check(program *p);
void program_run(program *p);