#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "messages.h"
#include "send.h"
#include "get.h"
#include "connect.h"
#include "file_utils.h"
#include "miniz/zip.h"
#include "miniz/zip_walk.h"
#include "gmssl/sm4_file.h"

#define VERSION         "0.1"

void usage(void);

int main(int argc, char *argv[]) {

    int socket;
    int check_send = 0;
    int check_get = 0;
    char *ipaddr = NULL;
    char *keyhex = NULL;
    char *ivhex = NULL;
    char *mode = NULL;
    char in_data[2048];
    char out_data[2048];
    char act[10];

    if (argc <= 1)
    {
        usage();
    }

    int argc_num = argc;
    int files_num_start;

    for(int m=0; m<argc; m++) {
        for (int i=0; i<argc_num; i++) {
            if (strcmp(argv[i], "-ip") == 0) {
                ipaddr = argv[i+1];
            }
            else if (strcmp(argv[i], "-key") == 0) {
                keyhex = argv[i+1];
            }
            else if (strcmp(argv[i], "-iv") == 0) {
                ivhex = argv[i+1];
            }
            else if (strcmp(argv[i], "-mode") == 0) {
                mode = argv[i+1];
            }
            else if (strcmp(argv[i], "-send") == 0) {
                check_send = 1;
            }
            else if (strcmp(argv[i], "-get") == 0) {
                check_get = 1;
            }
            else if (strcmp(argv[i], "-files") == 0) {
                files_num_start = i+1;
            }

        }
    }


    printf("\n");
    printf("+-------+----------------------------------------+\n");
    printf("|  key  |  %s\n", keyhex);
    printf("|  iv   |  %s\n", ivhex);
    printf("|  mode |  %s\n", mode);
    printf("|  ip   |  %s\n", ipaddr);
    printf("|  argc |  %d\n", argc);
    printf("+-------+----------------------------------------+\n");
    printf("\n");

    if (check_send) {
        if (argc < 10)
        {
            usage();
        }

        if (access("file", 0) == 0) {
            rm_dir("file");
        }
        mkdir("file",0777);
        for (int k=0; k<argc_num-9; k++) {
            printf("processing %s\n", argv[files_num_start+k]);
            char mv_target_name[2048] = "file/";
            strcat(mv_target_name, argv[files_num_start+k]);
            copy_by_block(argv[files_num_start+k],mv_target_name);
        }
        struct zip_t *zip = zip_open("foo.zip", 5, 'w');
        zip_walk(zip, "file");
        zip_close(zip);

        if (access("file", 0) == 0) {
            rm_dir("file");
        }

        strcpy(in_data, "foo.zip");
        strcpy(out_data, "foo.ms4");
        strcpy(act, "encrypt");
        sm4_file(keyhex, ivhex, act, mode, in_data, out_data);
        //remove("foo.zip");

        socket = connectForSend();
        sendFile(socket, "foo.ms4");
        closeConnect(socket);
        //remove("foo.ms4");
    }

    if (check_get) {
        if (argc < 10)
        {
            usage();
        }
        if (access("file", 0) == 0) {
            rm_dir("file");
        }
        printf("%d\n", argc);
        strcpy(in_data, "foo.ms4");
        strcpy(out_data, "foo.zip");
        strcpy(act, "decrypt");

        socket = connectForGet(ipaddr);
        getFile(socket, "foo.ms4");
        sm4_file(keyhex, ivhex, act, mode, in_data, out_data);
        //remove("foo.ms4");
        zip_extract("foo.zip", ".", NULL, NULL);
        //remove("foo.zip");
        closeConnect(socket);
    }

    return 0;
}

void usage(void)
{
    printf("\nFilertransfer v%s (Usage)\n"
           "Send a file  : filetransfer -send -key <key> -iv <iv> -mode <cbc/crt> -files <filepath>\n"
           "Get a file   : filetransfer -get -ip <ipaddr> -key <key> -iv <iv> -mode <cbc/crt>\n"
           "Help         : filetransfer -help\n\n", VERSION);

    exit(EXIT_FAILURE);
}
