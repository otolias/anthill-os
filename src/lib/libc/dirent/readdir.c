#include "dirent.h"

#include <errno.h>
#include <fcall.h>
#include <pstring.h>
#include <stddef.h>

#include "internal/stdio.h"

static unsigned char dirent_buffer[BUFSIZ];

struct dirent *readdir(DIR *dirp) {
    if (!(dirp->flags & F_OPEN) || !(dirp->flags & F_DIR)) {
        errno = EBADF;
        return NULL;
    }

    if (dirp->r_end == dirp->r_pos) {
        if (!file_read(dirp, BUFSIZ)) {
            dirp->flags |= F_EOF;
            return NULL;
        }
    }

    struct dirent *dirent = (struct dirent *) dirent_buffer;
    struct fcall_stat *stat = (struct fcall_stat *) dirp->r_pos;
    size_t read = 0;

    dirent->d_ino = stat->qid.id;
    read += sizeof(struct qid);

    dirent->d_len = stat->length;
    read += sizeof(dirent->d_len);

    pstring *entry_name = (pstring *) stat->buffer;
    if (pstrtoz(dirent->d_name, entry_name, BUFSIZ - sizeof(ino_t)) == NULL) {
        errno = EOVERFLOW;
        return NULL;
    }
    read += pstrlen(entry_name);

    dirp->offset++;
    dirp->r_pos += read;

    return dirent;
}
