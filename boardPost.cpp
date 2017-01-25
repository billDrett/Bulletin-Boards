#include "boardPost.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include "stringFuctions.h"

#define BUFFERSIZE 1024

using namespace std;

BoardPost::BoardPost(std::string BP2S, std::string S2BP)
{//anoigoume ta pipes
    if((pipeBoardPost2Server = open(BP2S.c_str(), O_WRONLY)) < 0)
    {
        perror("fifo BP2S not found");
    }

    if((pipeServer2BoardPost = open(S2BP.c_str(), O_RDWR)) < 0)
    {
        perror("fifo S2BP not found");
    }
}
BoardPost::~BoardPost()
{
    this->exitBoardPost();
}

int BoardPost::get_pipeBP2S()
{
    return pipeBoardPost2Server;
}

int BoardPost::get_pipeS2BP()
{
    return pipeServer2BoardPost;
}

void BoardPost::getChanels()
{
    int nwrite, nread;
    char buffer[BUFFERSIZE];
    char* tmp;
    char* tmp1;
    string tmpTag;
    statesChanels state = waitingChanel;

    if((nwrite = write(pipeBoardPost2Server, "<Chanels>", 9))== -1) //stelnoume mnm sto server na epistrepsei ta chanels tou
    {
        perror("Cant write to pipe S2BP");
    }

    while((nread = read(pipeServer2BoardPost, buffer + tmpTag.size(), BUFFERSIZE - tmpTag.size())) > 0) //diavazoume thn apanthsh tou server mexri na vroume to <end><Chanel>
    {                                                                                                   //se periptwsh pou diavastike meros tou proigoumenou tag diavazw ligotera bytes kai prostheto to proigoumeno tag sthn arxh
        strncpy(buffer, tmpTag.c_str(), tmpTag.size()); //antigrafw to meros tou proigoumenou tag efoson yparxei
        nread += tmpTag.size();
        tmpTag = ""; //midenizw to proigoumeno tag
        tmp = buffer;

        if(state == waitingChanel)//perimenw na vrw to <Chanel>
        {
            tmp1 = getAfterTag(tmp, "<Chanel>", nread);
            if(tmp1 == NULL) //dn vrethike to chanel tag
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL) // mporei na pira meros tou tag, to apothikeuw kai to prosthetw sthn arxh tou neou read
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = waitingStart;
        }

        if(state == waitingStart) //perimenw na vrw to <start>
        {
            tmp1 = getAfterTag(tmp, "<start>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL) // mporei na pira meros tou tag, to apothikeuw kai to prosthetw sthn arxh tou neou read
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = waitingEnd;
        }
        if(state == waitingEnd) //perimenw na vrw to <end>
        {
            int copyNread = nread;
            if(getBeforeTag(tmp, "<end>", copyNread) != NULL) //emfanizw ta chanels mexri na vrw to tag <end>, an to vrw emfanizw ta bytes mexri to <end>
            {
                state = waitingEndChanel;
            }

            write(1, tmp, copyNread); //grafw ta bytes mexri to copyNread
        }

        if(state == waitingEndChanel) //perimenw na vrw to <Chanel> meta to <end>
        {
            tmp1 = getBeforeTag(tmp, "<Chanel>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL) // mporei na pira meros tou tag, to apothikeuw kai to prosthetw sthn arxh tou neou read
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            break; //oloklirwthike to mnm
        }
    }
}

void BoardPost::sendMessage(string message, int Id)
{
    int nwrite;
    char convert[24];
    string name;
    if((nwrite = write(pipeBoardPost2Server, "<Message><start>", 16))== -1) //eisagwgika tag gia message
    {
        perror("Cant write to pipe BP2S");
    }

    sprintf(convert, "%d", Id); //metatrepw to Id se string
    name = "*";
    name.append(convert); //stelnw to chanel mesa se *ID*
    name.append("*");

    if((nwrite = write(pipeBoardPost2Server, name.c_str(), name.size()))== -1) //grafw to chanelID
    {
        perror("Cant write to pipe BP2S");
    }

    if((nwrite = write(pipeBoardPost2Server, message.c_str(), message.size()))== -1) // grafw to message
    {
        perror("Cant write to pipe BP2S");
    }

    if((nwrite = write(pipeBoardPost2Server, "<end><Message>", 14))== -1) //telos message tags
    {
        perror("Cant write to pipe BP2S");
    }
}

void BoardPost::sendFile(string file, int Id)
{
    int fp, found;
    int nread, nwrite;
    char buff[BUFFERSIZE];
    string name, tmpName;
    char convert[24];

    fp =open(file.c_str(), O_RDONLY, 0644);
    if(fp == -1) return;

    if((nwrite = write(pipeBoardPost2Server, "<File>", 6))== -1) //stelnoume ta tags gia arxh File
    {
        perror("Cant write to pipe BP2S");
    }
    if((nwrite = write(pipeBoardPost2Server, "<start>", 7))== -1)
    {
        perror("Cant write to pipe C2S");
    }
    sprintf(convert, "%d", Id);
    name.append(convert); //stelnw to chanel
    name.append(" ");
    found = file.find_last_of("/"); //prepei na parw to onoma, epeidh mporei na einai path pairnw to string meta to teleutaio "/"
    if(found ==  string::npos)
    {
        tmpName = file;
    }
    else
    {
        tmpName = file.substr(found+1);
    }
    name.append(tmpName);

    for(int i = name.size(); i <30; i++) //SYMVASH ta prwta 30 tha einai gia to chanelId kai to onoma tou arxeiou ara gemizw to ypoloipo me space
    {
        name.append(" ");
    }

    if((nwrite = write(pipeBoardPost2Server, name.c_str(), name.size()))== -1)
    {
        perror("Cant write to pipe BP2S");
    }

    while((nread = read(fp, buff, BUFFERSIZE)) > 0) //stelnw to arxeio
    {
        if(write(pipeBoardPost2Server, buff, nread) == -1)
        {
            perror("Cant write to pipe BP2S");
        }
    }

    if((nwrite = write(pipeBoardPost2Server, "<end>", 5))== -1)
    {
        perror("Cant write to pipe C2S");
    }

    if((nwrite = write(pipeBoardPost2Server, "<File>", 6))== -1)
    {
        perror("Cant write to pipe BP2S");
    }
}

void BoardPost::exitBoardPost()
{
    close(pipeBoardPost2Server);
    close(pipeBoardPost2Server);
}

int BoardPost::menu(string& line)
{
    string command, msg;
    int id;
    istringstream iss(line);

    iss>> command;
    if(iss.fail())
    {
        cout<<"error command"<<endl;
        cout<<line<<endl;
        return 0;
    }
    if(command == "list")
    {
        cout<<"Available chanels: "<<endl;
        this->getChanels();
    }
    else if(command == "write") //theloume na steiloume mnm me space
    {
        iss >>id >> msg ;
        if(!iss)
        {
            cout<<"error command"<<endl;
        }
        msg.insert(0, " "); //mporei to msg na einai substring tou command gia auto to logo vazoume to space etsi eksasfalizoume oti to find dn tha vrei kapoio substring tou command
        int pos = line.find(msg);
        line.erase(0, pos+1); //thelw na svisw to space pou prosthesa arxika gia auto to kanw pos+1

        this->sendMessage(line, id);
        cout<<"Message send"<<endl;
    }
    else if(command == "send")
    {
        iss >>id >> msg ;
        if(!iss)
        {
            cout<<"error command"<<endl;
        }
        this->sendFile(msg, id);
        cout<<"File send"<<endl;
    }
    else if(command == "exit")
    {
        cout<<"Exit boardPost"<<endl;
        this->exitBoardPost();
        return -1;
    }
    else //error
    {
        cout<<"error"<<endl;
    }
    return 0;
}

