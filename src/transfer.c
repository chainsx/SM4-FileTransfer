#include "transfer.h"

/* transfer_t* piece_init()
{
    transfer_t* piece_ = (transfer_t*)malloc(sizeof(transfer_t));
    if(NULL == piece_)  return NULL;
    memset(piece_, 0, sizeof(transfer_t));

    return piece_;
}

void piece_destory(transfer_t* piece_)
{
    if(NULL != piece_)  free(piece_);
    return NULL;
} */

int get_piece_num(int fileSize)
{
    return ((fileSize % PIECE_DATA_SIZE == 0) ? (fileSize/PIECE_DATA_SIZE):(fileSize/PIECE_DATA_SIZE+1));
}

int get_single_piece_size(int pieceNo, int filePieces, int fileSize)
{
    return ((pieceNo + 1 == filePieces) ? (fileSize%PIECE_DATA_SIZE) : PIECE_DATA_SIZE);
}
