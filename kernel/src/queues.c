#include "queues.h"

void init_semaphores_queues()
{
    mutex_queue_new = malloc(sizeof(sem_t));
    sem_init(mutex_queue_new, 0, 1);

    mutex_queue_ready = malloc(sizeof(sem_t));
    sem_init(mutex_queue_ready, 0, 1);

    mutex_queue_blocked = malloc(sizeof(sem_t)); // to check, is mutex?
    sem_init(mutex_queue_blocked, 0, 1);

    mutex_queue_exec = malloc(sizeof(sem_t));
    sem_init(mutex_queue_exec, 0, 1);
}

void init_queues()
{
    queue_new = queue_create();
    queue_ready = queue_create();
    queue_exec = queue_create();
}

void destroy_queues()
{
    queue_clean(queue_new);
    queue_clean(queue_ready);
    queue_clean(queue_exec);

    queue_destroy(queue_new);
    queue_destroy(queue_ready);
    queue_destroy(queue_exec);
}

void destroy_semaphores_queues()
{
    free(mutex_queue_new);
    free(mutex_queue_ready);
    // free(mutex_queue_waiting); not necessary
    free(mutex_queue_blocked);
    free(mutex_queue_exec);
}

t_list *build_all_queues_in_list()
{
    t_list *all_queues = list_create();

    sem_wait(mutex_queue_new);
    list_add(all_queues, queue_new);
    sem_post(mutex_queue_new);

    sem_wait(mutex_queue_ready);
    list_add(all_queues, queue_ready);
    sem_post(mutex_queue_ready);

    sem_wait(mutex_queue_exec);
    list_add(all_queues, queue_exec);
    sem_post(mutex_queue_exec);

    sem_wait(mutex_queue_blocked);
    /*
    for (int queue_index = 0; queue_index < list_size(list_of_queue_blocked); queue_index++)
    {
        t_queue *queue_to_add = list_get(list_of_queue_blocked, queue_index);
        list_add(all_queues, queue_to_add);
    }*/
    list_add_all(all_queues, list_of_queue_blocked);
    sem_post(mutex_queue_blocked);
    return all_queues;
}

t_list *build_all_mutex_queues_in_list()
{
    t_list *all_mutex_queues = list_create();
    list_add(all_mutex_queues, mutex_queue_new);
    list_add(all_mutex_queues, mutex_queue_ready);
    list_add(all_mutex_queues, mutex_queue_exec);
    for (int queue_index = 0; queue_index < list_size(list_of_queue_blocked); queue_index++)
    {
        list_add(all_mutex_queues, mutex_queue_blocked);
    }
    return all_mutex_queues;
}

t_pcb *find_process_from_queue(t_queue *queue_from, int pid)
{
    bool find_by_id(t_pcb * element)
    {
        return element->pid == pid;
    }

    return list_find(queue_from->elements, (void *)*(find_by_id));
}

