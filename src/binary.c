#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


void readBinaryFile(char *filename, struct Binary *binary){
    FILE *file = fopen(filename, "rb");
    if (file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct BinaryHeader *headers = &binary->headers;

    for (size_t i = 0; i < sizeof(headers->magic_number); i++){
        headers->magic_number |= ((uint32_t) fgetc(file)) << (i * 8);
    }

    if (headers->magic_number != MAGIC_NUMBER){
        fprintf(stderr, "File format not recognized : %x\n", headers->magic_number);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < sizeof(headers->entry_point); i++){
        headers->entry_point |= ((uint64_t) fgetc(file)) << (i * 8);
    }

    for (size_t i = 0; i < sizeof(headers->code_size); i++){
        headers->code_size |= ((uint64_t) fgetc(file)) << (i * 8);
    }

    for (size_t i = 0; i < headers->code_size; i++){
        binary->bytecode[i] = (uint8_t) fgetc(file);
    }
}

void writeBinaryFile(char *filename, struct Binary *binary){

    FILE *file = fopen(filename, "wb");
    if (file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    struct BinaryHeader *headers = &binary->headers;
    uint8_t byte;

    for (size_t i = 0; i < sizeof(headers->magic_number); i++){
        byte = (uint8_t) (headers->magic_number >> (i * 8));
        fputc(byte, file);
        printf("%x", byte);
    }
    printf("\n");
 
    for (size_t i = 0; i < sizeof(headers->entry_point); i++){
        byte = (uint8_t) (headers->entry_point >> (i * 8));
        fputc(byte, file);
    }
 
    for (size_t i = 0; i < sizeof(headers->code_size); i++){
        byte = (uint8_t) (headers->code_size >> (i * 8));
        fputc(byte, file);
    }
    
    for (size_t i = 0; i < headers->code_size; i++){
        fputc(binary->bytecode[i], file);
    }
}