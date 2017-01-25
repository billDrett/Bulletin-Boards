#ifndef Included_BoardServer_H
#define Included_BoardServer_H

#include <iostream>
#include "List.h"
#include "Node.h"
#include "chanel.h"
//void fileNameExists(string& fileN);
class BoardServer
{
    private:
        int pipeClient2Server;
        int pipeServer2Client;
        int pipeBoardPost2Server;
        int pipeServer2BoardPost;

        std::string Server2BoardPost;
        List<Chanel>* chanels;

        Chanel* searchChanel(int Id);
        void sendMesg(Chanel* chanelId);
        void sendFiles(Chanel* chanelId);
        bool readFromC2S();
        void readFromBp2S();
        void parseCommandPrototype(char* buffer, int &nread, std::string CommandType, void (BoardServer::*foo)(char* buffer, int nread, int state, std::string& previousRead), int pipeToRead);
        void parseAddChanel(char* buffer, int nread, int state, std::string& previousRead);
        void parseSendMessage(char* buffer, int nread, int state, std::string& previousRead);
        void parseGetMessage(char* buffer, int nread, int state, std::string& previousRead);
        void parseGetFiles(char* buffer, int nread, int state, std::string& previousRead);
        //bool findNameOfFile(char* buff, int nread, std::string& output);

        int nextState(char* buff, int nread, std::string str1, std::string str2, std::string str3, std::string& commandRest);
    public:
        BoardServer(std::string C2S, std::string S2C, std::string BP2S, std::string S2BP);
        ~BoardServer();

        Node<Chanel>* get_chanelList();
        int get_pipeC2S();
        int get_pipeS2C();
        int get_pipeBP2S();
        int get_pipeS2BP();

        void addChanel(Chanel* inChanel);
        bool addMessageToChanel(int Id, std::string message);
        bool addFileToChanel(int Id, std::string* file);

        void sendChanelsList();
        void sendMsgAndFiles(int Id);
        void shutDown();

        void parser();
};

enum statesC2S
{
    createChanel,
    getMessages,
    Shutdown,
};

enum statesBP2S
{
    chanelist,
    message,
    file,
};

enum statesCommand
{
    waitingCommand,
    waitingStart,
    waitingEnd,
    waitingEndCommand,
};
#endif

