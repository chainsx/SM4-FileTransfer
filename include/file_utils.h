#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#ifndef FILETRANSFER_FILECOPY_H
#define FILETRANSFER_FILECOPY_H

void error_quit(const char *msg);

void change_path(const char *path);

void rm_dir(const char *path);

int copy_by_block(const char *src_file_name, const char *dest_file_name);

#endif //FILETRANSFER_FILECOPY_H
