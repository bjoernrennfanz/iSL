#ifndef MINGWCOMPAT_H
#define MINGWCOMPAT_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include <inttypes.h>
#include <stdint.h>

#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <errno.h>
#include <winerror.h>

typedef unsigned short iSL_gid_t;
typedef unsigned short iSL_uid_t;

#undef gid_t
#define gid_t iSL_gid_t

#undef uid_t
#define uid_t iSL_uid_t

/* If MinGW doesn't (currently) define an error status we need, but winsock
 * does, then default to using the winsock status. They will not conflict.
 */
#ifndef EREMOTE
#   define EREMOTE          (WSAEREMOTE-WSABASEERR)
#endif

#ifndef EUSERS
#   define EUSERS           (WSAEUSERS-WSABASEERR)
#endif

#ifndef ESOCKTNOSUPPORT
#   define ESOCKTNOSUPPORT  (WSAESOCKTNOSUPPORT-WSABASEERR)
#endif

#ifndef EPFNOSUPPORT
#   define EPFNOSUPPORT     (WSAEPFNOSUPPORT-WSABASEERR)
#endif

#ifndef ESHUTDOWN
#   define ESHUTDOWN        (WSAESHUTDOWN-WSABASEERR)
#endif

#ifndef ETOOMANYREFS
#   define ETOOMANYREFS     (WSAETOOMANYREFS-WSABASEERR)
#endif

#ifndef EHOSTDOWN
#   define EHOSTDOWN        (WSAEHOSTDOWN-WSABASEERR)
#endif

#ifndef EDQUOT
#   define EDQUOT           (WSAEDQUOT-WSABASEERR)
#endif

#ifndef ESTALE
#   define ESTALE           (WSAESTALE-WSABASEERR)
#endif

#ifndef EMULTIHOP
#   define ENOTBLK          2015
#endif

#ifndef EMULTIHOP
#   define EMULTIHOP        2004
#endif

/* ------------------------------------------------- */
/*                      Socket                       */
/* ------------------------------------------------- */
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT        0x1000000
#endif

#ifndef MSG_EOR
#define MSG_EOR             0
#endif

#ifndef MSG_CTRUNC
#define MSG_CTRUNC          0
#endif

#ifndef MSG_TRUNC
#define MSG_TRUNC           MSG_PARTIAL
#endif

#ifndef PF_LOCAL
#define PF_LOCAL            AF_UNIX
#endif

// Definition of iovec structure
struct iovec
{
    void  *iov_base;
    size_t iov_len;
};
#define IOV_MAX 255

// Definition of msghdr structure
struct msghdr
{
    void *msg_name;
    size_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    int msg_flags;
};

#ifndef HAVE_RECVMSG
struct msghdr;
ssize_t iSL_recvmsg(int, struct msghdr *, int);
#define recvmsg iSL_recvmsg
#endif

// POSIX 1003.1g - ancillary data object information
// Ancillary data consits of a sequence of pairs of
// (cmsghdr, cmsg_data[])
//
struct cmsghdr {
    size_t cmsg_len;
    int cmsg_level;
    int	cmsg_type;
};

#define __CMSG_NXTHDR(ctl, len, cmsg) __iSL_cmsg_nxthdr((ctl),(len),(cmsg))
#define CMSG_NXTHDR(mhdr, cmsg) iSL_cmsg_nxthdr((mhdr), (cmsg))

#define CMSG_ALIGN(len) (((len)+sizeof(long)-1) & ~(sizeof(long)-1))

#undef CMSG_DATA
#define CMSG_DATA(cmsg)	((void *)((char *)(cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr))))
#define CMSG_SPACE(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(len))
#define CMSG_LEN(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))

#define __CMSG_FIRSTHDR(ctl,len) ((len) >= sizeof(struct cmsghdr) ? \
    (struct cmsghdr *)(ctl) : \
    (struct cmsghdr *)NULL)

#define CMSG_FIRSTHDR(msg)	__CMSG_FIRSTHDR((msg)->msg_control, (msg)->msg_controllen)
#define CMSG_OK(mhdr, cmsg) ((cmsg)->cmsg_len >= sizeof(struct cmsghdr) && \
    (cmsg)->cmsg_len <= (unsigned long) \
    ((mhdr)->msg_controllen - \
    ((char *)(cmsg) - (char *)(mhdr)->msg_control)))

static inline struct cmsghdr * __iSL_cmsg_nxthdr(void *__ctl, size_t __size, struct cmsghdr *__cmsg)
{
    struct cmsghdr * __ptr;

    __ptr = (struct cmsghdr*)(((unsigned char *) __cmsg) +  CMSG_ALIGN(__cmsg->cmsg_len));
    if ((unsigned long)((char*)(__ptr+1) - (char *) __ctl) > __size)
    {
        return (struct cmsghdr *)0;
    }

    return __ptr;
}

static inline struct cmsghdr * iSL_cmsg_nxthdr(struct msghdr *__msg, struct cmsghdr *__cmsg)
{
    return __iSL_cmsg_nxthdr(__msg->msg_control, __msg->msg_controllen, __cmsg);
}

/* ------------------------------------------------- */
/*                   Posix.FileSys                   */
/* ------------------------------------------------- */

// Do not exist in a windows filesystem
#ifndef S_IFLNK
#define S_IFLNK 0
#endif

#ifndef S_IFSOCK
#define S_IFSOCK 0
#endif

#ifndef S_ISVTX
#define S_ISVTX 0
#endif

#ifndef O_NOCTTY
#define O_NOCTTY 0x8000
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK 0x4000
#endif

// Use m to silence unused warnings
#undef S_ISLNK
#undef S_ISSOCK
#define S_ISLNK(m) (m?FALSE:FALSE)
#define S_ISSOCK(m) (m?FALSE:FALSE)

#ifndef O_ACCMODE
#define O_ACCMODE O_RDONLY|O_WRONLY|O_RDWR
#endif




#endif // MINGWCOMPAT_H
