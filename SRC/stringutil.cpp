
#include "game.h"
#include "stringutil.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>  //vsnprintf
#include <stdarg.h> //va_list

char * String_Create(const char * stringWithFormat, ...) {
    char *str = NULL;
    char *fmt;

    // Make a copy of our formatted string to work with
    if (stringWithFormat != NULL) {
        fmt = strdup(stringWithFormat);
    } else {
        fmt = strdup("");
    }

    // Now apply the formatting on a trial run to determine how long the formatted string should be
    va_list argp;
    va_start(argp, stringWithFormat);
    char one_char[1];
    int len = vsnprintf(one_char, 1, fmt, argp);
    if (len < 1) {
        return NULL;
    }
    va_end(argp);

    // Allocate enough memory, and generate the formatted string for reals
    str = (char*)malloc(len + 1);
    if (!str) {
        return NULL;
    }
    va_start(argp, stringWithFormat);
    vsnprintf(str, len + 1, fmt, argp);
    va_end(argp);

    free(fmt);

    return str;
}

void String_Destroy(char *string) {
    free(string);
}

bool String_Equals(char *string1, char *string2) {
    return (strcmp(string1, string2) == 0);
}
