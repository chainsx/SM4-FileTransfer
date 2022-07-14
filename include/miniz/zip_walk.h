#include <miniz/zip.h>
#include <stdio.h>
#include <assert.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#ifndef FILETRANSFER_ZIP_WALK_H
#define FILETRANSFER_ZIP_WALK_H
int is_dir(const char *path) {
    struct stat s;
    stat(path, &s);
    return S_ISDIR(s.st_mode);
}

void zip_walk(struct zip_t *zip, const char *path) {
    DIR *dir;
    struct dirent *entry;
    char fullpath[MAX_PATH];

    memset(fullpath, 0, MAX_PATH);
    dir = opendir(path);
    assert(dir);

    while ((entry = readdir(dir))) {
        // skip "." and ".."
        if (!strcmp(entry->d_name, ".\0") || !strcmp(entry->d_name, "..\0"))
            continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (is_dir(fullpath))
            zip_walk(zip, fullpath);
        else {
            zip_entry_open(zip, fullpath);
            zip_entry_fwrite(zip, fullpath);
            zip_entry_close(zip);
        }
    }
    closedir(dir);
}
#endif //FILETRANSFER_ZIP_WALK_H
