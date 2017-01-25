#ifndef Included_StringFuctions_H
#define Included_StringFuctions_H
#include <iostream>

char* findSubstring(const char* mainString, const char* substring, int mainLength);
char* getAfterTag(const char* mainString, const char* tag, int& length);
char* getBeforeTag(const char* mainString, const char* tag, int& length);
void shiftArray(char* buffer, char* beg, int bytes);
void fileNameExists(std::string& fileN);

#endif
