#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <stdio.h>
#include <sys/vfs.h>

int main(int argc, const char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "No device given \n");
        return 1;
    }

    errno = 0;
    FILE *channel = fopen(argv[1], "r+");
    if (!channel) {
        fprintf(stderr, "Device %s was not found\n", argv[1]);
        return 1;
    }

    errno = 0;
    mqd_t mq_vfs = mq_open("vfs/server", O_WRONLY);
    if (mq_vfs == -1) {
        fprintf(stderr, "Failed to initialise connection to VFS\n");
        fflush(stderr);
        fclose(channel);
        return 1;
    }

    errno = 0;
    mqd_t mq_in = mq_open("vfs/export", O_RDONLY | O_CREAT | O_EXCL);
    if (mq_in == -1) {
        fprintf(stderr, "Failed to create message queue\n");
        fflush(stderr);
        fclose(channel);
        mq_close(mq_vfs);
        return 1;
    }

    puts("Initialising server...");

    size_t len = 0;
    char buf[VFS_MAX_MSG_LEN];
    while (1) {
        int c;
        while ((c = fgetc(channel)) != EOF) {
            buf[len++] = c;
        }

        if (len == 0) {
            /* TODO: Replace with libc functions */
            channel->offset = 0;
            channel->flags |= ~F_EOF;
            continue;
        }

        unsigned msg_size = (unsigned) *(buf + FCALL_SIZE_OFF);
        if (len < msg_size || msg_size == 0) {
            channel->offset = 0;
            channel->flags |= ~F_EOF;
            continue;
        }

        /* Replace mq_id */
        mqd_t old_mq = *((mqd_t *) (buf + msg_size));
        *(buf + msg_size) = mq_in;

        if (mq_send(mq_vfs, buf, len + sizeof(mqd_t), 0) == -1) {
            puts("Failed to forward message");
            continue;
        }

        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1) {
            puts("Failed to receive response");
            continue;
        }

        /* Restore mq_id */
        msg_size = (unsigned) *(buf + FCALL_SIZE_OFF);
        *(buf + msg_size) = old_mq;

        if (fwrite(buf, 1, msg_size + sizeof(mqd_t), channel) == 0) {
            fprintf(stderr, "Failed to return response\n");
            fflush(stderr);
            continue;
        }

        fflush(channel);

        len = 0;
    }

    fclose(channel);
    mq_close(mq_vfs);
    mq_close(mq_in);
    mq_unlink("vfs/export");
}
