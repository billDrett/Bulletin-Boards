#include <string>
#include "boardPost.h"
#include "boardServer.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;
void catchinterrupt(int signo);

int main(int argc, char *argv[])
{
    string line, path, boardID, pipe1, pipe2;
    int found;
    BoardPost* bPost;

    if(argc != 2) //thelw ena mono argument
    {
        cout<<"error arguments"<<endl;
        return - 2;
    }

    signal(SIGINT, SIG_IGN); // agnow to control C
//while(1){}
    path = argv[1];
    found = path.find_last_of("/"); //thelw na parw to onoma tou current folder
    if(found ==  string::npos)
    {
        boardID = path;
    }
    else
    {
        boardID = path.substr(found+1);
    }
    pipe1 = path + "/" + boardID + "_othersS2BP";
    pipe2 = path + "/" + boardID + "_othersBP2S";
    bPost = new BoardPost(pipe2, pipe1);

    while(!getline(cin, line).eof()) //eisodos apo pliktrologio
    {
        //cout<<line<<endl;
        if(bPost->menu(line) == -1) break;
    }

    delete bPost;
}

void catchinterrupt(int signo)
{
    cout<<"i cought you"<<endl;
}
