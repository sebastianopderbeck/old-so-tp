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
    case MEMORY:
        log_info(logger, "Connected %s with %s", socket_name, translate_header(header));
        handle_memory_connection(socket);
        break;
    case -1:
        log_error(logger, "Client was disconnected from %s", socket_name);
        return;
    default:
        log_error(logger, "Incorrect operation code (%d) from %s", header, socket_name);
        return;
    }
}

void handle_memory_connection(int socket) {
    //TODO
}

void handle_kernel_connection(int socket)
{
    uint8_t header, interruption_motive;
    t_buffer *buffer;

    while (1)
    {
        header = receive_header(socket);
        log_info(logger, "OPERATION RECEIVE (%s) FROM KERNEL", translate_header(header));

        switch (header)
        {
        case EXECUTE_PROCESS:
            log_info(logger, "EXECUTE_PROCESS");
            break;
        case INTERRUPT:
            log_trace(logger, "PROCESS RECIEVED FOR INTERRUPTION");
            break;
        case INTERRUPT_QUANTUM:
            log_trace(logger, "PROCESS RECIEVED FOR END OF QUANTUM");
            break;
        case WAIT_RESPONSE:
            log_trace(logger, "WAIT_RESPONSE");
            break;
        case SIGNAL_RESPONSE:
            log_trace(logger, "SIGNAL_RESPONSE");
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
