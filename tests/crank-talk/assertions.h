#ifndef CRANK_TALK_TEST_ASSERTIONS_H
#define CRANK_TALK_TEST_ASSERTIONS_H

#include <stdlib.h>
#include <stdio.h>

#define Assert(expression) \
    do { \
        if(!(expression)) { \
            fprintf(stderr, "Assertion Failure: " __FILE__ ":%d: %s\n", __LINE__, #expression); \
            abort(); \
        } \
    } while (false)

#define AssertEquals(expected, value) \
    do { \
        if((expected) != (value)) { \
            fprintf(stderr, "Assertion Failure: " __FILE__ ":%d: No matching value\n", __LINE__); \
            abort(); \
        } \
    } while (false)

#endif //CRANK_TALK_TEST_ASSERTIONS_H