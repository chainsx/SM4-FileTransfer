#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#include "get.h"
#include "connect.h"
#include "messages.h"

#include <stdio.h>
#include <stdlib.h>

#define FILENAMELEN         256
#define BUFSIZE             10000
#define FILEPATHLEN         256

#ifdef OS_WIN
    #include <windows.h>
#else
    #include <sys/socket.h>
#include <string.h>

#define SOCKET_ERROR        -1
#endif

static int read_socket(int s, void *pBuf, int n);

int getFile(int socket, const char *path)
{
    FILE *f;
    char filename[FILENAMELEN];
    int filesize;
    int result_read;
    char buf[BUFSIZE];
    int filesize_check, sum = 0;
    char filepath[FILEPATHLEN];

    if (recv(socket, filename, FILENAMELEN, 0) == SOCKET_ERROR)
        tool_error("Failed to get file properties");
    recv(socket, &filesize, sizeof(int), 0);

    strcpy(filepath, path);

     if ((f = fopen(filepath, "wb")) == NULL)
        tool_error("Can not create file");

    printf("* download retrieving ... (Size: %d Kb)\n\n", filesize);

    while ((result_read = read_socket(socket, buf, BUFSIZE)) != -1) {
        int n, k;

        if (result_read == 0)
           break;

        sum += fwrite(buf, 1, result_read, f);

        //graphic (==>(%100))
        if (filesize >= 1) {
            k = n = (sum / 1024 * 100) / (filesize);
            n /= 2;
            while (n--)
                printf("=");
            printf(">(%%%d)\r", k);
        }
    }

    if (result_read == -1)
        tool_error("File download failed");

    fseek(f, 0, SEEK_END);
    filesize_check = ftell(f) / 1024;

    if (filesize == filesize_check)
        tool_info("File downloaded");
    else
        tool_error("File transfer was not completed");

    fclose(f);

    return 0;
}

static int read_socket(int s, void *pBuf, int n)
{
    int result;
    int index = 0;
    int left = n;

    while (left > 0) {
        result = recv(s, (char *) pBuf + index , left, 0);

        if (result == 0)
            return index;

        if (result == SOCKET_ERROR)
            return SOCKET_ERROR;

        index += result;
        left -= result;

    }
    return index;
}
