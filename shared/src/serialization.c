#include "serialization.h"

t_buffer *create_buffer(size_t buffer_size)
{
    t_buffer *buffer_to_create = malloc(sizeof(t_buffer));
    buffer_to_create->offset = 0;
    buffer_to_create->buffer_size = buffer_size;
    buffer_to_create->stream = malloc(buffer_size);
    return buffer_to_create;
}

void destroy_buffer(t_buffer *buffer)
{
    free(buffer->stream);
    free(buffer);
}

void add_to_buffer(t_buffer *buffer, void *information_to_add, size_t buffer_size)
{
    change_buffer_size(buffer, buffer_size);
    memcpy(buffer->stream + buffer->offset, information_to_add, buffer_size);
    buffer->offset += buffer_size;
}

void change_buffer_size(t_buffer *buffer, size_t buffer_size)
{
    while (buffer->buffer_size < buffer->offset + buffer_size)
    {
        buffer->buffer_size += DEFAULT_BUFFER_SIZE;
        buffer->stream = realloc(buffer->stream, buffer->buffer_size);
    }
}

t_package *create_package(uint8_t header, size_t buffer_size)
{
    t_package *package_to_create = malloc(sizeof(t_package));
    package_to_create->header = header;
    package_to_create->payload = create_buffer(buffer_size);
    return package_to_create;
}

void destroy_package(t_package *package)
{
    destroy_buffer(package->payload);
    free(package);
}

void add_INT32_to_buffer(t_buffer *buffer, int32_t value_to_add)
{
    add_to_buffer(buffer, (void *)&value_to_add, sizeof(int32_t));
}

void add_UINT32_to_buffer(t_buffer *buffer, uint32_t value_to_add)
{
    add_to_buffer(buffer, (void *)&value_to_add, sizeof(uint32_t));
}

void add_UINT8_to_buffer(t_buffer *buffer, uint8_t value_to_add)
{
    add_to_buffer(buffer, (void *)&value_to_add, sizeof(uint8_t));
}

void add_string_to_buffer(t_buffer *buffer, char *value_to_add)
{
    uint32_t size = string_length((char *)value_to_add) + 1;
    add_UINT32_to_buffer(buffer, size);
    add_to_buffer(buffer, (void *)value_to_add, size);
}

void add_list_to_buffer(t_buffer *buffer, t_list *list_to_add, void (*add_type_list_to_buffer)(t_buffer *, void *))
{
    void _add_to_list(void *list_element)
    {
        add_type_list_to_buffer(buffer, list_element);
    };

    uint32_t list_size = list_to_add->elements_count;
    add_UINT32_to_buffer(buffer, list_size);
    list_iterate(list_to_add, _add_to_list);
}

void add_double_to_buffer(t_buffer *buffer, double value_to_add)
{
    add_to_buffer(buffer, (void *)&value_to_add, sizeof(double));
}

void buffer_take(t_buffer *buffer, void **destination, size_t take_to_size)
{
    if (*destination == NULL)
        *destination = calloc(1, take_to_size);
    memcpy(*destination, buffer->stream + buffer->offset, take_to_size);
    buffer->offset += take_to_size;
}

int32_t take_INT32_from_buffer(t_buffer *buffer)
{
    int32_t value_taked;
    int32_t *value_direction = &value_taked;
    buffer_take(buffer, (void **)&value_direction, sizeof(int32_t));
    return value_taked;
}

uint32_t take_UINT32_from_buffer(t_buffer *buffer)
{
    uint32_t value_taked;
    uint32_t *value_direction = &value_taked;
    buffer_take(buffer, (void **)&value_direction, sizeof(uint32_t));
    return value_taked;
}

uint8_t take_UINT8_from_buffer(t_buffer *buffer)
{
    uint8_t value_taked;
    uint8_t *value_direction = &value_taked;
    buffer_take(buffer, (void **)&value_direction, sizeof(uint8_t));
    return value_taked;
}

char *take_string_from_buffer(t_buffer *buffer)
{
    char *value_taked = NULL;
    uint32_t value_size = take_UINT32_from_buffer(buffer);
    buffer_take(buffer, (void **)&value_taked, value_size);
    return value_taked;
}

t_list *take_list_from_buffer(t_buffer *buffer, void *(*take_value_from_buffer)(t_buffer *))
{
    t_list *list_to_take = list_create();
    uint32_t list_size = take_UINT32_from_buffer(buffer);

    for (uint32_t i = 0; i < list_size; i++)
    {
        void *elemement_taked = take_value_from_buffer(buffer);
        list_add(list_to_take, elemement_taked);
    }

    return list_to_take;
}

double take_double_from_buffer(t_buffer *buffer)
{
    double value_taked;
    double *value_direction = &value_taked;
    buffer_take(buffer, (void **)&value_direction, sizeof(double));
    return value_taked;
}