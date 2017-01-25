#ifndef Included_BoardPost_H
#define Included_BoardPost_H

#include <iostream>

class BoardPost
{
    private:
        int pipeBoardPost2Server;
        int pipeServer2BoardPost;

    public:
        BoardPost(std::string BP2S, std::string S2BP);
        ~BoardPost();

        int get_pipeBP2S();
        int get_pipeS2BP();

        void getChanels();
        void sendMessage(std::string message, int Id);
        void sendFile(std::string file, int Id);
        void exitBoardPost();

        int menu(std::string& line);

        enum statesChanels
        {
            waitingChanel,
            waitingStart,
            waitingEnd,
            waitingEndChanel,
        };
};


#endif