void finish_process(uint32_t pid)
{
    log_info(logger, "Finishing process with ID: %d", pid);
    t_list *all_queues_in_list = build_all_queues_in_list();
    t_list *all_mutex_queues = build_all_mutex_queues_in_list();
    t_pcb *process_to_finish;
    int queue_index = 0;
    for (queue_index = 0; queue_index < list_size(all_queues_in_list); queue_index += 1)
    {
        sem_wait(list_get(all_mutex_queues, queue_index));
        process_to_finish = find_process_from_queue(list_get(all_queues_in_list, queue_index), pid);
        sem_post(list_get(all_mutex_queues, queue_index));
        if (process_to_finish)
            break;
    }
    /*
    if (queue_size(queue_new) == 0 && queue_size(queue_ready) == 0 && queue_size(queue_blocked) == 0 && queue_size(queue_exec) == 0)
    {
        log_info(logger, "There are no processes in the queue");
    }
    */
    if (process_to_finish == NULL)
    {
        log_info(logger, "Process with ID: %d not found", pid);
        return;
    }
    uint8_t previous_state = process_to_finish->state;
    if (process_to_finish->state == EXECUTION)
    {
        // TODO: CHECK THIS CODE
        // send_pcb(cpu_interrupt_connection, INTERRUPT, process_to_finish);
        // uint8_t cpu_response = receive_header(cpu_interrupt_connection);
        // t_buffer *payload = receive_payload(cpu_response);
        // process_to_finish = take_pcb_from_buffer(payload);
    }
    process_to_finish->state = EXIT;

    free_resources_retained(process_to_finish);

    sem_wait(list_get(all_mutex_queues, queue_index));
    t_queue *queue_to_remove = list_get(all_queues_in_list, queue_index);
    list_remove_element(queue_to_remove->elements, process_to_finish);
    sem_post(list_get(all_mutex_queues, queue_index));

    check_process_in_blocked_and_send_to_ready();

    log_trace(logger, "Process with pid: %d, has been finished", process_to_finish->pid);

    send_pcb(memory_connection, FINISH_PROCESS, process_to_finish);

    update_queue_semaphores(previous_state);
    destroy_pcb(process_to_finish);
}

void check_process_in_blocked_and_send_to_ready()
{
    for (int list_index = 0; list_index < list_size(list_of_queue_blocked); list_index += 1)
    {
        t_queue *queue_to_check = list_get(list_of_queue_blocked, list_index);
        if (queue_size(queue_to_check) > 0)
        {
            int quantity_of_available_resources = atoi(list_get(all_resources_instances, list_index));
            if (quantity_of_available_resources > 0)
            {
                for (int queue_index = 0; queue_index < get_minimun(quantity_of_available_resources, queue_size(queue_to_check)); queue_index++)
                {
                    sem_wait(mutex_queue_blocked);
                    t_pcb *process_to_move = list_get(queue_to_check->elements, queue_index);
                    sem_post(mutex_queue_blocked);
                    move_process_from_block_to_ready(process_to_move, queue_to_check);
                }
            }
        }
    }
}

void update_queue_semaphores(uint8_t state)
{
    if (state == EXECUTION)
    {
        sem_post(sem_cpu_available);
        sem_post(sem_multiprogrammming_degree);
    }
    else if (state == READY)
    {
        sem_wait(sem_process_ready);
        sem_post(sem_multiprogrammming_degree);
    }
    else if (state == BLOCKED)
    {
        sem_wait(sem_process_blocked);
        sem_post(sem_multiprogrammming_degree);
    }
    else
    {
        sem_wait(sem_process_new);
    }
}

void move_process_to_new(t_pcb *process_to_move)
{
    // TODO: may be need a mutex for connetions with memory
    send_pcb(memory_connection, CREATE_PROCESS, process_to_move);
    /* TEST
    uint8_t memory_response = receive_header(memory_connection);

    if (memory_response == PROCESS_DENIED)
    {
        log_error(logger, "Process with ID: %d was denied for memory", process_to_move->pid);
        close(process_to_move->client_socket);
        return;
    }
    */
    sem_wait(mutex_queue_new);
    queue_push(queue_new, (void *)process_to_move);
    sem_post(mutex_queue_new);

    /* TEST
    t_buffer *payload = receive_payload(memory_connection);
    uint32_t table_pages = take_INT32_from_buffer(payload);
    process_to_move->table_pages = table_pages;

    destroy_buffer(payload);
    */

    log_trace(logger, "Process was created with ID: %d and has state: %s", process_to_move->pid, translate_process_state(process_to_move->state));
    sem_post(sem_process_new);
}

void move_process_to_ready()
{
    sem_wait(mutex_queue_new);
    t_pcb *process_to_move = (t_pcb *)queue_pop(queue_new);
    sem_post(mutex_queue_new);

    log_trace(logger, "Moving process with ID:%d from NEW queue to READY queue", process_to_move->pid);
    process_to_move->state = READY;

    log_info(logger, "Sending message to memory for create structures", process_to_move->pid);

    sem_wait(mutex_queue_ready);
    queue_push(queue_ready, (void *)process_to_move);
    sem_post(mutex_queue_ready);
}

