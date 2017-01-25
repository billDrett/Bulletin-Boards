#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "boardClient.h"
#include "boardServer.h"
#include <unistd.h>
#include <iostream>
#include <signal.h>

using namespace std;

int main(int argc, char *argv[])
{
    pid_t serverPid = 1;
    string path, tmpPath, boardID;
    string line;
    int found;

    if(argc != 2)
    {
        cout<<"error arguments"<<endl;
        return - 2;
    }

    signal(SIGINT, SIG_IGN); //agnow to control c
    path = argv[1];
    found = path.find_last_of("/");
    if(found ==  string::npos)
    {
        boardID = path;
    }
    else
    {
        boardID = path.substr(found+1);
    }

    tmpPath = path + "/" + boardID + "_pid";

    if(open(tmpPath.c_str(), O_RDONLY, 0666) == -1)
    {
        //tmpPath = path + "/" +boardID;
        mkdir(path.c_str(), 0773);
        tmpPath = path + "/" +boardID + "_selfC2S";
        mkfifo(tmpPath.c_str(), 0666);
        tmpPath = path + "/" +boardID + "_selfS2C";
        mkfifo(tmpPath.c_str(), 0666);
        tmpPath = path + "/" +boardID + "_othersS2BP";
        mkfifo(tmpPath.c_str(), 0666);
        tmpPath = path + "/" +boardID + "_othersBP2S";
        mkfifo(tmpPath.c_str(), 0666);

        serverPid = fork();
        if(serverPid == -1)
        {
            perror("Failed to fork");
            exit(1);
        }
    }

    if(serverPid != 0) //boardClient process
    {
        cout<<"client proccess created"<<endl;
        string tmp1 = path + "/" + boardID + "_selfC2S", tmp2 = path + "/" + boardID + "_selfS2C";
        BoardClient* client = new BoardClient(tmp1, tmp2);
        while(!getline(cin, line).eof()) //eisodos apo pliktrologio
        {
            if(client->menu(line) == -1) break;
        }
        delete client;
        exit(0);
    }
    else //server process
    {
        char conver[32];
        chdir(path.c_str());
        tmpPath = boardID + "_pid";
        ofstream pidFile(tmpPath.c_str());

        pid_t currentPid = getpid();
        pidFile << currentPid;
        pidFile.close();
        cout<<"server process created"<<endl;

        string tmp1 = boardID + "_selfC2S", tmp2 = boardID + "_selfS2C", tmp3 = boardID + "_othersBP2S", tmp4 = boardID + "_othersS2BP";
        BoardServer* server = new BoardServer(tmp1, tmp2, tmp3, tmp4);
        server->parser();

        delete server;
        unlink(tmp1.c_str());
        unlink(tmp2.c_str());
        unlink(tmp3.c_str());
        unlink(tmp4.c_str());
        unlink(tmpPath.c_str());
        chdir("..");
        rmdir(path.c_str());
        exit(0);

    }
}
