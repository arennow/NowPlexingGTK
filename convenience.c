#include "convenience.h"
#include <stdlib.h>
#include <string.h>

char* allocating_stringCopy(const char* src) {
	char* outBuffer = malloc(strlen(src)+1);
	strcpy(outBuffer, src);
	
	return outBuffer;
}

bool freeIfNonNull(void* ptr) {
	if (ptr) {
		free(ptr);
		
		return true;
	}
	
	return false;
}