void move_process_to_exec()
{
    sem_wait(mutex_queue_ready);
    t_pcb *process_to_move = (t_pcb *)queue_pop(queue_ready);
    sem_post(mutex_queue_ready);

    process_to_move->state = EXECUTION;
    log_trace(logger, "Moving process with ID:%d from READY queue to EXECUTION queue", process_to_move->pid);

    sem_wait(mutex_queue_exec);
    queue_push(queue_exec, (void *)process_to_move);
    sem_post(mutex_queue_exec);

    dispatch_process_to_cpu(process_to_move, EXECUTE_PROCESS);
    log_info(logger, "Waiting cpu response...");
    // handle_interruption(process_to_move);

    // destroy_pcb(process_to_move);
}

void sort_queue()
{
    sem_wait(mutex_queue_ready);
    list_sort(queue_ready->elements, (void *)*(sort_by_priority));
    sem_post(mutex_queue_ready);
}

void move_process_from_execution_to_ready()
{
    sem_wait(mutex_queue_exec);
    t_pcb *process_to_move = (t_pcb *)queue_pop(queue_exec);
    sem_post(mutex_queue_exec);

    sem_post(sem_cpu_available);

    log_trace(logger, "Moving process with ID:%d from EXECUTION queue to READY queue", process_to_move->pid);
    process_to_move->state = READY;

    sem_wait(mutex_queue_ready);
    queue_push(queue_ready, (void *)process_to_move);
    sem_post(mutex_queue_ready);

    sem_post(sem_process_ready);
}

void dispatch_process_to_cpu(t_pcb *process_to_move, uint8_t header)
{
    sem_wait(mutex_cpu_dispatch_connection);
    send_pcb(cpu_dispatch_connection, header, process_to_move);
    sem_post(mutex_cpu_dispatch_connection);
    log_trace(logger, "Process was sent through CPU DISPATCH for %s", translate_header(header));
}

void move_process_from_exec_to_blocked(t_queue *queue_blocked)
{
    sem_wait(mutex_queue_exec);
    t_pcb *process_to_move = (t_pcb *)queue_pop(queue_exec);
    sem_post(mutex_queue_exec);

    log_trace(logger, "Moving process with ID:%d from EXECUTION queue to BLOCKED queue", process_to_move->pid);

    sem_post(sem_cpu_available);
    process_to_move->state = BLOCKED;

    // TODO: one mutex for each queue?
    sem_wait(mutex_queue_blocked);
    queue_push(queue_blocked, (void *)process_to_move);
    sem_post(mutex_queue_blocked);
    sem_post(sem_process_blocked);

    check_deadlocks(process_to_move);
}

void move_process_from_block_to_ready(t_pcb *process_to_move, t_queue *queue_blocked)
{
    log_trace(logger, "Moving process with ID:%d from BLOCKED queue to READY queue", process_to_move->pid);
    process_to_move->state = READY;

    sem_wait(mutex_queue_ready);
    queue_push(queue_ready, (void *)process_to_move);
    sem_post(mutex_queue_ready);

    sem_wait(mutex_queue_blocked);
    list_remove_element(queue_blocked->elements, process_to_move);
    sem_post(mutex_queue_blocked);

    sem_wait(sem_process_blocked);

    sem_post(sem_process_ready);
}

t_queue *get_queue_blocked(char *queue_tag)
{
    t_queue *queue_found = queue_create();
    for (int queue_index = 0; queue_index < list_size(list_of_queue_blocked); queue_index++)
    {
        if (strcmp(list_get(all_resources, queue_index), queue_tag) == 0)
            queue_found = list_get(list_of_queue_blocked, queue_index);
    }
    return queue_found;
}