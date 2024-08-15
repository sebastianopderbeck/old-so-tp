#include "handle_connection.h"

void handler_connection(void *args)
{
    t_handler_connection_args *deconstructed_args = (t_handler_connection_args *)args;
    t_log *logger = deconstructed_args->logger;
    int socket = deconstructed_args->client_socket;
    char *socket_name = deconstructed_args->socket_name;
    free(deconstructed_args);

    uint8_t header;
    t_pcb *pcb;
    while (1)
    {
        log_info(logger, "LISTENING OPERATIONS");

        header = receive_header(socket);
        log_info(logger, "OPERATION RECEIVE (%s) FROM %s", translate_header(header), socket_name);

        switch (header)
        {
        case CREATE_PROCESS:
            pcb = receive_pcb_and_create(socket);
            move_process_to_new(pcb);
            break;
        case FINISH_PROCESS:
            uint32_t pid = get_process_id_from_socket(socket);
            finish_process(pid);
            break;
        case STOP_PLANNING:
            /*TODO: START STOPPED, CHANGE TO STOP MANUALLY?*/
            sem_post(mutex_stopping_planner);
            sem_wait(sem_short_term_planner);
            sem_wait(sem_long_term_planner);
            // sem_wait(sem_process_blocked);
            break;

        case START_PLANNING:
            sem_wait(mutex_stopping_planner);
            sem_post(sem_short_term_planner);
            sem_post(sem_long_term_planner);
            // sem_post(sem_process_blocked); TODO: CHECKEAR ESTO
            break;

        case MODIFY_MULTIPROGRAMMING_DEGREE:
            modify_multiprogramming_degree(socket);
            break;

        case SHOW_ALL_PROCESSES:
            show_all_processes();
            break;
        case -1:
            log_error(logger, "Client was disconnected from %s", socket_name);
            break;
        default:
            log_error(logger, "Incorrect operation code (%s) from %s", translate_header(header), socket_name);
            break;
        }
    }
}

void modify_multiprogramming_degree(int socket)
{
    log_info(logger, "Trying change multiprogramming degree");
    int32_t new_degree = get_new_degree_from_socket(socket);
    sem_init(sem_multiprogrammming_degree, 0, new_degree);
    log_trace(logger, "The new multiprogramming degree is: %d", new_degree);
}

void show_all_processes()
{
    log_info(logger, "Trying to show all current process");
    t_list *all_queues_in_list = build_all_queues_in_list();
    t_pcb *process_to_show;
    t_queue *queue_to_show;
    for (int queue_index = 0; queue_index < list_size(all_queues_in_list); queue_index++)
    {
        queue_to_show = list_get(all_queues_in_list, queue_index);
        for (int process_index = 0; process_index < queue_size(queue_to_show); process_index++)
        {
            process_to_show = list_get(queue_to_show->elements, process_index);
            log_trace(logger, "Process with ID: %d - STATE: %s", process_to_show->pid, translate_process_state(process_to_show->state));
        }
    }

    // destroy_pcb(process_to_show); TODO: check this function, trigger error: invalid pointer
    /*
    if (queue_size(queue_new) == 0 && queue_size(queue_ready) == 0 && queue_size(queue_exec) == 0)
    {
        log_trace(logger, "There are no processes to show");
    }
    */
}

