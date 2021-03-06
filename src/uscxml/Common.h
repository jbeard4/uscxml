#ifndef COMMON_H_YZ3CIYP
#define COMMON_H_YZ3CIYP

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#endif

#if defined(_MSC_VER)
// disable signed / unsigned comparison warnings
#pragma warning (disable : 4018)
// possible loss of data
#pragma warning (disable : 4244)
#pragma warning (disable : 4267)
// 'this' : used in base member initializer list (TypedSubscriber)
#pragma warning (disable : 4355)

// is thrown alot in arabica headers
#pragma warning (disable : 4240)
#pragma warning (disable : 4250)

#endif


#endif /* end of include guard: COMMON_H_YZ3CIYP */
