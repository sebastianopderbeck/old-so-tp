#include "handle_connection.h"

void handler_connection(void *args)
{
  t_handler_connection_args *deconstructed_args = (t_handler_connection_args *)args;
  t_log *logger = deconstructed_args->logger;
  int socket = deconstructed_args->client_socket;
  char *socket_name = deconstructed_args->socket_name;
  free(deconstructed_args);

  uint8_t header = receive_header(socket);

  switch (header)
  {
  case KERNEL:
    log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
    handle_kernel_connection(socket);
    break;
  case CPU:
    log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
    handle_cpu_connection(socket);
    break;
  case FILESYSTEM:
    log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
    handle_filesystem_connection(socket);
    break;
  case -1:
    log_error(logger, "Client was disconnected from %s", socket_name);
    return;
  default:
    log_error(logger, "Incorrect operation code (%d) from %s", header, socket_name);
    return;
  }
}

void handle_kernel_connection(int socket)
{
  uint8_t header;
  while (1)
  {
    header = receive_header(socket);
    log_info(logger, "OPERATION RECEIVE (%s) FROM KERNEL", translate_header(header));

    switch (header)
    {
    case CREATE_PROCESS:
      handle_create_process(socket);
      break;
    case FINISH_PROCESS:
      handle_finish_process();
      break;
    case -1:
      log_error(logger, "Client was disconnected from KERNEL");
      break;
    default:
      log_error(logger, "Incorrect operation code (%s) from KERNEL", translate_header(header));
      break;
    }
  }
}

void handle_cpu_connection(int socket)
{
  uint8_t header;
  while (1)
  {
    header = receive_header(socket);
    log_info(logger, "OPERATION RECEIVE (%s) FROM CPU", translate_header(header));

    switch (header)
    {
    case INSTRUCTIONS:
      handle_instruction(socket);
      break;
    case RESERVE_SWAP:
      log_info(logger, "Receiving swap memory");
      reserve_swap_memory(socket);
      break;
    case GET_FRAME:
      log_info(logger, "Searching frames");
      get_frame(socket);
      break;
    case PAGE_FAULT:
      log_info(logger, "Looking for some victim frame to replace");
      handle_page_fault(socket);
      break;
    case READ_MEMORY:
      log_info(logger, "READ FROM MEMORY");
      handle_read_memory(socket);
      break;
    case WRITE_MEMORY:
      log_info(logger, "WRITE TO MEMORY");
      handle_write_memory(socket);
      break;
    case -1:
      log_error(logger, "Client was disconnected from CPU");
      break;
    default:
      log_error(logger, "Incorrect operation code (%s) from CPU", translate_header(header));
      break;
    }
  }
}

void handle_filesystem_connection(int socket)
{
  uint8_t header, interruption_motive;
  t_buffer *buffer;

  while (1)
  {
    header = receive_header(socket);
    log_info(logger, "OPERATION RECEIVE (%s) FROM FILESYSTEM", translate_header(header));
    // sem_wait(mutex_cpu_dispatch_connection);

    switch (header)
    {
    case RESERVE_SWAP:
      log_info(logger, "Receiving swap memory");
      reserve_swap_memory(socket);
      break;
    case -1:
      log_error(logger, "An error has Ocurred with FILESYSTEM connection");
      return;
    default:
      log_error(logger, "Incorrect operation code (%s)", translate_header(header));
      return;
    }
  }
}

void handle_instruction(int socket)
{
  t_pcb *process_to_execute = get_pcb_from_socket(socket);
  /* TEST: remove this*/
  char *path = string_new();
  string_append(&path, memory_config->instruction_path);
  string_append(&path, "/");
  string_append(&path, process_to_execute->path);
  log_trace(logger, "Searching instruction to execute for process with ID:%d", process_to_execute->pid);
  char *instruction = get_instruction_from_file(path, process_to_execute->program_counter);
  log_info(logger, "Sending instruction to CPU");
  send_instruction(socket, instruction);

  // omit_header(socket);
  free(process_to_execute);
  free(path);
}

void handle_create_process(int socket)
{
  t_pcb *process_to_create = get_pcb_from_socket(socket);
  log_trace(logger, "Creating memory structures for process with ID:%d", process_to_create->pid);

  int page_quantity = process_to_create->size / memory_config->pagination_size;
  // t_page_table *page_table = create_page_table(process_to_create->pid, page_quantity);

  t_package *package = create_package(CREATE_PROCESS, sizeof(t_package));

  add_INT32_to_buffer(package->payload, process_to_create->pid);
  add_INT32_to_buffer(package->payload, page_quantity);
  send_package(filesystem_connection, package);
  destroy_package(package);

  free(process_to_create);
}

// TODO liberar su espacio de memoria
void handle_finish_process()
{
  t_pcb *pcb = get_pcb_from_socket(socket);
  log_trace(logger, "Finishing process with ID:%d in memory destroying resources", pcb->pid);

  t_page_table *table_page = get_table_pages_by_pid(pcb->pid);
  t_list *swap_positions = delete_page_table(table_page);

  send_swap(filesystem_connection, FINISH_PROCESS, pcb->pid, swap_positions);

  free(table_page);
  free(pcb);
}

void reserve_swap_memory(int socket)
{
  t_buffer *payload = receive_payload(socket);
  int pid = take_INT32_from_buffer(payload);
  t_list *swap_blocks = take_list_from_buffer(payload, (void *)take_UINT32_from_buffer);

  t_page_table *page_table = create_page_table(pid, swap_blocks);
  list_add(page_table_list, page_table);
}

void get_frame(int socket)
{
  t_pcb *pcb = get_pcb_from_socket(socket);
  t_page_table *page_table = get_table_pages_by_pid(pcb->pid);

  t_page *table_pages = list_get(page_table->pages, pcb->table_pages);

  if (table_pages->presence == 1)
  {
    printf("FOUND_FRAME");
    if (strcmp(memory_config->replacement_algorithm, "LRU") == 0)
    {
      replace_page_lru_with_presence(pcb->pid, pcb->table_pages);
    }

    send_int(socket, GET_FRAME, table_pages->frame);
  }
  else
  {
    printf("PAGE_FAULT");
    send_header(socket, PAGE_FAULT);
  }
  free(page_table);
}

void handle_page_fault(int socket)
{
  t_pcb *pcb = get_pcb_from_socket(socket);
  t_page_table *page_table = get_table_pages_by_pid(pcb->pid);

  t_page *page = list_get(page_table->pages, pcb->table_pages);

  replace_page_fifo(page, pcb->pid, pcb->table_pages);

  free(page_table);
  free(pcb);
}

void handle_read_memory(int socket)
{
  t_buffer *payload = receive_payload(socket);
  t_pcb *pcb = take_pcb_from_buffer(payload);
  int physical_direction = take_INT32_from_buffer(payload);

  read_memory(physical_direction);

  destroy_buffer(payload);
  destroy_pcb(pcb);
}

void handle_write_memory(int socket)
{
  t_buffer *payload = receive_payload(socket);
  t_pcb *pcb = take_pcb_from_buffer(payload);
  int physical_direction = take_INT32_from_buffer(payload);
  uint32_t value = 0;
  write_memory(physical_direction, value);

  destroy_buffer(payload);
  destroy_pcb(pcb);
}

uint32_t read_memory(int physical_direction)
{
  uint32_t value;

  memcpy(&value, main_memory + physical_direction, sizeof(uint32_t));

  return value;
}

void write_memory(int physical_direction, uint32_t value)
{

  memcpy(&main_memory + physical_direction, &value, sizeof(uint32_t));

  return;
}