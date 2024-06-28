#ifndef _BIN_SH_H
#define _BIN_SH_H

enum sh_error {
    SH_OK,
    SH_NOMEM,
    SH_INVALID,
    SH_NOTFOUND,
    SH_CONT,
};

/*
* Change current directory to _dir_.
*
* _dir_ must be an absolute path
*/
enum sh_error cmd_cd(char *dir);

/*
* List current directory entries
*/
enum sh_error cmd_ls(void);

/*
* Deinitialise root directory stream
*/
void sh_deinit(void);

/*
* Initialise root directory stream
*/
enum sh_error sh_init(void);

#endif /* _BIN_SH_H */
