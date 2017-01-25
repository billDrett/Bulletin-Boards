#include "stringFuctions.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

char* findSubstring(const char* mainString, const char* substring, int mainLength) //epistrefei thn thesh pou vrisketai to subString sto mainString
{
    unsigned int j=0;
    for(int i =0; i < mainLength; i++)
    {
        if(mainString[i]==substring[j])
        {
            j++;
        }
        else
        {
            j = 0;
        }
        if(j == strlen(substring))
        {
            return ((char*)mainString+i-j+1); //to i exei timi thn thesh tou teleutaiou xarakthra tou substring, gia auto afairw to j pou einai to mhkos tou substring kai prosthetw 1 gia na parw thn arxh sto mainString
        }
    }
    return NULL;
}

char* getAfterTag(const char* mainString, const char* tag, int& length) //epistrefei thn thesh meta to substring, allazei to length gia na ypologisei ta enapominanta bytes
{
    char* start = findSubstring(mainString, tag, length);
    if(start == NULL) return NULL;

    length -= strlen(tag) + (start - mainString); //afairoume ta bytes pou prosperasame
    start+=strlen(tag); //thesh meta to substring
    return start;
}

char* getBeforeTag(const char* mainString, const char* tag, int& length) //allazei to length gia na ypologisei ta enapominanta bytes
{
    char* endTag = findSubstring(mainString, tag, length);
    if(endTag == NULL) return NULL;

    length =endTag-mainString;
    return (char*)mainString;
}

void fileNameExists(string& fileN) //an yparxei to arxeio sto current directory tou prosthetei sto telos to _copy.
{
    int fp;
    while((fp=open(fileN.c_str(), O_RDONLY, 0644)) != -1)
    {
        int found = fileN.find(".");
        if(found ==string::npos) //an dn yparxei h teleia mpainei sto telos
        {
            found = fileN.size();
        }
        fileN.insert(found, "_copy"); //to _copy prepei na mpei prin thn teleia
        close(fp);
    }
}

void shiftArray(char* buffer, char* beg, int bytes)
{
    for(int i = 0; i < bytes; i++)
    {
        buffer[i] = beg[i];
    }
}
