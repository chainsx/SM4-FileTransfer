#include "gmssl/sm4_file.h"
int sm4_main(char *u_key, char *u_iv, char *act, char *u_mode, char *in_data, char *out_data)
{
    int ret;
    char *keyhex = NULL;
    char *ivhex = NULL;
    char *infile = NULL;
    char *outfile = NULL;
    uint8_t key[16];
    uint8_t iv[16];
    size_t keylen = sizeof(key);
    size_t ivlen = sizeof(iv);
    FILE *infp = stdin;
    FILE *outfp = stdout;
    int mode = 0;
    int enc = -1;
    SM4_CBC_CTX cbc_ctx;
    SM4_CTR_CTX ctr_ctx;
    uint8_t inbuf[4096];
    size_t inlen;
    uint8_t outbuf[4196];
    size_t outlen;

    keyhex = u_key;
    if (strlen(keyhex) != sizeof(key) * 2) {
        printf("invalid key length\n");
        goto end;
    }
    if (hex_to_bytes(keyhex, strlen(keyhex), key, &keylen) != 1) {
        printf("invalid HEX digits\n");
        goto end;
    }

    ivhex = u_iv;
    if (strlen(ivhex) != sizeof(iv) * 2) {
        printf("invalid IV length\n");
        goto end;
    }
    if (hex_to_bytes(ivhex, strlen(ivhex), iv, &ivlen) != 1) {
        printf("invalid HEX digits\n");
        goto end;
    }

    if (!strcmp(act, "encrypt")) {
        enc = 1;
    }
    if (!strcmp(act, "decrypt")) {
        enc = 0;
    }

    if (!strcmp(u_mode, "cbc")) {
        mode = SM4_MODE_CBC;
    }
    else if (!strcmp(u_mode, "ctr")) {
        mode = SM4_MODE_CTR;
    }

    infile = in_data;
    if (!(infp = fopen(infile, "r"))) {
        printf("open '%s' failure : %s\n", infile, strerror(errno));
        goto end;
    }

    outfile = out_data;
    if (!(outfp = fopen(outfile, "w"))) {
        printf("open '%s' failure : %s\n", outfile, strerror(errno));
        goto end;
    }


    if (mode == SM4_MODE_CTR) {
        if (sm4_ctr_encrypt_init(&ctr_ctx, key, iv) != 1) {
            printf("inner error\n");
            goto end;
        }
        while ((inlen = fread(inbuf, 1, sizeof(inbuf), infp)) > 0) {
            if (sm4_ctr_encrypt_update(&ctr_ctx, inbuf, inlen, outbuf, &outlen) != 1) {
                printf("inner error\n");
                goto end;
            }
            if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
                printf("output failure : %s\n", strerror(errno));
                goto end;
            }
        }
        if (sm4_ctr_encrypt_finish(&ctr_ctx, outbuf, &outlen) != 1) {
            printf("inner error\n");
            goto end;
        }
        if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
            printf("output failure : %s\n", strerror(errno));
            goto end;
        }

    }

    if (enc < 0) {
        printf("option -encrypt or -decrypt should be set\n");
        goto end;

    }

    if (enc) {
        if (sm4_cbc_encrypt_init(&cbc_ctx, key, iv) != 1) {
            printf("inner error\n");
            goto end;

        }
        while ((inlen = fread(inbuf, 1, sizeof(inbuf), infp)) > 0) {
            if (sm4_cbc_encrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1) {
                printf("inner error\n");
                goto end;

            }
            if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
                printf("output failure : %s\n", strerror(errno));
                goto end;

            }
        }
        if (sm4_cbc_encrypt_finish(&cbc_ctx, outbuf, &outlen) != 1) {
            printf("inner error\n");
            goto end;

        }
        if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
            printf("output failure : %s\n", strerror(errno));
            goto end;

        }

    } else {
        if (sm4_cbc_decrypt_init(&cbc_ctx, key, iv) != 1) {
            printf("inner error\n");
            goto end;

        }
        while ((inlen = fread(inbuf, 1, sizeof(inbuf), infp)) > 0) {
            if (sm4_cbc_decrypt_update(&cbc_ctx, inbuf, inlen, outbuf, &outlen) != 1) {
                printf("inner error\n");
                goto end;
            }
            if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
                printf("output failure : %s\n", strerror(errno));
                goto end;
            }
        }
        if (sm4_cbc_decrypt_finish(&cbc_ctx, outbuf, &outlen) != 1) {
            printf("inner error\n");
            goto end;
        }
        if (fwrite(outbuf, 1, outlen, outfp) != outlen) {
            printf("output failure : %s\n", strerror(errno));
            goto end;
        }
    }

    end:
    gmssl_secure_clear(&cbc_ctx, sizeof(cbc_ctx));
    gmssl_secure_clear(&ctr_ctx, sizeof(ctr_ctx));
    gmssl_secure_clear(key, sizeof(key));
    gmssl_secure_clear(iv, sizeof(iv));
    gmssl_secure_clear(inbuf, sizeof(inbuf));
    gmssl_secure_clear(outbuf, sizeof(outbuf));
    if (infile && infp) fclose(infp);
    if (outfile && outfp) fclose(outfp);
    return ret;
}
