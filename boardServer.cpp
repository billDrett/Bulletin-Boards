#include "boardServer.h"
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

BoardServer::BoardServer(string C2S, string S2C, string BP2S, string S2BP)
{
    if((pipeClient2Server = open(C2S.c_str(), O_RDWR)) < 0) //isws kai O_NONBLOCK
    {
        perror("fifo C2S not found");
        exit(1);
    }

    if((pipeServer2Client = open(S2C.c_str(), O_WRONLY)) < 0)
    {
        perror("fifo S2C not found");
        exit(1);
    }

    if((pipeBoardPost2Server = open(BP2S.c_str(), O_RDWR)) < 0)
    {
        perror("fifo BP2S not found");
        exit(1);
    }

    /*if((pipeServer2BoardPost = open(S2BP.c_str(), O_WRONLY)) < 0)
    {
        perror("fifo S2BP not found");
    }*/
    pipeServer2BoardPost = -1; //tha to anoiksw otan xreiastei na kanw to write
    this->Server2BoardPost = S2BP.c_str(); //apothikeuw to onoma sto opoio vrisketai


    chanels = new List<Chanel>;
}

BoardServer::~BoardServer()
{
    Chanel* tmp;

    tmp = chanels->deleteFirstNode();
    while(tmp != NULL)
    {
        delete tmp;
        tmp = chanels->deleteFirstNode();//diagrafoume ta stoixeia tis listas
    }
    delete chanels;
    this->shutDown();
}

Node<Chanel>* BoardServer::get_chanelList()
{
    return chanels->get_begin();
}

int BoardServer::get_pipeC2S()
{
    return pipeClient2Server;
}

int BoardServer::get_pipeS2C()
{
    return pipeServer2Client;
}

int BoardServer::get_pipeBP2S()
{
    return pipeBoardPost2Server;
}

int BoardServer::get_pipeS2BP()
{
    return pipeServer2BoardPost;
}

void BoardServer::addChanel(Chanel* inChanel)
{
    if(searchChanel(inChanel->get_id()) != NULL)
    {
        delete inChanel;
        return;
    }

    chanels->insertEnd(inChanel);
}

bool BoardServer::addMessageToChanel(int Id, string message)
{
    Chanel* tmp = searchChanel(Id);
    if(tmp == NULL) return false; //dn yparxei chanel me auto to id

    tmp->addMessage(message);
    return true;
}

bool BoardServer::addFileToChanel(int Id, string* file)
{
    Chanel* tmp = searchChanel(Id);
    if(tmp == NULL) return false; //dn yparxei chanel me auto to id

    tmp->addFile(file);
    return true;
}

Chanel* BoardServer::searchChanel(int Id)
{
    for(Node<Chanel>* i = chanels->get_begin(); i != NULL; i = i->get_next()) //vriskei to chanel me auto to ID
    {
        if(i->get_data()->get_id() == Id)
        {
            return i->get_data();
        }
    }

    return NULL;
}

void BoardServer::sendChanelsList()
{
    char convert[24];
    string buff;
    int nwrite;
    int total = 0;

    if((nwrite = write(pipeServer2BoardPost, "<Chanel>", 8))== -1) //start tag gia chanel
    {
        perror("Cant write to pipe S2BP");
    }

    //prwta to start tag
    if((nwrite = write(pipeServer2BoardPost, "<start>", 7))== -1)
    {
        perror("Cant write to pipe S2BP");
    }
    total += nwrite;
    for(Node<Chanel>* i = chanels->get_begin(); i != NULL; i = i->get_next()) //stelnw ena ena ta chanel
    {
        buff = i->get_data()->get_name(); //prwta to onoma
        buff.append(" ");
        sprintf(convert, "%d", i->get_data()->get_id()); //meta to id
        buff.append(convert);
        buff.append("\n");

        if((nwrite = write(pipeServer2BoardPost, buff.c_str(), buff.size()))== -1)
        {
            perror("Cant write to pipe S2BP");
            cout<<nwrite<<endl;
        }
        total += nwrite;
    }

    //telos to end tag
    if((nwrite = write(pipeServer2BoardPost, "<end>", 5))== -1)
    {
        perror("Cant write to pipe S2BP");
    }
    total += nwrite;
    if((nwrite = write(pipeServer2BoardPost, "<Chanel>", 8))== -1)
    {
        perror("Cant write to pipe S2BP");
    }
    total += nwrite;
}

