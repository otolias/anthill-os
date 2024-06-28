#include "dirent.h"

void rewinddir(DIR *dirp) {
    dirp->r_pos = dirp->buf;
    dirp->r_end = dirp->buf;
    dirp->offset = 0;
    dirp->flags |= ~F_EOF;
}
