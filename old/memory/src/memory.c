#include "memory.h"

t_log *logger;
t_memory_config *memory_config;

pthread_t thread_memory, thread_filesystem;
int memory_connection;
int filesystem_connection;
int page_time;

t_list *page_table_list;
t_list *frames_used_list;
void *main_memory;

int main()
{
  page_time = 0;

  memory_config = create_memory_config("configuration/memory.config");

  logger = log_create("logs/memory.log", "MEMORY", 1, LOG_LEVEL_TRACE);
  log_info(logger, "Memory started");

  reserve_memory();

  init_page_table_list();

  filesystem_connection = create_connection(logger, "FILESYSTEM", memory_config->filesystem_ip, memory_config->filesystem_port, TYPE_SOCKET_CLIENT);

  init_connections();

  destroy_socket(filesystem_connection);
  destroy_socket(memory_connection);
  destroy_memory_config(memory_config);

  close_program(logger);
  return EXIT_SUCCESS;
}

void reserve_memory()
{
  main_memory = malloc(memory_config->memory_size);

  if (main_memory == NULL)
  {
    log_error(logger, "Error on reserve memory");
    return EXIT_FAILURE;
  }
}

void init_connections()
{
  // pthread_create(&thread_filesystem, 0, (void *)handle_filesystem_connection, NULL);
  // pthread_detach(thread_filesystem);
  pthread_create(&thread_memory, NULL, (void *)handle_memory_connection, NULL);
  pthread_join(thread_memory, NULL);
}

void *handle_memory_connection()
{
  memory_connection = create_connection(logger, "MEMORY", memory_config->listening_ip, memory_config->listening_port, TYPE_SOCKET_SERVER);
  while (server_listen(logger, "MEMORY", memory_connection, (void *)(*handler_connection)))
    ;
}

char *get_instruction_from_file(char *instruction_path, int program_counter)
{
  FILE *instruction_file = fopen(instruction_path, "r");
  char *line_instruction = string_new();

  usleep(memory_config->response_delay);

  if (instruction_file == NULL)
  {
    log_error(logger, "File (%s) could not be opened", instruction_path);
    return;
  }

  for (int i = 0; i <= program_counter; i++)
  {
    fgets(line_instruction, 1024, instruction_file);
  }

  line_instruction[strcspn(line_instruction, "\n")] = 0;

  fclose(instruction_file);

  // t_instruction* instruction = parse_instruction(line_instruction);
  log_trace(logger, "Program counter:%d, Instruction found %s", program_counter, line_instruction);

  return line_instruction;
}
