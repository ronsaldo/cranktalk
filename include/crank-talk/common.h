#ifndef CRANK_TALK_COMMON_H
#define CRANK_TALK_COMMON_H

#pragma once

#ifdef __cplusplus
#   define CRANK_TALK_EXTERN_C extern "C"
#   define CRANK_TALK_INLINE inline
#else
#   define CRANK_TALK_EXTERN_C
#   define CRANK_TALK_INLINE static inline
#endif

#define CRANK_TALK_EXPORT
#define CRANK_TALK_IMPORT

#define CRANK_TALK_API CRANK_TALK_EXTERN_C CRANK_TALK_EXPORT

#if defined(_WIN32)
#define CRANK_TALK_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__)
#define CRANK_TALK_THREAD_LOCAL __thread
#else
#error Add support for thread local storage.
#endif

#endif //CRANK_TALK_COMMON_H