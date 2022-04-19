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
#define MEM_LEN 30000
#define INIT_CAPACITY 1024

typedef enum command {
  INC_PTR,
  DEC_PTR,
  INC_BYTE,
  DEC_BYTE,
  OUT_BYTE,
  IN_BYTE,
  START_LOOP,
  END_LOOP
} command;

typedef struct {
  size_t capacity;
  size_t size;
  command *commands;
} command_list;

typedef struct stack_node {
  int val;
  struct stack_node *prev;
  struct stack_node *next;
} stack_node;

typedef struct {
  size_t size;
  stack_node *head;
  stack_node *tail;
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
stack_node *stack_node_new(int val);
void stack_free(stack *s);
bool stack_push(stack *s, int elem);
int stack_peek(stack *s);
int stack_pop(stack *s);

program *program_new(void);
void program_free(program *p);

void program_read(program *p, char *filename);
void program_check(program *p);
void program_run(program *p);