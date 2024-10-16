#ifndef READS_H
#define READS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_BUFFER_SIZE 128
#define BUFFER_GROWTH_FACTOR 2

/**
    reads a line from a given file  

    note: this function uses malloc and realloc so the returned value must be freed after use

    @param FILE* fd file expected to be opens with fopen with read access
    @return char* the line read from the file / NULL if the file is empty or an error occurred
*/
char* readline(FILE* fd);

/**

    reads n lines from a given file  

    note: this function uses malloc and realloc so the returned value must be freed after use

    @param FILE* fd file expected to be opens with fopen with read access , int* n = number of lines to read -1 => all lines 
    @return char** contains the lines read each line is saved in char* and are sorted according to the original file 
            int* n the value inside n is changed according to the number of lines read from the file    
            
*/
char** read_lines(FILE* fd, int* n);

#endif // READS_H