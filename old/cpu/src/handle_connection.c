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
    uint8_t header, interruption_motive;
    t_buffer *buffer;

    while (1)
    {
        header = receive_header(socket);
        log_info(logger, "OPERATION RECEIVE (%s) FROM KERNEL", translate_header(header));
        // sem_wait(mutex_cpu_dispatch_connection);

        switch (header)
        {
        case EXECUTE_PROCESS:
            execute_process(socket);
            break;
        case INTERRUPT:
            log_trace(logger, "PROCESS RECIEVED FOR INTERRUPTION");
            sem_wait(mutex_interrupt);
            has_interrupt = true;
            // interruption_motive = INTERRUPT_QUANTUM;
            sem_post(mutex_interrupt);
            break;
        case INTERRUPT_QUANTUM:
            log_trace(logger, "PROCESS RECIEVED FOR END OF QUANTUM");
            sem_wait(mutex_interrupt);
            has_interrupt = true;
            // interruption_motive = INTERRUPT_QUANTUM;
            sem_post(mutex_interrupt);
            break;
        case WAIT_RESPONSE:
            execute_process(socket);
            break;
        case SIGNAL_RESPONSE:
            execute_process(socket);
            break;
        case -1:
            log_error(logger, "An error has Ocurred with KERNEL connection");
            return;
        default:
            log_error(logger, "Incorrect operation code (%s)", translate_header(header));
            return;
        }
    }
}

void execute_process(int socket)
{
    t_pcb *process = get_pcb_from_socket(socket);
    log_trace(logger, "Receiving process with ID:%d for execute", process->pid);
    bool end_of_execution = false;

    do
    {
        end_of_execution = false;
        send_pcb(memory_connection, INSTRUCTIONS, process);
        log_info(logger, "Requesting instruction to Memory");

        /*TEST */
        uint8_t header = receive_header(memory_connection);
        t_buffer *payload = receive_payload(memory_connection);
        char *instruction = take_string_from_buffer(payload);

        destroy_buffer(payload);

        // char *instruction = get_instruction_from_socket(memory_connection);

        log_info(logger, "Recieving instruction (%s) from Memory", instruction);
        int execution_success = execute_instruction_cycle(socket, process, instruction);
        end_of_execution = execution_success != 1;

        process->program_counter += 1;

        sem_wait(mutex_interrupt);
        if (has_interrupt)
        {
            log_warning(logger, "Stopping execution");
            if (end_of_execution)
            {
                // send_header(socket, REJECTED_INTERRUPTION);
                sem_post(mutex_interrupt);
                has_interrupt = false;
                sem_wait(mutex_interrupt);
                continue;
            }
            log_info(logger, "Sending interrupted process with ID:%d", process->pid);
            send_pcb(socket, INTERRUPT, process);

            /* TODO: check if is important to send interruption motive
            t_package *package = create_package(INTERRUPT, sizeof(t_pcb));
            add_pcb_to_buffer(package, process);
            add_UINT8_to_buffer(package, interruption_motive);
            send_package(socket, package);
            destroy_package(package);
            */
            end_of_execution = true;
            has_interrupt = false;
            destroy_pcb(process);
        }
        sem_post(mutex_interrupt);
    } while (!end_of_execution);
}