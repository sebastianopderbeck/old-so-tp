#include "process.h"

const char *translate_process_state(t_process_state state)
{
    switch (state)
    {
    case NEW:
        return "NEW";
        break;
    case READY:
        return "READY";
        break;
    case EXECUTION:
        return "EXECUTION";
        break;
    case BLOCKED:
        return "BLOCKED";
        break;
    }
}

t_process *create_process(char *path)
{
    t_process *process_to_create = malloc(sizeof(t_process));

    // process_to_create->process_size = size;
    // process_to_create->priority = priority;
    process_to_create->path = path;

    return process_to_create;
}

void destroy_process(t_process *process)
{
    // DEPRECATED
    // destroy_instruction_list(process->instruction_list);
    free(process);
}

void send_process(int socket, t_process *process)
{
    t_package *package = create_package(CREATE_PROCESS, sizeof(t_process));
    add_process_to_buffer(package->payload, process);
    send_package(socket, package);
    destroy_package(package);
}

void add_process_to_buffer(t_buffer *buffer, t_process *process)
{
    // add_UINT32_to_buffer(buffer, process->process_size);
    // add_INT32_to_buffer(buffer, process->priority);
    add_string_to_buffer(buffer, process->path);
    // add_list_to_buffer(buffer, process->instruction_list, (void *)add_instruction_to_buffer);
}

t_process *take_process_from_buffer(t_buffer *buffer)
{
    //uint32_t process_size = take_UINT32_from_buffer(buffer);
    //int32_t priority = take_INT32_from_buffer(buffer);
    char *path = take_string_from_buffer(buffer);

    // t_instruction_list *instruction_list;
    // instruction_list = take_list_from_buffer(buffer, (void *)*take_instruction_from_buffer);

    t_process *process_to_create = create_process(path);

    return process_to_create;
}

void send_pcb(int socket, uint8_t header, t_pcb *pcb)
{
    t_package *package = create_package(header, sizeof(t_pcb));
    add_pcb_to_buffer(package, pcb);
    send_package(socket, package);
    destroy_package(package);
}

void add_pcb_to_buffer(t_package *package, t_pcb *pcb)
{
    add_UINT32_to_buffer(package->payload, pcb->pid);
    add_INT32_to_buffer(package->payload, pcb->program_counter);
    //add_INT32_to_buffer(package->payload, pcb->priority);
    add_INT32_to_buffer(package->payload, pcb->quantum);
    add_UINT32_to_buffer(package->payload, pcb->size);
    add_list_to_buffer(package->payload, pcb->cpu_register, (void *)add_UINT32_to_buffer);
    add_list_to_buffer(package->payload, pcb->resources, (void *)add_string_to_buffer);
    add_INT32_to_buffer(package->payload, pcb->table_pages);
    add_string_to_buffer(package->payload, pcb->path);
    add_INT32_to_buffer(package->payload, pcb->state);
    add_INT32_to_buffer(package->payload, pcb->client_socket);
}

void send_sleep(int socket, uint8_t header, t_pcb *pcb, int sleep_time)
{
    t_package *package = create_package(header, sizeof(t_sleep));

    add_pcb_to_buffer(package, pcb);
    add_INT32_to_buffer(package->payload, sleep_time);

    send_package(socket, package);
    destroy_package(package);
}

void send_process_id(int socket, uint8_t header, int id)
{
    t_package *package = create_package(header, sizeof(id));

    add_UINT32_to_buffer(package->payload, id);

    send_package(socket, package);
    destroy_package(package);
}

void send_page_to_load(int socket, uint8_t header, int page)
{
    t_package *package = create_package(header, sizeof(page));

    add_INT32_to_buffer(package->payload, page);

    send_package(socket, package);
    destroy_package(package);
}

void send_multiprogramming_degree(int socket, uint8_t header, int new_degree)
{
    t_package *package = create_package(header, sizeof(new_degree));

    add_INT32_to_buffer(package->payload, new_degree);

    send_package(socket, package);
    destroy_package(package);
}

void send_int(int socket, uint8_t header, int integer)
{
    t_package *package = create_package(header, sizeof(int));

    add_INT32_to_buffer(package->payload, integer);

    send_package(socket, package);
    destroy_package(package);
}

t_pcb *take_pcb_from_buffer(t_buffer *buffer)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    pcb->pid = take_UINT32_from_buffer(buffer);
    pcb->program_counter = take_INT32_from_buffer(buffer);
    //pcb->priority = take_INT32_from_buffer(buffer);
    pcb->quantum = take_INT32_from_buffer(buffer);
    pcb->size = take_UINT32_from_buffer(buffer);
    pcb->cpu_register = take_list_from_buffer(buffer, (void *)*take_UINT32_from_buffer);
    pcb->resources = take_list_from_buffer(buffer, (void *)*take_string_from_buffer);
    pcb->table_pages = take_INT32_from_buffer(buffer);
    pcb->path = take_string_from_buffer(buffer);
    pcb->state = take_INT32_from_buffer(buffer);
    pcb->client_socket = take_INT32_from_buffer(buffer);

    return pcb;
}

uint32_t get_process_id_from_socket(int socket)
{
    t_buffer *payload = receive_payload(socket);
    uint32_t id = take_UINT32_from_buffer(payload);
    destroy_buffer(payload);

    return id;
}

t_pcb *get_pcb_from_socket(int socket)
{
    t_buffer *payload = receive_payload(socket);
    t_pcb *pcb = take_pcb_from_buffer(payload);
    destroy_buffer(payload);

    return pcb;
}

int get_page_from_socket(int socket)
{
    t_buffer *payload = receive_payload(socket);
    int page = take_INT32_from_buffer(payload);
    destroy_buffer(payload);

    return page;
}

int get_int_from_socket(int socket)
{
    t_buffer *payload = receive_payload(socket);
    int page = take_INT32_from_buffer(payload);
    destroy_buffer(payload);

    return page;
}

int32_t get_new_degree_from_socket(int socket)
{
    t_buffer *payload = receive_payload(socket);
    int32_t new_degree = take_INT32_from_buffer(payload);
    destroy_buffer(payload);

    return new_degree;
}

void destroy_pcb(t_pcb *pcb)
{
    // destroy_instruction_list(pcb->instruction_list);
    free(pcb);
}

bool sort_by_priority(t_pcb *first_process, t_pcb *second_process)
{
    return first_process->priority <= second_process->priority;
}

void send_pcb_and_resource(int socket, uint8_t header, t_pcb *pcb, char *resource)
{
    t_package *package = create_package(header, sizeof(t_package));
    add_pcb_to_buffer(package, pcb);
    add_string_to_buffer(package->payload, resource);
    send_package(socket, package);
    destroy_package(package);
}

void send_swap(int socket, uint8_t header, int pid, t_list *list)
{
    t_package *package = create_package(header, sizeof(t_package));

    add_INT32_to_buffer(package->payload, pid);
    add_list_to_buffer(package->payload, list, (void *)add_string_to_buffer);

    send_package(socket, package);
    destroy_package(package);
}