void BoardServer::sendMsgAndFiles(int Id)
{
    Chanel* tmp = searchChanel(Id);
    string buff;
    int nwrite;
    if(tmp == NULL) //an den ypaxei to kanali stelnw <NONE>
    {
        write(pipeServer2Client, "<NONE>", 6);
        return;
    }

    this->sendMesg(tmp); //stelnw message
    this->sendFiles(tmp); //stelnw arxeia
}

void BoardServer::sendMesg(Chanel* chanelId)
{
    char buff[BUFFERSIZE];
    int file;
    int nread, nwrite;

    file=open(chanelId->get_messagesFile().c_str(), O_RDONLY, 0644);

    if((nwrite = write(pipeServer2Client, "<Message><start>", 16))== -1) //arxh message
    {
        perror("Cant write to pipe S2C");
    }

    while((nread = read(file, buff, BUFFERSIZE)) > 0) //diavazw apo to arxeio kai stelnw to message
    {
        if(write(pipeServer2Client, buff, nread) == -1)
        {
            perror("Cant write to pipe S2C");
        }
    }

    if((nwrite = write(pipeServer2Client, "<end><Message>", 14))== -1) //telos message
    {
        perror("Cant write to pipe S2C");
    }
    close(file);
}

void BoardServer::sendFiles(Chanel* chanelId)
{
    char buff[BUFFERSIZE];
    int file;
    int nread, nwrite;

    if((nwrite = write(pipeServer2Client, "<File>", 6))== -1) //arxh files
    {
        perror("Cant write to pipe S2C");
    }

    for(Node<string>* i = chanelId->get_filesList(); i!= NULL; i = i->get_next()) //pairnw ola ta arxeia
    {
        string tmp ="<:";
        file=open(i->get_data()->c_str(), O_RDONLY, 0644);
        tmp.append(*(i->get_data()));
        tmp.append(">");
        if((nwrite = write(pipeServer2Client, tmp.c_str(), tmp.size()))== -1) //stelnw to onoma tou arxeiou mesa se <: >
        {
            perror("Cant write to pipe S2C");
        }

        if((nwrite = write(pipeServer2Client, "<start>", 7))== -1) //arxh dedomenwn arxeiou
        {
            perror("Cant write to pipe S2C");
        }

        while((nread = read(file, buff, BUFFERSIZE)) > 0) //grafw to arxeio
        {
            if(write(pipeServer2Client, buff, nread) == -1)
            {
                perror("Cant write to pipe S2C");
            }
        }
        close(file);

        if((nwrite = write(pipeServer2Client, "<end>", 5))== -1) //telos
        {
            perror("Cant write to pipe S2C");
        }
    }

    if(chanelId->get_filesList() == NULL) //SYMVASH ama dn yparxoun arxeia gia na steilei stelnei keno
    {
        string tmp = "<: ><start><end>";
        if((nwrite = write(pipeServer2Client, tmp.c_str(), tmp.size()))== -1)
        {
            perror("Cant write to pipe S2C");
        }
    }

    if((nwrite = write(pipeServer2Client, "<File>", 6))== -1)
    {
        perror("Cant write to pipe S2C");
    }

    chanelId->removeAllMessages(); //afairoume ta messages
    chanelId->removeAllFiles(); //kai ta arxeia apo ton server

}
void BoardServer::parser()
{
    fd_set set_read, set_copy;
    int maxfd = 0;

    FD_ZERO(&set_read);
    FD_SET(pipeClient2Server, &set_read);
    FD_SET(pipeBoardPost2Server, &set_read);

    if(pipeClient2Server > pipeBoardPost2Server) maxfd = pipeClient2Server;
    else maxfd = pipeBoardPost2Server;

    while(1)
    {
        set_copy = set_read;
        select(maxfd + 1, &set_copy, NULL, NULL, NULL);
        if(FD_ISSET(pipeClient2Server, &set_copy)) //otan einai etoimo diavazw apo to pipe client to server
        {
            if(this->readFromC2S() == false)
            {
                return;
            }
        }
        if(FD_ISSET(pipeBoardPost2Server, &set_copy)) //otan einai etoimo diavazw apo to pipe boardPost to server
        {
            this->readFromBp2S();
        }

    }

}

