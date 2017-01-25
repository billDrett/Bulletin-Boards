#include "boardClient.h"
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

BoardClient::BoardClient(std::string C2S, std::string S2C)
{
    if((pipeClient2Server = open(C2S.c_str(), O_WRONLY)) < 0)
    {
        perror("fifo C2S not found");
    }

    if((pipeServer2Client = open(S2C.c_str(), O_RDWR)) < 0) //isws kai O_NONBLOCK
    {
        perror("fifo S2C not found");
    }
}

BoardClient::~BoardClient()
{
    //KLEINEI TA PIPES PROS TON SERVER
}

int BoardClient::get_pipeC2S()
{
    return pipeClient2Server;
}

int BoardClient::get_pipeS2C()
{
    return pipeServer2Client;
}

void BoardClient::createChanel(int Id, std::string Name)
{
    int nwrite;
    char convert[24];
    string buff;

    if((nwrite = write(pipeClient2Server, "<Chanel>", 8))== -1) //stelnoume ta tags gia to ksekinima tou aitimatos
    {
        perror("Cant write to pipe C2S");
    }

    if((nwrite = write(pipeClient2Server, "<start>", 7))== -1)
    {
        perror("Cant write to pipe C2S");
    }

    sprintf(convert, "%d ", Id);
    buff.append(convert);
    buff.append(Name.c_str());
    if((nwrite = write(pipeClient2Server, buff.c_str(), buff.size()))== -1) //stelnw to id kai to onoma tou chanel
    {
        perror("Cant write to pipe C2S");
    }

    if((nwrite = write(pipeClient2Server, "<end>", 5))== -1)
    {
        perror("Cant write to pipe C2S");
    }

    if((nwrite = write(pipeClient2Server, "<Chanel>", 8))== -1) //telos tou aitimatos
    {
        perror("Cant write to pipe C2S");
    }
}

void BoardClient::getMessages(int Id)
{
    char convert[24];
    string buff;

    int nwrite;
    if((nwrite = write(pipeClient2Server, "<Message><start>", 16))== -1) //zhtaw apo to server ta messages kai arxeia
    {
        perror("Cant write to pipe C2S");
    }
    sprintf(convert, "%d", Id);
    buff = convert;
    if((nwrite = write(pipeClient2Server, buff.c_str(), buff.size()))== -1)
    {
        perror("Cant write to pipe C2S");
        cout<<nwrite<<endl;
    }

    if((nwrite = write(pipeClient2Server, "<end><Message>", 14))== -1) //telos aitimatos
    {
        perror("Cant write to pipe C2S");
    }

    this->readMessages(); //twra ta diavazw
}

