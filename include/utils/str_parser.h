#ifndef STR_PARSER_H
#define STR_PARSER_H

/**
    converts string to int* array

    @param char* str the string to convert int* n the size of the array after parsing
    @return int* the array of the numbers inside the string note: this function uses malloc to create the array of numbers
    the returned pointer needs to be fread to avoid memory leaks
*/
int *parse_ints(char *str, int *n);

/**

    converts string to double array

    @param char* str the string to convert int* n the size of the array after parsing
    @return double* the array of the numbers inside the string note: this function uses malloc to create the array of numbers
    the returned pointer needs to be fread to avoid memory leaks
*/
double *parse_doubles(char *str, int *n);

#endif // STR_PARSER_H