#include "instructions.h"

t_instruction *create_instruction(t_instruction_type instruction_type)
{
  t_instruction *instruction = malloc(sizeof(t_instruction));

  instruction->instruction_type = instruction_type;
  instruction->params = list_create();

  return instruction;
}

void destroy_instruction(t_instruction *instruction)
{
  list_destroy(instruction->params);
  free(instruction);
}

void add_param_to_instruction(t_instruction *instruction, int param)
{
  list_add(instruction->params, (void *)(intptr_t)param);
}

t_instruction_list *create_instruction_list(void)
{
  t_instruction_list *list_to_create = list_create();
  return list_to_create;
}

void destroy_instruction_list(t_instruction_list *instruction_list)
{
  list_destroy_and_destroy_elements(instruction_list, (void *)*destroy_instruction);
}

void add_instruction_to_list(t_list *instruction_list, t_instruction *instruction)
{
  if (instruction->instruction_type == NO_OP)
  {
    t_instruction *default_instruction;
    for (int i = 0; i < (intptr_t)list_get(instruction->params, 0); i++)
    {
      default_instruction = create_instruction(NO_OP);
      list_add(instruction_list, default_instruction);
    }
    return;
  }
  list_add(instruction_list, instruction);
}

void add_instruction_to_buffer(t_buffer *buffer, t_instruction *instruction)
{
  add_UINT8_to_buffer(buffer, instruction->instruction_type);
  add_list_to_buffer(buffer, instruction->params, (void *)add_INT32_to_buffer);
}

void send_instruction_list(int socket, t_instruction_list *instruction_list)
{
  t_package *package = create_package(INSTRUCTIONS, sizeof(t_list));
  add_list_to_buffer(package->payload, instruction_list, (void *)add_instruction_to_buffer);
  send_package(socket, package);
  destroy_package(package);
}

t_instruction *take_instruction_from_buffer(t_buffer *buffer)
{
  t_instruction *instruction_to_take = malloc(sizeof(t_instruction));

  uint8_t instruction_type;
  instruction_type = take_UINT8_from_buffer(buffer);

  t_list *params;
  params = take_list_from_buffer(buffer, (void *)take_UINT32_from_buffer);

  instruction_to_take->instruction_type = instruction_type;
  instruction_to_take->params = params;

  return instruction_to_take;
}

void send_instruction(int socket, char *instruction)
{
  // t_package* package = create_package(INSTRUCTION, sizeof(t_instruction));
  t_package *package = create_package(INSTRUCTION, string_length((char *)instruction) + 1);

  // add_UINT8_to_buffer(package -> payload, instruction -> instruction_type);
  add_string_to_buffer(package->payload, instruction);
  send_package(socket, package);
  destroy_package(package);
}

char *get_instruction_from_socket(int socket)
{
  t_buffer *payload = receive_payload(socket);
  char *instruction = take_string_from_buffer(payload);
  destroy_buffer(payload);

  return instruction;
}

uint8_t translate_instruction(char *instruction_type)
{
  if (strcmp(instruction_type, "SET") == 0)
    return SET;
  else if (strcmp(instruction_type, "SUM") == 0)
    return SUM;
  else if (strcmp(instruction_type, "SUB") == 0)
    return SUB;
  else if (strcmp(instruction_type, "JNZ") == 0)
    return JNZ;
  else if (strcmp(instruction_type, "WAIT") == 0)
    return WAIT;
  else if (strcmp(instruction_type, "EXIT") == 0)
    return EXIT;
  else if (strcmp(instruction_type, "SLEEP") == 0)
    return SLEEP;
  else if (strcmp(instruction_type, "F_READ") == 0)
    return F_READ;
  else if (strcmp(instruction_type, "F_OPEN") == 0)
    return F_OPEN;
  else if (strcmp(instruction_type, "F_SEEK") == 0)
    return F_SEEK;
  else if (strcmp(instruction_type, "SIGNAL") == 0)
    return SIGNAL;
  else if (strcmp(instruction_type, "MOV_IN") == 0)
    return MOV_IN;
  else if (strcmp(instruction_type, "F_WRITE") == 0)
    return F_WRITE;
  else if (strcmp(instruction_type, "MOV_OUT") == 0)
    return MOV_OUT;
  else if (strcmp(instruction_type, "F_CLOSE") == 0)
    return F_CLOSE;
  else if (strcmp(instruction_type, "F_TRUNCATE") == 0)
    return F_TRUNCATE;
  else
    return NO_OP;
}