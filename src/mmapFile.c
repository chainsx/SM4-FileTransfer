#include "mmapFile.h"

char* mmap_file(char* fileName, int file_Size)
{
    int fd = open(fileName, O_RDWR);
    char* buf = (char*)mmap(0, file_Size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return buf;
}

void destory_mmap(char *buf, int fileSize)
{
    munmap(buf, fileSize);
}

int getFileSize(char* filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
    return size;
}

int create_file(char* fileName, int file_Size)
{
    int fd = open(fileName, O_RDWR|O_CREAT, 0666);
    if(fd < 0){
        perror("open");
        return -1;
    }

    lseek(fd, file_Size-1, SEEK_SET);
    write(fd, "", 1);
    close(fd);
    return 0;
}


