#ifndef _FCNTL_H
#define _FCNTL_H

#ifndef O_RDONLY
#define O_RDONLY 1 << 0
#endif /* O_RDONLY */

#ifndef O_WRONLY
#define O_WRONLY 1 << 1
#endif /* O_WRONLY */

#ifndef O_RDWR
#define O_RDWR 1 << 2
#endif /* O_RDWR */

#ifndef O_CREAT
#define O_CREAT 1 << 3
#endif /* O_CREAT */

#ifndef O_EXCL
#define O_EXCL 1 << 4
#endif /* O_EXCL */

#ifndef O_NONBLOCK
#define O_NONBLOCK 1 << 5
#endif /* O_NONBLOCK */

#ifndef O_TRUNC
#define O_TRUNC 1 << 6
#endif  /*O_TRUNC */

#ifndef O_APPEND
#define O_APPEND 1 << 7
#endif /* O_APPEND */

#ifndef O_DIRECTORY
#define O_DIRECTORY 1 << 8
#endif /* O_DIRECTORY */

#endif /* _FCNTL_H */
