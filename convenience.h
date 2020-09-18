#ifndef convenience_h
#define convenience_h

#include <stdbool.h>

char* allocating_stringCopy(const char* src);
bool freeIfNonNull(void* ptr);

#endif