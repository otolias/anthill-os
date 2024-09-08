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
* Change current directory.
*
* Usage: cd _path_
*
* _path_ must be absolute
*/
enum sh_error cmd_cd(int argc, char *argv[]);

/*
* Import _channel_ on _path_
*
* Usage: importfs _channel_ _path_
*
* _path_ must be absolute.
*/
enum sh_error cmd_importfs(int argc, char *argv[]);

/*
* List current directory entries
*
* Usage: ls
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
