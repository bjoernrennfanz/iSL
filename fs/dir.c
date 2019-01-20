#include <string.h>

#include "kernel/calls.h"
#include "kernel/user-errno.h"
#include "kernel/fs.h"
#include "fs/fd.h"

struct linux_dirent64 {
    qword_t inode;
    qword_t offset;
    word_t reclen;
    byte_t type;
    char name[];
} __attribute__((packed));

int_t sys_getdents64(fd_t f, addr_t dirents, dword_t count) {
    STRACE("getdents64(%d, %#x, %#x)", f, dirents, count);
    struct fd *fd = f_get(f);
    if (fd == NULL)
        return _EBADF;
    if (fd->ops->readdir == NULL)
        return _ENOTDIR;

    dword_t orig_count = count;

    long ptr;
    int err;
    int printed = 0;
    while (true) {
        ptr = fd->ops->telldir(fd);
        struct dir_entry entry;
        err = fd->ops->readdir(fd, &entry);
        if (err < 0)
            return err;
        if (err == 0)
            break;

        dword_t reclen = offsetof(struct linux_dirent64, name) +
            strlen(entry.name) + 4; // name, null terminator, padding, file type
        char dirent_data[reclen];
        struct linux_dirent64 *dirent = (struct linux_dirent64 *) dirent_data;
        dirent->inode = entry.inode;
        dirent->offset = fd->ops->telldir(fd);
        dirent->reclen = reclen;
        dirent->type = 0;
        strcpy(dirent->name, entry.name);
        if (printed < 20) {
            STRACE(" {inode=%d, offset=%d, name=%s, type=%d, reclen=%d}",
                    dirent->inode, dirent->offset, dirent->name, dirent->type, dirent->reclen);
            printed++;
        }

        if (reclen > count)
            break;
        if (user_put(dirents, dirent_data))
            return _EFAULT;
        dirents += reclen;
        count -= reclen;
    }

    fd->ops->seekdir(fd, ptr);
    return orig_count - count;
}
