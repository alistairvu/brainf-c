#include "brainf.h"

// ANCHOR: Create a new command_list
command_list *command_list_new(void) {
  command *commands = (command *)malloc(sizeof(command) * INIT_CAPACITY);

  if (commands == NULL) {
    puts("Memory error: Cannot allocate memory for command list");
    exit(1);
  }

  command_list *c_list = (command_list *)malloc(sizeof(command_list));

  if (c_list == NULL) {
    puts("Memory error: Cannot allocate memory for command list");
    exit(1);
  }

  c_list->commands = commands;
  c_list->capacity = INIT_CAPACITY;
  c_list->size = 0;

  return c_list;
};

// ANCHOR: Free command list
void command_list_free(command_list *list) {
  free(list->commands);
  free(list);
};

// ANCHOR: Create a new stack
stack *stack_new(void) {
  int *elements = (int *)malloc(sizeof(int) * STACK_LEN);

  if (elements == NULL) {
    puts("Memory error: Cannot allocate memory for command list");
    exit(1);
  }

  stack *s = (stack *)malloc(sizeof(stack));

  if (s == NULL) {
    puts("Memory error: Cannot allocate memory for command list");
    exit(1);
  }

  s->elements = elements;
  s->size = 0;

  return s;
};

// ANCHOR: Free stack
void stack_free(stack *s) {
  free(s->elements);
  free(s);
};

// ANCHOR: Push element on top of the stack
bool stack_push(stack *s, int elem) {
  if (s->size >= STACK_LEN) {
    return false;
  }

  s->elements[s->size++] = elem;
  return true;
};

// ANCHOR: Create a new program
program *program_new(void) {
  char *memory = calloc(MEM_LEN, sizeof(char));

  if (memory == NULL) {
    printf("Memory error: Cannot allocate memory for program\n");
    exit(1);
  }

  program *p = malloc(sizeof(program));

  if (p == NULL) {
    printf("Memory error: Cannot allocate memory for program\n");
    exit(1);
  }

  p->program_counter = 0;
  p->mem_ptr = 0;
  p->program_stack = stack_new();
  p->program_commands = command_list_new();
  p->memory = memory;
  p->filename = "";

  return p;
};

// ANCHOR: Free program memory
void program_free(program *p) {
  stack_free(p->program_stack);
  command_list_free(p->program_commands);
  free(p->memory);
  free(p->filename);
  free(p);
};

// ANCHOR: Read program
void program_read(program *p, char *filename) {
  char buffer[BUF_LEN];
  FILE *input_file = fopen(filename, "r");

  if (input_file == NULL) {
    program_free(p);
    puts("File error: Cannot open file");
    exit(EXIT_FAILURE);
  }

  while (fgets(buffer, BUF_LEN, input_file)) {
    int limit = strlen(buffer);

    for (int i = 0; i < limit; i++) {
      char c = buffer[i];
      command_list *cl = p->program_commands;

      switch (c) {
        case '>': {
          cl->commands[cl->size++] = INC_PTR;
          break;
        }

        case '<': {
          cl->commands[cl->size++] = DEC_PTR;
          break;
        }

        case '+': {
          cl->commands[cl->size++] = INC_BYTE;
          break;
        }

        case '-': {
          cl->commands[cl->size++] = DEC_BYTE;
          break;
        }

        case '.': {
          cl->commands[cl->size++] = OUT_BYTE;
          break;
        }

        case ',': {
          cl->commands[cl->size++] = IN_BYTE;
          break;
        }

        case '[': {
          cl->commands[cl->size++] = START_LOOP;
          break;
        }

        case ']': {
          cl->commands[cl->size++] = END_LOOP;
          break;
        }
      }

      if (cl->size >= cl->capacity) {
        int new_capacity = cl->size * 2;
        cl->commands = realloc(cl->commands, new_capacity * sizeof(command));

        if (cl->commands == NULL) {
          program_free(p);
          puts("Memory allocation: Failed to allocate commands");
          exit(1);
        }

        cl->capacity = new_capacity;
      }
    }
  }

  p->filename = strdup(filename);
  fclose(input_file);
};

// ANCHOR: Check program
void program_check(program *p) {
  int bracket_balance = 0;

  for (int i = 0; i < p->program_commands->size; i++) {
    command current = p->program_commands->commands[i];

    if (current == START_LOOP) {
      bracket_balance++;
    }

    if (current == END_LOOP) {
      if (bracket_balance <= 0) {
        printf("File error: %s is invalid\n", p->filename);
        program_free(p);
        exit(1);
      }

      bracket_balance--;
    }
  }

  if (bracket_balance > 0) {
    printf("File error: %s is invalid\n", p->filename);
    program_free(p);
    exit(1);
  }
};

// ANCHOR: Run program
void program_run(program *p) {
  int program_length = p->program_commands->size;

  while (p->program_counter < program_length) {
    command current = p->program_commands->commands[p->program_counter];

    switch (current) {
      case INC_PTR: {
        p->mem_ptr++;
        break;
      }

      case DEC_PTR: {
        p->mem_ptr--;
        break;
      }

      case INC_BYTE: {
        p->memory[p->mem_ptr]++;
        break;
      }

      case DEC_BYTE: {
        p->memory[p->mem_ptr]--;
        break;
      }

      case OUT_BYTE: {
        putchar(p->memory[p->mem_ptr]);
        break;
      }

      case IN_BYTE: {
        p->memory[p->mem_ptr] = getchar();
        break;
      }

      case START_LOOP: {
        // Skip the loop if current memory value is 0
        if (p->memory[p->mem_ptr] == 0) {
          int next_command = p->program_counter + 1;
          bool match_found = false;
          int match_count = 0;

          while (!match_found) {
            command checking = p->program_commands->commands[next_command];

            if (checking == START_LOOP) {
              match_count++;
            }

            if (checking == END_LOOP) {
              if (match_count == 0) {
                match_found = true;
                break;
              }

              match_count--;
            }

            next_command++;
          }

          p->program_counter = next_command;
          break;
        }

        if (!stack_push(p->program_stack, p->program_counter)) {
          program_free(p);
          puts("Stack error: Overflow");
          exit(1);
        }

        break;
      }

      case END_LOOP: {
        if (p->program_stack->size == 0) {
          program_free(p);
          puts("Stack error: Underflow");
          exit(1);
        }

        if (p->memory[p->mem_ptr]) {
          stack *s = p->program_stack;
          p->program_counter = s->elements[s->size - 1];
        } else {
          stack *s = p->program_stack;
          s->size--;
        }

        break;
      }
    }

    p->program_counter++;
  }
};

int main(int argc, char *argv[]) {
  char *filename = NULL;
  char *file_dot = NULL;
  program *p = NULL;

  if (argc != 2) {
    printf("Usage: ./brainf <filename>\n");
    exit(64);
  }

  filename = argv[1];

  if (access(filename, F_OK) != 0) {
    printf("File error: %s not found\n", filename);
    exit(64);
  }

  file_dot = strrchr(filename, '.');

  if (!file_dot || file_dot == filename) {
    printf("File error: %s is not a BF file\n", filename);
    exit(64);
  }

  if ((strcmp(file_dot, ".bf") != 0) && (strcmp(file_dot, ".b") != 0)) {
    printf("File error: %s is not a BF file\n", filename);
    exit(64);
  }

  p = program_new();

  program_read(p, filename);
  program_check(p);
  program_run(p);
  program_free(p);

  return 0;
}