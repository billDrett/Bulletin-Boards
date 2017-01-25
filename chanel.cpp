#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "chanel.h"
#include <fstream>
#include <string.h>
#include <string>
#include <unistd.h>


using namespace std;

Chanel::Chanel(int Id, string Name): id(Id)
{
    ofstream messageFile;
    char buffId[12];
    name = Name;
    files = new List<string>();

    sprintf(buffId, "%d", Id);
    messageFileName = "message_";
    messageFileName.append(buffId);
    messageFile.open(messageFileName.c_str()); //dimiourgoume to arxeio gia thn apothikeush twn mhnymatwn
    messageFile.close();
}

Chanel::~Chanel()
{
    this->removeAllMessages();
    this->removeAllFiles();
    delete files;
}

int Chanel::get_id()
{
    return id;
}

string Chanel::get_name()
{
    return name;
}

string Chanel::get_messagesFile()
{
    return messageFileName;
}

Node<string>* Chanel::get_filesList()
{
    return files->get_begin();
}

void Chanel::addFile(string* file)
{
    files->insertEnd(file);
}

void Chanel::addMessage(string& message) //apothikeuoume to onoma sto telos tou arxeiou
{
    ofstream messageFile;
    messageFile.open(messageFileName.c_str(), std::ofstream::out | std::ofstream::app);
    if(!messageFile)
    {
        perror("chanel file cant open");
        exit(3);
    }

    messageFile << message;
    messageFile.close();
}

void Chanel::removeAllMessages()
{
    remove(messageFileName.c_str());
}

void Chanel::removeAllFiles()
{
    string* tmp;

    tmp = files->deleteFirstNode();
    while(tmp != NULL)
    {
        unlink(tmp->c_str());
        delete tmp;
        tmp = files->deleteFirstNode();//diagrafoume ta stoixeia tis listas
    }
}
