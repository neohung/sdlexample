#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_
char * String_Create(const char * stringWithFormat, ...);
void String_Destroy(char *string) ;
bool String_Equals(char *string1, char *string2);
#endif
