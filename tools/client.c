#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <file_utils.h>
#include <miniz/zip_walk.h>
#include <mmapFile.h>
#include <transfer.h>
#include <threadpool.h>
#include <arpa/inet.h>
#include <gmssl/sm4_file.h>

//0xAF 0xAE -- 传送文件信息
//0xAF 0xAF -- 传送文件数据

#define     BUF_SIZE        1024
#define     FILENAME        "foo.ms4"
#define     PTHREAD_NUM     5

char server_addr[2048];
void *send_fileinfo(void *arg);
void *send_filedata(void *arg);
int socket_and_connect();

static int pthreads = PTHREAD_NUM; //线程数

int send_main(int argc, char const *argv[])
{
    //判断参数
    if (argc < 7)
    {
        perror("usage:client <IPaddress> <key> <iv> <cbc/ctr> file1 file2 file3...");
        exit(1);
    }
    strcpy(server_addr, argv[2]);

    //打包
    if (access("file", 0) == 0) {
        rm_dir("file");
    }
    mkdir("file",0777);
    int file_num = 6;
    while(file_num < argc) {
        char mv_target_name[2048] = "file/";
        strcat(mv_target_name, argv[file_num]);
        copy_by_block(argv[file_num],mv_target_name);
        file_num++;
    }
    struct zip_t *zip = zip_open("foo.zip", 5, 'w');
    zip_walk(zip, "file");
    zip_close(zip);

    if (access("file", 0) == 0) {
        rm_dir("file");
    }

    char keyhex[2048];
    strcpy(keyhex, argv[3]);
    char ivhex[2048];
    strcpy(ivhex, argv[4]);
    char act[10];
    strcpy(act, "encrypt");
    char u_mode[10];
    strcpy(u_mode, argv[5]);
    char in_data[2048];
    strcpy(in_data, "foo.zip");
    char out_data[2048];
    strcpy(out_data, "foo.ms4");
    sm4_main(keyhex, ivhex, act, u_mode, in_data, out_data);

    if( remove("foo.zip") == 0 )
        printf("Removed tmp zip file.");
    else
        perror("remove");

  pthread_t pthread_;
    pthread_create(&pthread_, NULL, send_fileinfo, NULL);
    pthread_join(pthread_, NULL);

    threadpool_t *pool = threadpool_create(5, 120);
    int arr[PTHREAD_NUM] = {0};
    int i = 0;
    for(i=0; i<PTHREAD_NUM; i++){
        arr[i] = i;
    }

    for(i=0; i<PTHREAD_NUM; i++){
        task_t task;
        task.data = (void*)&arr[i];
        task.func = send_filedata;
        tasks_add(pool, task);
    }

    threadpool_destory(pool);
    return 0;
}

void *send_fileinfo(void *arg)
{
    int ret = 0;
    int fd = socket_and_connect();
    uint8_t buf[BUF_SIZE] = {0};

    int fileSize = getFileSize(FILENAME);
    /*     transfer_.fileSize = fileSize;
    transfer_.filePieces = get_piece_num(fileSize); */

    int fileNameSize = strlen(FILENAME);
    //同步字
    buf[HEAD_POS_SYNC_WORD] = 0xAF;
    buf[HEAD_POS_SYNC_WORD + 1] = 0xAE;
    //所有数据大小==fileSize
    buf[HEAD_POS_TOTAL_SIZE] = fileSize >> 8;
    buf[HEAD_POS_TOTAL_SIZE + 1] = (fileSize & 0xff);
    // 文件名字节数
    buf[HEAD_POS_P_LENGTH] = fileNameSize >> 8;
    buf[HEAD_POS_P_LENGTH + 1] = (fileNameSize & 0xff);

    memcpy(buf + HAND_TOTAL_SIZE, FILENAME, fileNameSize);

    send(fd, buf, BUF_SIZE, 0);

    close(fd);
}

void *send_filedata(void *arg)
{
    int pthread_number = *(int *)arg; //线程编号--从0开始
    int ret = 0;
    int fd = socket_and_connect();
    uint8_t buf[HAND_TOTAL_SIZE + 1] = {0}; //12+1
    transfer_t transfer_;
    int fileSize = getFileSize(FILENAME);
    transfer_.fileSize = fileSize;
    transfer_.filePieces = get_piece_num(fileSize);
    char *begin_ = mmap_file(FILENAME, fileSize);

    int n = 0;
    int pieceNo = pthread_number; //某一线程起始编号为线程编号
    //对应本线程将要处理的编号 --> 总线程数 * n + 线程编号
    while (pieceNo < transfer_.filePieces)
    {
        //同步字
        buf[HEAD_POS_SYNC_WORD] = 0xAF;
        buf[HEAD_POS_SYNC_WORD + 1] = 0xAF; //0xAF 0xAF == 发送数据部分
        //所有数据大小==fileSize
        buf[HEAD_POS_TOTAL_SIZE] = transfer_.fileSize >> 8;
        buf[HEAD_POS_TOTAL_SIZE + 1] = (transfer_.fileSize & 0xff);
        // 所有分片的数量
        buf[HEAD_POS_TOTAL_PIECES] = transfer_.filePieces >> 8;
        buf[HEAD_POS_TOTAL_PIECES + 1] = (transfer_.filePieces & 0xff);
        // 分片编号，从0开始
        transfer_.pieceNo = pieceNo;
        buf[HEAD_POS_P_INDEX] = transfer_.pieceNo >> 8;
        buf[HEAD_POS_P_INDEX + 1] = (transfer_.pieceNo & 0xff);
        // 分片数据的大小
        transfer_.pieceSize = get_single_piece_size(transfer_.pieceNo, transfer_.filePieces, transfer_.fileSize);
        buf[HEAD_POS_P_LENGTH] = transfer_.pieceSize >> 8;
        buf[HEAD_POS_P_LENGTH + 1] = (transfer_.pieceSize & 0xff);
        //客户端线程数
        transfer_.clientPthreads = pthreads;
        buf[CLIENT_PTHREAD_SIZE] = transfer_.clientPthreads >> 8;
        buf[CLIENT_PTHREAD_SIZE + 1] = (transfer_.clientPthreads & 0xff);

        send(fd, buf, HAND_TOTAL_SIZE, 0);
        send(fd, begin_ + (transfer_.pieceNo * PIECE_DATA_SIZE), transfer_.pieceSize, 0);
        n++;
        pieceNo = pthreads * n + pthread_number;
    }
    close(fd);
}

int socket_and_connect()
{
    int fd, ret;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr(server_addr);

    ret = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret < 0)
        exit(-1);

    return fd;
}
