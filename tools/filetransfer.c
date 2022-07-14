#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int recv_main(int argc, char **argv);
extern int send_main(int argc, char **argv);

static const char *options =
        "command [options]\n"
        "\n"
        "Commands:\n"
        "  help            Print this help message\n"
        "  send            Send Files\n"
        "  recv            Receive Files\n";

int main(int argc, char **argv)
{
    int ret = 1;
    char *prog = argv[0];

    argc--;
    argv++;

    if (argc < 1) {
        printf("Usage: %s %s\n", prog, options);
        return 1;
    }

    while (argc > 0) {
        if (!strcmp(*argv, "help")) {
            printf("usage: %s %s\n", prog, options);
            return 0;
        } else if (!strcmp(*argv, "recv")) {
            return recv_main(argc, argv);
        } else if (!strcmp(*argv, "send")) {
            return send_main(argc, argv);

        } else {
            fprintf(stderr, "%s: illegal option '%s'\n", prog, *argv);
            fprintf(stderr, "usage: %s %s\n", prog, options);
            return 1;
        }

        argc--;
        argv++;
    }

    return ret;
}