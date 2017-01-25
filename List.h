#ifndef Included_List_H
#define Included_List_H

#include "Node.h"

template <class T>
class List
{
    private:
        int listSize;
        Node<T>* start;
        Node<T>* last;

    public:
        List();
        ~List();

        bool checkEmpty();
        int getSize();
        Node<T>* get_begin();
        Node<T>* get_last();

        void insertEnd(T* account);
        void insertBeginning(T* account);
        T* deleteFirstNode();
        T* deleteLastNode();
        T* deleteNode(Node<T>* node);

        void printList();
};
#endif