bool BoardServer::readFromC2S() //diavasma apo pipe client to server
{
    char buffer[BUFFERSIZE];
    char* tmp;
    int nread;
    string tmpTag;
    int state;
    void (BoardServer::*f)(char* buffer, int nread, int state, string& previousRead);

    while((nread = read(pipeClient2Server, buffer + tmpTag.size(), BUFFERSIZE - tmpTag.size())) > 0) //an exw lavei meros ena tag entolis tote ksanadiavazw gia na parw to ypoleipo
    {
        strncpy(buffer, tmpTag.c_str(), tmpTag.size());
        nread += tmpTag.size();
        tmp = buffer;
        tmpTag = "";
        while(1) //se ena read mporei na exw panw apo mia entoli gia auto mpainei se loop while 1 pou termatizetai otan dn yparxoun alloi xaraktires gia epeksergasia 'h parw meros tag
        {
            if(nread == 0) return true;
            strncpy(buffer, tmpTag.c_str(), tmpTag.size());
            tmpTag = "";
            state = this->nextState(tmp, nread, "<Chanel>", "<Message>", "<Shutdown>", tmpTag) -1; //ta states ksekinan apo 0 oxi apo 1

            if(state == -2) //exei kopei entoli sthn mesh ara na ksanadiavasw
            {
                if(tmpTag.size() == 0) return true;
                break;
            }
            else if(state==createChanel)
            {
                f = &BoardServer::parseAddChanel;
                this->parseCommandPrototype(tmp, nread, "<Chanel>", f, pipeClient2Server);
            }
            else if(state == getMessages)
            {
                f = &BoardServer::parseSendMessage;
                this->parseCommandPrototype(tmp, nread, "<Message>", f, pipeClient2Server);
            }
            else //shutdown
            {
                tmp = getAfterTag(tmp, "<Shutdown>", nread); //dn mporei na einai null afou me thn prohpothesh oti yparxei vrethike edw
                this->shutDown();
                return false;
            }
        }
    }
}

void BoardServer::readFromBp2S() //diavasma apo pipe boardPost to server
{
    char buffer[BUFFERSIZE];
    char* tmp;
    int nread;
    string tmpTag;
    int state;
    void (BoardServer::*f)(char* buffer, int nread, int state, string& previousRead);

    while((nread = read(pipeBoardPost2Server, buffer + tmpTag.size(), BUFFERSIZE - tmpTag.size())) > 0) //an exw lavei meros ena tag entolis tote ksanadiavazw gia na parw to ypoleipo
    {
        strncpy(buffer, tmpTag.c_str(), tmpTag.size());
        nread += tmpTag.size();
        tmp = buffer;
        tmpTag = "";
        while(1) //se ena read mporei na exw panw apo mia entoli gia auto mpainei se loop while 1 pou termatizetai otan dn yparxoun alloi xaraktires gia epeksergasia 'h parw meros tag
        {
            if(nread == 0)return;

            strncpy(buffer, tmpTag.c_str(), tmpTag.size());
            tmpTag = "";
            state = this->nextState(tmp, nread, "<Chanels>", "<Message>", "<File>", tmpTag) -1; //ta states ksekinan apo 0 oxi apo 1

            if(state == -2) //exei kopei entoli sthn mesh ara na ksanadiavasw
            {
                if(tmpTag.size() == 0) return;
                break;
            }
            else if(state==chanelist)
            {
                tmp = getAfterTag(tmp, "<Chanels>", nread);
                if(pipeServer2BoardPost < 0) //an dn to exw anoiskei hdh to anoigw
                {
                    if((pipeServer2BoardPost = open(Server2BoardPost.c_str(), O_WRONLY)) < 0)
                    {
                        perror("fifo S2BP not found");
                        exit(1);
                    }
                }
                this->sendChanelsList();

            }
            else if(state == message)
            {
                f = &BoardServer::parseGetMessage;
                this->parseCommandPrototype(tmp, nread, "<Message>", f, pipeBoardPost2Server);
            }
            else //file
            {
                f = &BoardServer::parseGetFiles;
                this->parseCommandPrototype(tmp, nread, "<File>", f, pipeBoardPost2Server);
            }
        }
    }
}

