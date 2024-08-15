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
  case MEMORY:
    log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
    handle_memory_connection(socket);
    break;
  case KERNEL:
    log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
    handle_kernel_connection(socket);
    break;
  case -1:
    log_error(logger, "Client was disconnected from %s", socket_name);
    return;
  default:
    log_error(logger, "Incorrect operation code (%d) from %s", header, socket_name);
    return;
  }
}

void handle_memory_connection(int socket)
{
  uint8_t header;
  while (1)
  {
    header = receive_header(socket);
    log_info(logger, "OPERATION RECEIVE (%s) FROM MEMORY", translate_header(header));

    switch (header)
    {
    case CREATE_PROCESS:
      handle_create_process(socket);
      break;
    // case FINISH_PROCESS:
    // handle_finish_process(socket);
    // break;
    // case WRITE_SWAP:
    //  handle_write_swap();
    // break;
    // case READ_SWAP:
    // log_info(logger, "Receiving swap memory");
    // handle_read_swap(socket);
    // break;
    case -1:
      log_error(logger, "Client was disconnected from MEMORY");
      break;
    default:
      log_error(logger, "Incorrect operation code (%s) from MEMORY", translate_header(header));
      break;
    }
  }
}

void handle_kernel_connection(int socket)
{
  uint8_t header;
  while (1)
  {
    header = receive_header(socket);
    log_info(logger, "OPERATION RECEIVE (%s) FROM MEMORY", translate_header(header));

    switch (header)
    {
    case -1:
      log_error(logger, "Client was disconnected from MEMORY");
      break;
    default:
      log_error(logger, "Incorrect operation code (%s) from MEMORY", translate_header(header));
      break;
    }
  }
}

void handle_create_process(int socket)
{
  t_buffer *payload = receive_payload(socket);
  int pid = take_INT32_from_buffer(payload);
  int reserve_quantity = take_INT32_from_buffer(payload);

  log_trace(logger, "Reserving %d SWAP blocks for process with ID:%d", reserve_quantity, pid);

  t_list *swap_blocks = reserve_swap(reserve_quantity);

  log_trace(logger, "Sending SWAP blocks for process:%d to memory", pid);

  // send_swap(socket, RESERVE_SWAP, pid, swap_blocks);

  memory_connection = create_connection(logger, "MEMORY", filesystem_config->memory_ip, filesystem_config->memory_port, TYPE_SOCKET_CLIENT);
  send_header(memory_connection, FILESYSTEM);

  t_package *package = create_package(RESERVE_SWAP, sizeof(t_package));

  add_INT32_to_buffer(package->payload, 12);

  send_package(memory_connection, package);
  send_package(4, package);
  destroy_package(package);

  // omit_header(socket);
  free(payload);
}
/*
void handle_finish_process(int socket)
{
  t_pcb *process_to_create = get_pcb_from_socket(socket);
  log_trace(logger, "Creating memory structures for process with ID:%d", process_to_create->pid);

  int page_quantity = process_to_create->size / memory_config->pagination_size;
  t_page_table *page_table = create_page_table(process_to_create->pid, page_quantity);

  send_int(filesystem_connection, CREATE_PROCESS, page_quantity);
  free(process_to_create);
}
*/
void handle_write_swap()
{
  t_buffer *payload = receive_payload(socket);
  t_pcb *pcb = take_pcb_from_buffer(payload);
  uint32_t value = take_UINT32_from_buffer(payload);
  int swap_position = take_INT32_from_buffer(payload);
  destroy_buffer(payload);

  log_trace(logger, "Writing %d in SWAP", value);

  write_swap(swap_position, value);

  free(pcb);
}

void handle_read_swap(int socket)
{
  // TODO: RECIBIR PID Y UNA LISTA DE BLOQUES DE SWAP
  uint32_t value = 0;
  t_buffer *payload = receive_payload(socket);
  t_pcb *pcb = take_pcb_from_buffer(payload);
  int swap_pos = take_INT32_from_buffer(payload);

  read_swap(swap_pos, value);

  free(payload);
  free(pcb);
}

// MANEJO DE SWAP
t_list *reserve_swap(int reserve_quantity)
{
  t_list *swap_blocks = list_take_and_remove(free_swap_blocks, reserve_quantity);

  handle_swap(swap_blocks);

  return swap_blocks;
}
/*
t_list* free_swap(char* blocks_path, t_list* free_blocks, int block_size, int swap_blocks){
    list_add_all(free_blocks, swap_blocks);

    handle_swap(blocks_path, swap_blocks, block_size);

    return swap_blocks;
}*/

void handle_swap(t_list *swap_blocks)
{
  FILE *blocks_file = fopen(filesystem_config->block_path, "wb");

  int swap = 0;

  int index = list_size(swap_blocks);

  for (int i = 0; i < index; i++)
  {
    swap = list_remove(swap_blocks, 0);
    fseek(blocks_file, swap * filesystem_config->block_size, SEEK_SET);

    fwrite("\0", filesystem_config->block_size, 1, blocks_file);
  }

  fclose(blocks_file);
}