#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#ifndef FILETRANSFER_FILECOPY_H
#define FILETRANSFER_FILECOPY_H

void error_quit(const char *msg)
{
    perror(msg);
    exit(-1);
}

void change_path(const char *path)
{
    printf("Leave %s Successed . . .\n",getcwd(NULL,0));

    if(chdir(path)==-1)
        error_quit("chdir");

    printf("Entry %s Successed . . .\n",getcwd(NULL,0));
}

void rm_dir(const char *path)
{
    DIR *dir;
    struct dirent *dirp;
    struct stat buf;
    char *p=getcwd(NULL,0);

    if((dir=opendir(path))==NULL)
        error_quit("OpenDir");

    change_path(path);

    while(dirp=readdir(dir))
    {
        if((strcmp(dirp->d_name,".")==0) || (strcmp(dirp->d_name,"..")==0))
            continue;

        if(stat(dirp->d_name,&buf)==-1)
            error_quit("stat");

        if(S_ISDIR(buf.st_mode))
        {
            rm_dir(dirp->d_name);
            continue;
        }

        if(remove(dirp->d_name)==-1)
            error_quit("remove");

        printf("rm %s Successed . . .\n",dirp->d_name);
    }

    closedir(dir);
    change_path(p);

    if(rmdir(path)==-1)
        error_quit("rmdir");

    printf("rm %s Successed . . .\n",path);
}

int copy_by_block(const char *src_file_name, const char *dest_file_name) {//ok
    FILE *fp1 = fopen(dest_file_name,"w");
    FILE *fp2 = fopen(src_file_name,"r");
    if(fp1 == NULL) {
        perror("fp1:");
        return -1;
    }
    if(fp2 == NULL) {
        perror("fp2:");
        return -1;
    }
    void *buffer = (void *)malloc(2);
    int cnt = 0;
    while(1) {
        int op = fread(buffer,1,1,fp2);
        if(op <= 0) break;
        fwrite(buffer,1,1,fp1);
        cnt++;
    }
    free(buffer);
    fclose(fp1);
    fclose(fp2);
    return cnt;
}

#endif //FILETRANSFER_FILECOPY_H
