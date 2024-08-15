#ifndef SHARED_INSTRUCTIONS_H_
#define SHARED_INSTRUCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include "serialization.h"
#include "sockets.h"
#include "operation_handler.h"
typedef enum
{
  SET,
  SUM,
  SUB,
  JNZ,
  SLEEP,
  WAIT,
  SIGNAL,
  MOV_IN,
  MOV_OUT,
  F_OPEN,
  F_CLOSE,
  F_SEEK,
  EXIT,
  F_READ,
  F_WRITE,
  F_TRUNCATE,
  NO_OP,
} t_instruction_type;
/*TODO: In this file (instruction.txt) there are many more unmapped instructions  */

typedef struct
{
  t_instruction_type instruction_type;
  t_list *params;
} t_instruction;

typedef t_list t_instruction_list;
typedef struct
{
  char *path;
} t_process;

t_instruction *create_instruction(t_instruction_type instruction_type);

void destroy_instruction(t_instruction *instruction);

void add_param_to_instruction(t_instruction *instruction, int param);

t_instruction_list *create_instruction_list(void);

void destroy_instruction_list(t_instruction_list *instruction_list);

void add_instruction_to_list(t_list *instruction_list, t_instruction *instruction);

void add_instruction_to_buffer(t_buffer *buffer, t_instruction *instruction);

void send_instruction_list(int socket, t_instruction_list *instruction_list);

t_instruction *take_instruction_from_buffer(t_buffer *buffer);

char *get_instruction_from_socket(int socket);

uint8_t translate_instruction(char *instruction_type);

void send_instruction(int socket, char *instruction);

#endif