/*
* 9p2000 protocol implementation
*
* Data is represented in little-endian order
*
* size[4] Tversion tag[2] msize[4] version[s]
* size[4] Rversion tag[2] msize[4] version[s]
*
* Negotiates protocol version and maximum message size.
*
* Note: Implemented but practically unused
*
* - _version_ is always the string 9p2000.
*
* size[4] Tattach tag[2] fid[4] afid[4] uname[s] aname[s]
* size[4] Rattach tag[2] qid[16]
*
* Attach file tree _aname_ to _fid_.
* Response contains qid of _aname_.
*
* Note: Because the 9p2000 protocol is meant to be used on top of other protocols
* not currently implemented, contrary to the 9p protocol, _uname_ currently contains
* the mqueue channel to be associated with the _fid_.
*
* - _afid_ is currently unused and its value should be NOFID
*
* size[4] Tcreate tag[2] fid[4] name[s] perm[4] mode[1]
* size[4] Rcreate tag[2] qid[16] iounit[4]
*
* Create file with _name_ in the directory represented by _fid_
* with permissions _perm_. Then, open file with _mode_
*
* - _iounit_ is currently unused and its value should be 0
*
* size[4] Rerror tag[2] ename[s]
*
* Error message _ename_
*/

#ifndef _FCALL_H
#define _FCALL_H

#define NOFID (int)~0
#define NOTAG (unsigned short)~0

#define CHAR_SIZE 1
#define SHRT_SIZE 2
#define INT_SIZE  4
#define LONG_SIZE 8

/* File modes */
#define OREAD   1 << 0 /* Read only */
#define OWRITE  1 << 1 /* Write only */
#define ORDWR   1 << 2 /* Read and write */
#define OEXEC   1 << 3 /* Execute */
#define OTRUNC  1 << 4 /* Truncate */
#define ORCLOSE 1 << 5 /* Remove when fid is cluncked */

/* File permissions */
#define DMDIR 1 << 31 /* Directory */

typedef struct {
    unsigned int  type;
    unsigned int  version;
    unsigned long id;
} qid;

typedef struct {
    unsigned char  type; /* Message type */
    unsigned int   fid;  /* Associated file descriptor */
    unsigned short tag;  /* Identifying tag */
    union {
        struct { /* Tversion, Rversion */
            unsigned msize;    /* Maximum message size */
            char     *version; /* Protocol version */
        };
        struct { /* Tattach */
            unsigned afid;   /* Authentication fid. Currently unused */
            char     *uname; /* Associated user. Currently unused */
            char     *aname; /* File tree to access */
        };
        struct { /* Rattach, Rcreate */
            qid      qid;    /* File information */
            unsigned iounit; /* Maximum number of bytes guaranteed */
                             /* to be read or written. Currently unused */
        };
        struct { /* Tcreate */
            char*    name; /* File name to be created */
            unsigned perm; /* File permissions */
            char     mode; /* File open mode */
        };
        struct { /* Rerror */
            char* ename; /* Error string */
        };
    };
} fcall;

enum fcall_types {
    Tversion = 100,
    Rversion,
    Tattach,
    Rattach,
    Tcreate,
    Rcreate,
    Terror, /* Illegal */
    Rerror,
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