void BoardServer::parseAddChanel(char* buffer, int nread, int state, std::string& previousRead) //prosthetw to kanali sthn lista
{
    string  name;
    int id;
    Chanel* ch;

    previousRead.append(buffer, nread); //prosthetw dedomena mporei na xreiastei panw apo ena loop
    if(state == waitingEndCommand)
    {
        istringstream iss(previousRead);
        iss>>id >> name;
        if(iss.fail())
        {
            perror("error detail of chanel");
        }
        else
        {
            Chanel* ch = new Chanel(id, name);
            this->addChanel(ch);
        }
    }
}

void BoardServer::parseSendMessage(char* buffer, int nread, int state, std::string& previousRead) //diavazei apo poio kanali thelei na lavei ta messages o client
{
    int id;

    previousRead.append(buffer, nread);
    if(state == waitingEndCommand)
    {
        istringstream iss(previousRead);
        iss>>id;
        if(iss.fail())
        {
            perror("error detail of chanel");
        }
        else
        {
            this->sendMsgAndFiles(id); //stelnei ta messages tou chanel
        }
    }
}

void BoardServer::parseGetMessage(char* buffer, int nread, int state, std::string& previousRead)
{
    char* chanel1;
    int id, chanelSize1, chanelSize2;

    previousRead.append(buffer, nread);

    if(state == waitingEndCommand)
    {
        string chanelString;
        chanelSize1 = previousRead.size();
        chanel1 = getAfterTag(previousRead.c_str(), "*", chanelSize1); //anamesa sta * * vrisketo to chanel sto opoio tha apothikeutei to message

        chanelSize2 = chanelSize1;
        getBeforeTag(chanel1, "*", chanelSize1);

        chanelString.assign(chanel1, chanelSize1);
        id = atoi(chanelString.c_str());
        chanel1 = getAfterTag(chanel1, "*", chanelSize2);
        previousRead.assign(chanel1, chanelSize2);
        this->addMessageToChanel(id, previousRead);
    }

}

