#ifndef _MMAPFILE_H_
#define _MMAPFILE_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

char* mmap_file(char* fileName, int file_Size);

void destory_mmap(char *buf, int fileSize);

int getFileSize(char* fileName);

int create_file(char* fileName, int file_Size);

#endif