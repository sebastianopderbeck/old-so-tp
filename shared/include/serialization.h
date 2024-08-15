#ifndef SHARED_SERIALIZATION_H_
#define SHARED_SERIALIZATION_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#define DEFAULT_BUFFER_SIZE 64
typedef struct
{
  uint32_t offset;
  size_t buffer_size;
  void *stream;
} t_buffer;

typedef struct
{
  uint8_t header;
  t_buffer *payload;
} t_package;

t_buffer *create_buffer(size_t buffer_size);

void destroy_buffer(t_buffer *buffer);

void add_to_buffer(t_buffer *buffer, void *information_to_add, size_t buffer_size);

void change_buffer_size(t_buffer *buffer, size_t buffer_size);

t_package *create_package(uint8_t header, size_t buffer_size);

void destroy_package(t_package *package);

void add_INT32_to_buffer(t_buffer *buffer, int32_t value_to_add);

void add_UINT32_to_buffer(t_buffer *buffer, uint32_t value_to_add);

void add_UINT8_to_buffer(t_buffer *buffer, uint8_t value_to_add);

void add_string_to_buffer(t_buffer *buffer, char *value_to_add);

void add_list_to_buffer(t_buffer *buffer, t_list *list_to_add, void (*add_type_list_to_buffer)(t_buffer *, void *));

void add_double_to_buffer(t_buffer *buffer, double value_to_add);

void buffer_take(t_buffer *buffer, void **destination, size_t take_to_size);

int32_t take_INT32_from_buffer(t_buffer *buffer);

uint32_t take_UINT32_from_buffer(t_buffer *buffer);

uint8_t take_UINT8_from_buffer(t_buffer *buffer);

char *take_string_from_buffer(t_buffer *buffer);

t_list *take_list_from_buffer(t_buffer *buffer, void *(*take_value_from_buffer)(t_buffer *));

double take_double_from_buffer(t_buffer *buffer);

#endif