void BoardServer::parseGetFiles(char* buffer, int nread, int state, std::string& previousRead)
{
    string name, tmpName;
    char* tmp = buffer;
    int len, fd, id;

    if(previousRead.size() != 30) //ta prwta 30 bytes exoun to chanel id kai to onoma tou file
    {
        if(nread < (30 - previousRead.size())) //an dn arkoun tha diavasw ta ypoloipa se epomeno read
        {
            previousRead.append(buffer, nread);
            return;
        }
        len = 30 - previousRead.size(); // grafw osa apomenoun
        previousRead.append(buffer, len);
        tmp = &buffer[len];
        nread -= len;

        istringstream iss(previousRead); //ama yparxei hdh to arxeio ston server dn theloume na to xalasei
        iss>>id >>name;
        tmpName = name;
        fileNameExists(tmpName);
        if(name != tmpName) //ama ypaxei hdh to onoma antikatastash me to kainourgio
        {
            previousRead.erase(previousRead.find(name));
            previousRead.append(tmpName);
            for(int i = previousRead.size(); i < 30; i++)
            {
                previousRead.append(" ");
            }
        }
    }

    istringstream iss(previousRead);
    iss>>id >>name;
    if(iss.fail())
    {
        perror("cant get id and file name");
        return;
    }
    fd = open(name.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    write(fd, tmp, nread); //grafoume to arxeio
    close(fd);


    if(state == waitingEndCommand) //afou to pirame olo to vazoume kai sto chanel
    {
        string* stringName = new string (name);
        this->addFileToChanel(id, stringName);
    }


}

int BoardServer::nextState(char* buff, int nread, std::string str1, std::string str2, std::string str3, std::string& commandRest) //mporei na exw panw apo mia entolh sto idio read 'h na min exw kamia
{                                                                                                                                 // prepei na parw thn prwth pou tha sinantisw
    char* tmp1 = findSubstring(buff, str1.c_str(), nread);
    char* tmp2 = findSubstring(buff, str2.c_str(), nread);
    char* tmp3 = findSubstring(buff, str3.c_str(), nread);


    if(tmp1 == NULL)tmp1 = buff + nread; //vazw thn megaliteri thmi se periptwsh poun dn yparxei
    if(tmp2 == NULL)tmp2 = buff + nread;
    if(tmp3 == NULL)tmp3 = buff + nread;

    if(tmp1 > tmp2) //prepei na parw thn pio mikri timh
    {
        if(tmp2 > tmp3) return 3;
        else return 2;
    }
    else //tmp1 <= tmp2
    {
        if(tmp1> tmp3) return 3;
        else //tmp1 <= tmp2 and tmp1 <=tmp3
        {
            if(tmp1 == (buff + nread)) //mporei na einai ola isa tote dn yparxei kanena
            {
                if((tmp1 = getAfterTag(buff, "<", nread)) != NULL) //apothikeuw meros tou string
                {
                    commandRest.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }

                return -1;
            }
            return 1;
        }
    }
}
void BoardServer::parseCommandPrototype(char* buffer, int &nread, std::string CommandType, void (BoardServer::*foo)(char* buffer, int nread, int state, std::string& previousRead), int pipeToRead)
{//o buffer krataei ta perienxomena autou pou diavasame, to commandType einai to string pou perimenoume na diavasoume arxika kai sto telos, kai o pointer se function einai thn tha ekteleitai mexri na synanththei <end>
    string tmpTag , previousRead;
    char* tmp;
    char* tmp1;

    int stateCommand = waitingCommand;

    do
    {
        strncpy(buffer, tmpTag.c_str(), tmpTag.size());
        nread += tmpTag.size();
        tmpTag = "";
        tmp = buffer; //dn ta kanw if else if giati mporei na allaksoun state mesa se ena loop analoga me ta perienxomena tou buffer

        if(stateCommand == waitingCommand)
        {
            tmp1 = getAfterTag(tmp, CommandType.c_str(), nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            stateCommand = waitingStart;
        }

        if(stateCommand == waitingStart)
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
            stateCommand = waitingEnd;
        }

        if(stateCommand == waitingEnd)
        {

            int copyNread = nread;
            if(getBeforeTag(tmp, "<end>", copyNread) != NULL)
            {
                stateCommand = waitingEndCommand;
            }

            (this->*foo)(tmp, copyNread, stateCommand, previousRead); //kalw thn synarthsh pou mou dothike ws parametros
        }

        if(stateCommand == waitingEndCommand)
        {
            tmp1 = getAfterTag(tmp, CommandType.c_str(), nread);
            if(tmp1 == NULL)
            {
                if((tmp1 = getAfterTag(tmp, "<", nread)) != NULL)
                {
                    tmpTag.assign(tmp1-1, nread+1); // -1 giati thelw na krathsw kai to "< kai +1 sto read gia na to symperilavw
                }
                continue;
            }
            tmp = tmp1;
            shiftArray(buffer, tmp, nread);//strncpy(buffer, tmp, nread); //apothikeuw ta ypoloipa tou buffer sthn arxh tou gia na ta diaxeiristei to menu
            return;
        }
    }while(((nread = read(pipeToRead, buffer + tmpTag.size(), BUFFERSIZE - tmpTag.size())) > 0)); //an dn exoun diavastei ola osa thelw kanw ksana read apo to pipe pou mou dwthike
}

void BoardServer::shutDown() //kleinei ola ta pipes
{
    close(pipeBoardPost2Server);
    close(pipeClient2Server);
    close(pipeServer2BoardPost);
    close(pipeServer2Client);
}
