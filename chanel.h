#ifndef Included_Chanel_H
#define Included_Chanel_H

#include <iostream>
#include <fstream>
#include "List.h"
#include "Node.h"
class Chanel
{
    private:
        const int id;
        std::string name;
        std::string messageFileName;
        List<std::string>* files;
    public:
        Chanel(int Id, std::string Name);
        ~Chanel();

        int get_id();
        std::string get_name();
        std::string get_messagesFile();
        Node<std::string>* get_filesList();

        void addMessage(std::string& message);
        void addFile(std::string* file);

        void removeAllMessages();
        void removeAllFiles();

};
#endif

