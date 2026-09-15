#ifndef CRANK_TALK_IO_H
#define CRANK_TALK_IO_H

#include "object-model.h"

typedef struct crank_talk_Stdio_s crank_talk_Stdio_t;
typedef struct crank_talk_BinaryFileStream_s crank_talk_BinaryFileStream_t;

struct crank_talk_Stdio_s
{
    crank_talk_Object_t super;
};

struct crank_talk_BinaryFileStream_s
{
    crank_talk_Object_t super;
    crank_talk_oop_t handle;
};

#endif // CRANK_TALK_IO_H