void handle_dispatch_connection(void *args)
{
    uint8_t header;
    t_pcb *pcb;

    t_buffer *payload;
    char *resource_requested;
    while (1)
    {
        log_info(logger, "Listenning KERNEL - CPU_DISPATCH");
        header = receive_header(cpu_dispatch_connection);
        log_info(logger, "OPERATION RECEIVE (%s) FROM CPU_DISPATCH", translate_header(header));
        sem_wait(mutex_cpu_dispatch_connection);
        switch (header)
        {
        case FINISH_PROCESS:
            pcb = get_pcb_from_socket(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            finish_process(pcb->pid);
            break;
        case BLOCK_PROCESS:
            pcb = get_pcb_from_socket(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            // move_process_from_exec_to_blocked();
            log_trace(logger, "Process with ID:%d was blocked", pcb->pid);
            // destroy_pcb(pcb); // TODO: check if is necessary?
            break;
        case INTERRUPT:
            /* TODO: check if is important to send interruption motive
            payload = receive_payload(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            pcb = take_pcb_from_buffer(payload);
            uint32_t interruption_motive = take_UINT32_from_buffer(payload);
            destroy_buffer(payload);
            */
            pcb = get_pcb_from_socket(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            log_info(logger, "Process with ID: %d was Interrupted", pcb->pid);

            /*TODO: duplicate code in move_process_from_execution_to_ready() */
            sem_wait(mutex_queue_exec);
            queue_pop(queue_exec);
            sem_post(mutex_queue_exec);

            sem_post(sem_cpu_available);

            log_trace(logger, "Moving process with ID:%d from EXECUTION queue to READY queue", pcb->pid);
            pcb->state = READY;

            sem_wait(mutex_queue_ready);
            queue_push(queue_ready, (void *)pcb);
            sem_post(mutex_queue_ready);

            sem_post(sem_process_ready);

            break;
        case WAIT_REQUEST:
            payload = receive_payload(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            pcb = take_pcb_from_buffer(payload);
            resource_requested = take_string_from_buffer(payload);
            log_trace(logger, "WAIT received for process with ID: %d", pcb->pid);
            handle_wait_interruption(pcb, resource_requested);
            destroy_buffer(payload);
            break;
        case SIGNAL_REQUEST:
            payload = receive_payload(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            pcb = take_pcb_from_buffer(payload);
            resource_requested = take_string_from_buffer(payload);
            log_trace(logger, "SIGNAL received for process with ID: %d", pcb->pid);

            handle_signal_interruption(pcb, resource_requested);

            destroy_buffer(payload);
            break;
        case SLEEP_REQUEST:
            payload = receive_payload(cpu_dispatch_connection);
            sem_post(mutex_cpu_dispatch_connection);
            pcb = take_pcb_from_buffer(payload);
            int time_to_sleep = take_INT32_from_buffer(payload);
            log_trace(logger, "SLEEP received for process with ID: %d", pcb->pid);

            handle_sleep_interruption(pcb, time_to_sleep);

            destroy_buffer(payload);
            break;
        case PAGE_FAULT_REQUEST:
            sem_post(mutex_cpu_dispatch_connection);
            handle_page_fault_interruption();
            break;
        case -1:
            sem_post(mutex_cpu_dispatch_connection);
            log_error(logger, "An error has Ocurred with CPU connection");
            return;

        default:
            sem_post(mutex_cpu_dispatch_connection);
            log_error(logger, "Incorrect operation code (%s)", translate_header(header));
            return;
        }
    }
}

void handle_sleep_interruption(t_pcb *process_to_move, int time_to_sleep)
{
    block_process(process_to_move, time_to_sleep);
    sem_post(sem_cpu_available);
}

void block_process(t_pcb *process_to_move, int time_to_sleep)
{
    pthread_t thread_id;
    t_thread_args args = {process_to_move, time_to_sleep};
    pthread_create(&thread_id, NULL, thread_sleep_process, (void *)&args);
    pthread_detach(thread_id);
}

void *thread_sleep_process(void *arg)
{
    t_thread_args *args = (t_thread_args *)arg;
    int time_to_sleep = args->sleep_time;
    t_pcb *process_to_move = args->process;

    sem_wait(mutex_queue_exec);
    (t_pcb *)queue_pop(queue_exec);
    sem_post(mutex_queue_exec);

    log_trace(logger, "SLEEP for process with ID:%d", process_to_move->pid);
    usleep(time_to_sleep);

    log_trace(logger, "Moving process with ID:%d from BLOCKED queue to READY queue", process_to_move->pid);
    process_to_move->state = READY;

    process_to_move->program_counter += 1;

    sem_wait(mutex_queue_ready);
    queue_push(queue_ready, (void *)process_to_move);
    sem_post(mutex_queue_ready);

    sem_post(sem_process_ready);
    pthread_exit(NULL);
}

void handle_end_of_quantum_interruption()
{
    log_info(logger, "A process was received interrupted by the end of quantum");
    // TODO send process
    move_process_from_execution_to_ready();
}

void handle_priority_interruption()
{
    log_info(logger, "A process was received interrupted by replanning with priority");
    // TODO send process
    move_process_from_execution_to_ready();

    // sort_queue();
    // log_info(logger, "Ready queue has been sorted by priorities");
    // move_process_to_exec();
}

void handle_page_fault_interruption()
{
    log_info(logger, "An interruption was recieved by page fault");
    t_pcb *process_to_move = get_pcb_from_socket(cpu_dispatch_connection);
    block_process_by_page_fault(process_to_move);
}

void *thread_page_fault_process(t_pcb *process_to_move)
{
    sem_wait(mutex_queue_exec);
    (t_pcb *)queue_pop(queue_exec);
    sem_post(mutex_queue_exec);

    log_error(logger, "Process with ID:%d BLOCKED BY PAGE FAULT ", process_to_move->pid);

    process_to_move->state = BLOCKED;
    process_to_move->program_counter += 1;

    // TODO: trigger action in memory so that this function makes sense
    send_page_to_load(memory_connection, PAGE_FAULT_REQUEST, process_to_move->table_pages);

    // TODO: Check if memory responds
    uint8_t memory_response = receive_header(memory_connection);
    log_info(logger, "Page fault response received from memory: %d", memory_response);
    if (memory_response == MEMORY_OK)
    {
        process_to_move = get_pcb_from_socket(cpu_dispatch_connection);
        sem_wait(mutex_queue_ready);
        queue_push(queue_ready, (void *)process_to_move);
        sem_post(mutex_queue_ready);

        log_info(logger, "Memory response ok moving process from BLOCKED to READY");
    }

    sem_post(sem_process_ready);
    pthread_exit(NULL);
}

void block_process_by_page_fault(t_pcb *process_to_move)
{
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, thread_page_fault_process, (void *)&process_to_move);
    pthread_detach(thread_id);
}

void handle_interrupt_connection(void *args)
{
    uint8_t header;
    t_pcb *pcb;

    while (1)
    {
        log_info(logger, "Listenning KERNEL - CPU_INTERRUPT");
        header = receive_header(cpu_interrupt_connection);
        log_info(logger, "OPERATION RECEIVE (%s) FROM CPU_INTERRUPT", translate_header(header));

        sem_wait(mutex_cpu_interrupt_connection);
        switch (header)
        {
        case REJECTED_INTERRUPTION:
            sem_post(mutex_cpu_interrupt_connection);
            log_info(logger, "A process was received by interrupted rejected");
            break;
        case INTERRUPT_QUANTUM:
            sem_post(mutex_cpu_interrupt_connection);
            log_info(logger, "A process was received interrupted by the end of quantum");
            break;
        case INTERRUPT_PRIORITY:
            sem_post(mutex_cpu_interrupt_connection);
            log_info(logger, "A process was received interrupted by replanning with priority");
            break;
        case PAGE_FAULT_REQUEST:
            sem_post(mutex_cpu_interrupt_connection);
            log_info(logger, "An interruption was recieved by page fault");
            break;
        case -1:
            sem_post(mutex_cpu_interrupt_connection);
            log_error(logger, "An error has Ocurred with CPU connection");
            return;

        default:
            sem_post(mutex_cpu_interrupt_connection);
            log_error(logger, "Incorrect operation code (%s)", translate_header(header));
            return;
        }
    }
}

t_pcb *create_pcb(t_process *process, int socket)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    // TODO: may be need mutex
    pcb->pid = id_to_assign;
    pcb->program_counter = 0;
    pcb->quantum = process->quantum;
    // TODO: check if necessary size
    pcb->size = 0;

    pcb->cpu_register = list_create();
    list_add(pcb->cpu_register, 0);
    list_add(pcb->cpu_register, 0);
    list_add(pcb->cpu_register, 0);
    list_add(pcb->cpu_register, 0);

    pcb->resources = list_create();
    pcb->table_pages = 0;
    pcb->path = process->path;
    pcb->state = NEW;
    pcb->client_socket = socket;

    id_to_assign++;

    return pcb;
}

t_pcb *receive_pcb_and_create(int socket)
{
    t_buffer *payload = receive_payload(socket);
    t_process *process = take_process_from_buffer(payload);
    t_pcb *pcb = create_pcb(process, socket);

    free(process);
    destroy_buffer(payload);

    return pcb;
}
