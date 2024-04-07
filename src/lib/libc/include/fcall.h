/*
* 9p2000 protocol implementation
*
* Data is represented in little-endian order
*
* size[4] Tversion tag[2] msize[4] version[s] channel[s]
* size[4] Rversion tag[2] msize[4] version[s] channel[s]
*
* Negotiates protocol, version and channel of communication.
* The channel field is an extension for use in mqueues.
*
* - _version_ is always the string 9p2000
*/

#ifndef _FCALL_H
#define _FCALL_H

#define NOFID (int)~0
#define NOTAG (unsigned short)~0

#define CHAR_SIZE 1
#define SHRT_SIZE 2
#define INT_SIZE  4

typedef struct {
    unsigned char  type; /* Message type */
    unsigned int   fid;  /* Associated file descriptor */
    unsigned short tag;  /* Identifying tag */
    union {
        struct { /* Tversion, Rversion */
            unsigned msize;    /* Maximum message size */
            char     *version; /* Protocol version */
            char     *channel; /* Where to reply */
        };
    };
} fcall;

enum fcall_types {
    Tversion = 100,
    Rversion,
};

/*
* Calculate the number of bytes _fcall_ message size
*
* On failure, returns 0
*/
unsigned fcall_msg_size(const fcall *fcall);

/*
* Writes _fcall_ struct and to buffer pointed to by _buf_,
* with at most _length_ bytes
*
* On success, returns the length of the resulting buffer.
* On failure, returns 0
*/
unsigned fcall_msg_to_buf(const fcall *fcall, char *buf, unsigned length);

/*
* Creates _fcall_ struct from buffer pointed to by _buf_
*
* On success, returns the length the number of bytes the buffer occupied
* On failure, returns 0
*/
unsigned fcall_buf_to_msg(const char *buf, fcall *fcall);

#endif /* _FCALL_H */
