#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <utils/reads.h>

char *readline(FILE *fd)
{
    if (fd == NULL) return NULL;

    int buffer_capacity = INITIAL_BUFFER_SIZE;
    char *buffer = (char *)malloc(buffer_capacity);
    if (buffer == NULL)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    int length = 0;
    int curr_char;

    while ((curr_char = fgetc(fd)) != EOF)
    {
        if (length >= buffer_capacity - 1)
        {
            buffer_capacity *= BUFFER_GROWTH_FACTOR;
            char *temp = (char *)realloc(buffer, buffer_capacity);
            if (temp == NULL)
            {
                perror("Failed to reallocate memory");
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }

        buffer[length++] = (char)curr_char;
        if (curr_char == '\n') break;
    }

    if (length == 0 && curr_char == EOF)
    {
        free(buffer);
        return NULL;
    }

    buffer = (char *)realloc(buffer, length + 1);
    buffer[length] = '\0';
    return buffer;
}

#define INITIAL_SIZE 10
#define GROWTH_FACTOR 2

char **read_lines(FILE *fd, int *n)
{
    fseek(fd, 0, SEEK_SET);

    char **Lines = NULL;
    int length = 0;
    char *line;

    while ((line = readline(fd)) != NULL)
    {
        if (length == 0)
        {
            Lines = (char **)malloc(sizeof(char *));
        }
        else
        {
            Lines = (char **)realloc(Lines, sizeof(char *) * (length + 2));
        }
        Lines[length] = line;
        length++;
        (*n)--;
        if (*n == 0)
        {
            break;
        }
    }
    (*n) = length;
    return Lines;
}
