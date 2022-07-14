//
// Created by chainsx on 22年7月12日.
//
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <gmssl/mem.h>
#include <gmssl/sm4.h>
#include <gmssl/hex.h>

#ifndef FILETRANSFER_SM4_FILE_H
#define FILETRANSFER_SM4_FILE_H

#define SM4_MODE_CBC 1
#define SM4_MODE_CTR 2

int sm4_main(char *u_key, char *u_iv, char *act, char *u_mode, char *in_data, char *out_data);

#endif //FILETRANSFER_SM4_FILE_H
