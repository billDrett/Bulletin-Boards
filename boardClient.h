#ifndef Included_BoardClient_H
#define Included_BoardClient_H

#include <iostream>

class BoardClient
{
    private:
        int pipeClient2Server;
        int pipeServer2Client;

        void readMessages();
        int readMessages(char* buffer, int buffSize); //epistrefei to current position tou buffer
        void readFiles(char* buffer, int buffSize, int startPosition);
    public:
        BoardClient(std::string C2S, std::string S2C);
        ~BoardClient();

        int get_pipeC2S();
        int get_pipeS2C();

        void createChanel(int Id, std::string Name);
        void getMessages(int Id);
        void receiveMessages();
        void exitClient();
        void shutdownServer();

        int menu(std::string& line);
};

enum states
{
    waitingMessage,
    MessageWaitStart,
    MessageWaitEnd,
    waitingEndMessage,
    waitingFile,
    FileWaitName,
    FileWaitStart,
    FileWaitEnd,
    waitingEndFile,
};

#endif
