#include "binary.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>


void readBinaryFile(char *filename, struct Binary *binary){
    int fd;
    CHK((fd = open(filename, O_RDONLY)));
    
    struct BinaryHeader *headers = &binary->headers;
    uint8_t buffer[HDR_SIZE] = {0};
    size_t cursor = 0;

    CHK(read(fd, buffer, HDR_SIZE));

    for (size_t i = 0; i < sizeof(headers->magic_number); i++){
        headers->magic_number |= ((uint32_t) buffer[cursor++]) << (i * 8);
    }

    if (headers->magic_number != MAGIC_NUMBER){
        fprintf(stderr, "File format not recognized : %x\n", headers->magic_number);
        CHK(close(fd));
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < sizeof(headers->entry_point); i++){
        headers->entry_point |= ((uint64_t) buffer[cursor++]) << (i * 8);
    }

    for (size_t i = 0; i < sizeof(headers->code_size); i++){
        headers->code_size |= ((uint64_t) buffer[cursor++]) << (i * 8);
    }

    for (size_t i = 0; i < sizeof(headers->data_size); i++){
        headers->data_size |= ((uint64_t) buffer[cursor++]) << (i * 8);
    }
    CHK(read(fd, binary->bytecode, headers->code_size));
    CHK(read(fd, binary->data, headers->data_size));

    CHK(close(fd));
}

void writeBinaryFile(char *filename, struct Binary *binary){

    int fd;

    CHK((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0755)));

    struct BinaryHeader *headers = &binary->headers;
    uint8_t buffer[HDR_SIZE];
    size_t cursor = 0;

    for (size_t i = 0; i < sizeof(headers->magic_number); i++){
        buffer[cursor++] = (uint8_t) (headers->magic_number >> (i * 8));
    }
    printf("\n");
 
    for (size_t i = 0; i < sizeof(headers->entry_point); i++){
        buffer[cursor++] = (uint8_t) (headers->entry_point >> (i * 8));
    }
 
    for (size_t i = 0; i < sizeof(headers->code_size); i++){
        buffer[cursor++] = (uint8_t) (headers->code_size >> (i * 8));
    }

    for (size_t i = 0; i < sizeof(headers->data_size); i++){
        buffer[cursor++] = (uint8_t) (headers->data_size >> (i * 8));
    }

    CHK(write(fd, buffer, HDR_SIZE));
    CHK(write(fd, binary->bytecode, headers->code_size));
    CHK(write(fd, binary->data, headers->data_size));
    CHK(close(fd));
}