#include "instructions.h"

void handle_filesystem(uint32_t header)
{
  switch(header) {
    case EXIT: printf("EXIT"); break;
    case F_OPEN: printf("F_OPEN"); break;
    case F_CLOSE: printf("F_CLOSE"); break;
    case F_SEEK: printf("F_SEEK"); break;
    case F_TRUNCATE: printf("F_TRUNCATE"); break;
    case F_READ: printf("F_READ"); break;
    case F_WRITE: printf("F_WRITE"); break;
  }
}