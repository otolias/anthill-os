#include "test_suite.h"

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <mqueue.h>

char section[] = "MQUEUE:ERROR";

void _test_mq_open(void) {
    /* --- Setup --- */

    int res;

    mq_attr attr = {
          .mq_maxmsg = 4,
          .mq_msgsize = 8,
    };

    /* -- Tests --- */

    errno = 0;
    mqd_t mq_1 = mq_open("test", O_RDWR | O_CREAT | O_EXCL, 0, attr);
    if (!mq_1 && errno != 0)
        printf("%s: Queue creation with attributes failed\n", section);

    errno = 0;
    mqd_t mq_2 = mq_open("test_2", O_RDWR | O_CREAT | O_EXCL, 0, 0);
    if (!mq_2 && errno != 0)
        printf("%s: Queue creation without attributes failed\n", section);

    errno = 0;
    res = mq_open("test", O_RDONLY);
    if (res != 1 && errno != 0)
        printf("%s: Queue open failed\n", section);

    errno = 0;
    res = mq_open("test", O_RDONLY | O_CREAT | O_EXCL, 0, attr);
    if (res != 1 && errno != EEXIST)
        printf("%s: Queue exclusivity failed\n", section);

    errno = 0;
    res = mq_open("test", 0);
    if (res != -1 && errno != EACCES)
        printf("%s: Queue flag validation failed\n", section);

    attr.mq_maxmsg = 9;
    errno = 0;
    res = mq_open("test_3", O_RDWR | O_CREAT, 0, attr);
    if (res != 1 && errno != ENOSPC)
        printf("%s: Queue flag maximum messages validation failed\n", section);

    attr.mq_maxmsg = 4;
    attr.mq_msgsize = 129;
    errno = 0;
    res = mq_open("test_3", O_RDWR | O_CREAT, 0, attr);
    if (res != 1 && errno != ENOSPC)
        printf("%s: Queue flag message size validation failed\n", section);

    /* --- Teardown --- */

    mq_close(mq_1);
    mq_unlink("test");
    mq_close(mq_2);
    mq_unlink("test_2");
}

void _test_mq_send(void) {
    /* --- Setup --- */

    int res;
    char message[] = "Message";
    char big_message[] = "Big Message";

    mq_attr attr = {
          .mq_maxmsg = 4,
          .mq_msgsize = 8,
    };

    mqd_t readable = mq_open("readable", O_RDONLY | O_CREAT, 0, attr);
    mqd_t writeable = mq_open("writeable", O_RDWR | O_CREAT, 0, attr);

    /* --- Test --- */

    errno = 0;
    res = mq_send(writeable, message, 8, 0);
    if (res != 0 && errno != 0)
        printf("%s: Queue send message failed\n", section);

    errno = 0;
    res = mq_send(readable, message, 8, 0);
    if (res != -1 && errno != EACCES)
        printf("%s: Queue send message permission validation failed\n", section);

    errno = 0;
    res = mq_send(-1, message, 8, 0);
    if (res != -1 && errno != EBADF)
        printf("%s: Queue send message descriptor validation failed\n", section);

    errno = 0;
    res = mq_send(writeable, big_message, 12, 0);
    if (res != -1 && errno != EMSGSIZE)
        printf("%s: Queue send message length validation failed\n", section);

    for (size_t i = 0; i < 3; i++) {
        mq_send(writeable, message, 8, 0);
    }

    errno = 0;
    res = mq_send(writeable, message, 8, 0);
    if (res != -1 && errno != EAGAIN)
        printf("%s: Queue send message max validation failed\n", section);

    /* --- Teardown --- */

    mq_close(readable);
    mq_unlink("readable");
    mq_close(writeable);
    mq_unlink("writeable");
}

void _test_mq_receive(void) {
    /* --- Setup --- */

    int res;
    char buffer[8];

    mq_attr attr = {
        .mq_maxmsg = 4,
        .mq_msgsize = 8,
    };

    mqd_t readable = mq_open("readable", O_RDONLY | O_CREAT, 0, attr);
    mqd_t writeable = mq_open("writeable", O_RDWR | O_CREAT, 0, attr);

    mq_send(writeable, "Message", 8, 0);

    /* --- Test --- */

    errno = 0;
    res = mq_receive(writeable, buffer, 8, 0);
    if (res != 0 && errno != 0)
        printf("%s: Queue receive message failed\n", section);

    errno = 0;
    res = mq_receive(readable, buffer, 8, 0);
    if (res != -1 && errno != EACCES)
        printf("%s: Queue receive message permission validation failed\n", section);

    errno = 0;
    res = mq_receive(-2, buffer, 8, 0);
    if (res != -1 && errno != EBADF)
        printf("%s: Queue receive message descriptor validation failed\n", section);

    errno = 0;
    res = mq_receive(writeable, buffer, 2, 0);
    if (res != -1 && errno != EMSGSIZE)
        printf("%s: Queue receive message length validation failed\n", section);

    errno = 0;
    res = mq_receive(writeable, buffer, 8, 0);
    if (res != -1 && errno != EAGAIN)
        printf("%s: Queue receive message empty validation failed\n", section);

    /* --- Teardown --- */

    mq_close(readable);
    mq_unlink("readable");
    mq_close(writeable);
    mq_unlink("writeable");
}

void _test_mq_close(void) {
    /* --- Setup --- */

    int res;

    mq_attr attr = {
          .mq_maxmsg = 4,
          .mq_msgsize = 8,
    };

    mqd_t mq = mq_open("mq", O_RDWR | O_CREAT, 0, attr);

    /* --- Tests --- */

    errno = 0;
    res = mq_close(-2);
    if (res != -1 && errno != EBADF)
        printf("%s: Queue close validation failed\n", section);

    errno = 0;
    res = mq_close(mq);
    if (res != 0 && errno != 0)
        printf("%s: Queue close failed\n", section);

    /* --- Teardown --- */

    mq_unlink("mq");
}

void _test_mq_unlink(void) {
    /* --- Setup --- */

    int res;

    mq_attr attr = {
          .mq_maxmsg = 4,
          .mq_msgsize = 8,
    };

    mqd_t pending = mq_open("pending", O_RDWR | O_CREAT, 0, attr);
    mqd_t no_pending = mq_open("no_pending", O_RDWR | O_CREAT, 0, attr);
    mq_close(no_pending);

    /* --- Tests --- */

    errno = 0;
    res = mq_unlink("non_existent");
    if (res != -1 && errno != ENOENT)
        printf("%s: Queue unlink validation failed\n", section);

    errno = 0;
    res = mq_unlink("no_pending");
    if (res != 0 && errno != 0)
        printf("%s: Queue unlink without pending failed\n", section);

    errno = 0;
    res = mq_unlink("pending");
    if (res != 0 && errno != 0)
        printf("%s: Queue unlink with pending failed\n", section);

    res = mq_close(pending);
    if (res != 0 && errno != 0)
        printf("%s: Queue close with pending failed\n", section);

    res = mq_close(pending);
    if (res != -1 && errno != EBADF)
        printf("%s: Queue non-blocking unlink failed\n", section);
}

void test_mqueue(void) {
    _test_mq_open();
    _test_mq_send();
    _test_mq_receive();
    _test_mq_close();
    _test_mq_unlink();
}