void BoardClient::readMessages()
{
    char* tmp;
    char* tmp1;
    int fp = -1;
    int nread;
    char buffer [BUFFERSIZE];
    states state = waitingMessage;
    string  tmpTag;

    while((nread = read(pipeServer2Client, buffer + tmpTag.size(), BUFFERSIZE - tmpTag.size())) > 0) //diavazoume thn apanthsh tou server mexri na vroume to <end><File>
    {                                                                                                //an se proigoumeno read diavase meros tou tag to prosthetei sthn arxh tou buffer kai diavazei ligotera stoixeia
        strncpy(buffer, tmpTag.c_str(), tmpTag.size()); //prostetw sthn arxh tou buffer to proigoumeno tag efoson yparxei
        nread += tmpTag.size();
        tmpTag = "";
        tmp = buffer;

        if(findSubstring(tmp, "<NONE>", nread) != NULL)//ama lavw to NONE tote dn yparxei to chanel
        {
            cout<<"The chanel doesnt exists"<<endl;
            return;
        }

        if(state == waitingMessage) //prwta to message tag
        {
            tmp1 = getAfterTag(tmp, "<Message>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL) // mporei na pira meros tou tag, to apothikeuw kai to prosthetw sthn arxh tou neou read
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = MessageWaitStart;
        }

        if(state == MessageWaitStart) //meta perimeno to start tag
        {
            tmp1 = getAfterTag(tmp, "<start>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = MessageWaitEnd;
            cout<<"Message :"<<endl;
        }

        if(state == MessageWaitEnd)
        {
            int copyNread = nread;
            if(getBeforeTag(tmp, "<end>", copyNread) != NULL)
            {
                state = waitingEndMessage;
            }

            printf("%.*s", copyNread, tmp); //mporei to string na mhn teleiwnei me \0 gia auto xrhsimopoiw auth thn print
        }

        if(state == waitingEndMessage)
        {
            cout<<endl;
            tmp1 = getAfterTag(tmp, "<Message>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = waitingFile;
        }

        if(state == waitingFile) //teleiwsa me to Message perimenw ta files twra
        {
            tmp1 = getAfterTag(tmp, "<File>", nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            state = FileWaitName;
        }

        if(state < FileWaitName) continue; //dn thelw na mpw sto loop enw dn exw ftasei se ena apo ta parakatw stadia giati tote dn tha termatistei pote
        while(1) //dn kserw posa files tha diavasei gia auto tha stamathsei otan dei to <File>
        {
            if(state == FileWaitName)
            {
                string fileName;
                int lenght;

                tmp1 = getAfterTag(tmp, "<:", nread); //pairnw to onoma tou file
                lenght = nread;
                if((tmp1 == NULL) || getBeforeTag(tmp1, ">", lenght) == NULL)
                {
                    if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                    {
                        tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                    }
                    break;
                }
                tmp = tmp1;

                fileName.assign(tmp, lenght);
                if(fileName == " ")//keno file, to chanel dn eixe arxeia
                {
                    cout<<"No files to download"<<endl;
                    state = FileWaitStart;
                    continue;
                }
                close(fp);

                fileNameExists(fileName); //elenxo an yparxei hdh to onoma

                fp=open(fileName.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644); //anoigw to arxeio gia grapsimo
                if(fp == -1)
                {
                    perror("problem opening file");
                }

                state = FileWaitStart;
                cout<<fileName<<"is downloaded"<<endl;
            }

            if(state == FileWaitStart)
            {
                tmp1 = getAfterTag(tmp, "<start>", nread);
                if(tmp1 == NULL)
                {
                    if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                    {
                        tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                    }
                    break;
                }
                tmp = tmp1;
                state = FileWaitEnd;
            }

            if(state == FileWaitEnd)
            {
                int copyNread = nread;
                if(getBeforeTag(tmp, "<end>", copyNread) != NULL)
                {
                    state = waitingEndFile;
                }

                write(fp, tmp, copyNread);
                if(state == FileWaitEnd) break; //an exw kai alla na diavastoun pairnw nea dodomena apo to pipe
            }

            if(state == waitingEndFile)
            {
                if(findSubstring(tmp, "<:", nread) != NULL) //an yparxei akoma arxeio epistrofi pisw
                {
                    state = FileWaitName;
                    close(fp);
                    continue;
                }
                close(fp);
                tmp = getAfterTag(tmp, "<File>", nread);
                if(tmp1 == NULL)
                {
                    if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                    {
                        tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                    }
                    break;
                }
                return;
            }
        }
    }
}

void BoardClient::exitClient()
{
    close(pipeClient2Server);
    close(pipeServer2Client);
}

void BoardClient::shutdownServer()
{
    int nwrite;
    if((nwrite = write(pipeClient2Server, "<Shutdown>", 10))== -1) //stelnw to aitima gia kleisimo tou server
    {
        perror("Cant write to pipe C2S");
    }
}

int BoardClient::menu(string& line)
{
    string command, name;
    int id;
    istringstream iss(line);

    iss>> command;
    if(iss.fail())
    {
        cout<<"error command"<<endl;
        return 0;
    }
    if(command == "createchannel")
    {
        iss>> id>>name;
        if(!iss)
        {
            cout<<"error command"<<endl;
        }
        this->createChanel(id, name);
    }
    else if(command == "getmessages")
    {
        iss >>id;
        if(!iss)
        {
            cout<<"error command"<<endl;
        }
        this->getMessages(id);
    }
    else if(command == "exit")
    {
        cout<<"exit client"<<endl;
        this->exitClient();
        return -1;
    }
    else if(command == "shutdown")
    {
        cout<<"server shudown"<<endl;
        this->shutdownServer();
        return -1;
    }
    else //error
    {
        cout<<"error"<<endl;
    }
    return 0;
}
