#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <mmapFile.h>
#include <transfer.h>
#include "miniz/zip.h"
#include "filecopy.h"
#include "gmssl/sm4_file.h"

#define ADDR "127.0.0.1"
#define PORT 8080
#define MAXEVENTS 32
#define BUF_SIZE 1024
#define FILENAMESIZE 48

const int _ISOK_ = 0;
const int _NOOK_ = 1;

static int set_socket_non_block(int sfd);
static int socket_and_bind();
static void *handler(void *arg);
static void recv_fileinfo_and_create_file(uint8_t *buf, int cfd);
static int recv_filedata_and_copy_2_file(uint8_t *buf, int cfd);
static int dataNo = 0;
char FILENAME[FILENAMESIZE] = {0};
char *begin_ = NULL;

int main(int argc, char const *argv[])
{
    if (argc < 4)
    {
        perror("usage:server <key> <iv> <cbc/ctr>");
        exit(1);
    }

    char keyhex[2048];
    strcpy(keyhex, argv[1]);
    char ivhex[2048];
    strcpy(ivhex, argv[2]);
    char act[10];
    strcpy(act, "decrypt");
    char u_mode[10];
    strcpy(u_mode, argv[3]);
    char in_data[2048];
    strcpy(in_data, "foo.ms4");
    char out_data[2048];
    strcpy(out_data, "foo.zip");

    if (access("file", 0) == 0) {
        rm_dir("file");
    }
    mkdir("file",0777);

    int sockfd, epfd, cfd, ret, clientaddrLen;
    struct epoll_event event, events[MAXEVENTS];
    struct sockaddr clientAddr;
    char buf[BUF_SIZE] = {0};
    clientaddrLen = sizeof(struct sockaddr);

    sockfd = socket_and_bind();
    if (sockfd < 0)
    {
        perror("error: socket_and_bind\n");
        exit(1);
    }

    ret = set_socket_non_block(sockfd);
    if (ret < 0)
    {
        perror("error: set_socket_non_block\n");
        exit(1);
    }

    ret = listen(sockfd, SOMAXCONN);
    if (ret < 0)
    {
        perror("error: listen\n");
        exit(1);
    }

    epfd = epoll_create(1);
    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = sockfd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

    while (1)
    {
        int events_count = epoll_wait(epfd, events, MAXEVENTS, -1);
        int i = 0;
        for (i = 0; i < events_count; i++)
        {
            if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR) ||
                !(events[i].events & EPOLLIN))
            {
                perror("events sockfd error!");
                close(events[i].data.fd);
                continue;
            }
            if ((events[i].data.fd == sockfd) && (events[i].events & EPOLLIN))
            {
                while (1)
                {
                    cfd = accept(sockfd, &clientAddr, &clientaddrLen);
                    if (cfd == -1)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            sm4_main(keyhex, ivhex, act, u_mode, in_data, out_data);
                            zip_extract("foo.zip", "file", NULL, NULL);
                            if( remove("foo.zip") == 0 )
                                printf("Removed tmp zip file.");
                            else
                                perror("remove");
                            printf("haha--\n");
                            break;
                        }
                        else
                        {
                            perror("accept");
                            printf("--haha\n");
                            continue;
                        }
                    }
                    pthread_t pthread_;
                    ret = pthread_create(&pthread_, NULL, handler, (void *)&cfd);
                    if (ret != 0)
                    {
                        perror("pthread_create");
                        printf("%d\n", ret);
                    }
                    sleep(1);
                }
            }
        }
    }

    return 0;
}

int set_socket_non_block(int sfd)
{
    int flags, res;

    flags = fcntl(sfd, F_GETFL);
    if (flags == -1)
    {
        perror("error : cannot get socket flags!\n");
        exit(1);
    }

    flags |= O_NONBLOCK;
    res = fcntl(sfd, F_SETFL, flags);
    if (res == -1)
    {
        perror("error : cannot set socket flags!\n");
        exit(1);
    }

    return 0;
}

int socket_and_bind()
{
    int fd, ret;
    struct sockaddr_in addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket create error\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ADDR);

    ret = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret < 0)
    {
        perror("bind sockfd error\n");
        exit(1);
    }

    return fd;
}

void *handler(void *arg)
{
    pthread_detach(pthread_self());
    int cfd = *(int *)arg;
    uint8_t buf[HAND_TOTAL_SIZE + 1] = {0};
    int dataRes = 0;
    while (1)
    {
        int rets = recv(cfd, buf, HAND_TOTAL_SIZE, 0);
        if(rets < 0){
            perror("recv");
        }
        //uint8_t* piece_buf = buf;
        if ((buf[0] == 0xAF) && (buf[1] == 0xAE)) //接收文件信息
        {
            printf("recv file information success!\n");
            recv_fileinfo_and_create_file(buf, cfd);
            //return NULL;
            pthread_exit(NULL);
        }
        else if ((buf[0] == 0xAF) && (buf[1] == 0xAF))
        {
            printf("recv file data success\n");
            dataRes = recv_filedata_and_copy_2_file(buf, cfd);
            if (dataRes == _ISOK_)
            {
                pthread_exit(NULL);
            }
        }
    }
}

void recv_fileinfo_and_create_file(uint8_t *buf, int cfd)
{
    int fileSize = (buf[HEAD_POS_TOTAL_SIZE] << 8) + (buf[HEAD_POS_TOTAL_SIZE + 1]);
    int fileNameSize = (buf[HEAD_POS_P_LENGTH] << 8) + (buf[HEAD_POS_P_LENGTH + 1]);
    uint8_t fileName[FILENAMESIZE] = {0};
    int rets = recv(cfd, fileName, fileNameSize, 0);
    if(rets < 0){
        perror("recv");
    }

    printf("文件大小为%d; 文件名大小为%d; 文件名为%s\n", fileSize, fileNameSize, fileName);

    create_file(fileName, fileSize);
    close(cfd);
    begin_ = mmap_file(fileName, fileSize);
    return;
}

int recv_filedata_and_copy_2_file(uint8_t *buf, int cfd)
{
    int fileSize = (buf[HEAD_POS_TOTAL_SIZE] << 8) + (buf[HEAD_POS_TOTAL_SIZE + 1]);       //文件数据大小
    int filePieces = (buf[HEAD_POS_TOTAL_PIECES] << 8) + (buf[HEAD_POS_TOTAL_PIECES + 1]); //分片数
    int pieceNo = (buf[HEAD_POS_P_INDEX] << 8) + (buf[HEAD_POS_P_INDEX + 1]);              //分片编号
    int pieceSize = (buf[HEAD_POS_P_LENGTH] << 8) + (buf[HEAD_POS_P_LENGTH + 1]);          //分片数据大小
    int clientPthreads = (buf[CLIENT_PTHREAD_SIZE] << 8) + (buf[CLIENT_PTHREAD_SIZE + 1]); //客户端线程数

    while (dataNo != pieceNo);

    int rets = recv(cfd, begin_ + (pieceNo * PIECE_DATA_SIZE), pieceSize, 0);
    if(rets < 0){
        perror("recv");
    }
    dataNo++;

    printf("文件大小为%d; 文件总分片为%d; 文件编号为%d; 分片数据为%d; 客户端线程数%d\n", 
            fileSize, filePieces, pieceNo, pieceSize, clientPthreads);

    if (pieceNo + clientPthreads + 1 > filePieces)
    {
        close(cfd);
        return _ISOK_;
    }
    return _NOOK_;
}
