#ifndef TOOLS_H
#define TOOLS_H
#include<iostream>
#include<string>
#include<sstream>
#include<cctype>
#include<stdexcept>
using namespace std;

namespace dsl
{
    class Object 
    {
        public:
        virtual string toString() const = 0;

        friend ostream& operator<<(ostream& os, const Object& obj)
        {
            return os << obj.toString();
        }
    };

    template <class T>
    class Node 
    {
        public:
        T data;
        Node<T>* prev;
        Node<T>* next;

        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

    template <class T>
    Node<T>* clone(Node<T>* obj)
    {
        if(obj == nullptr)
        {
            return nullptr;
        }

        Node<T>* copy, *tr, *tc;
        copy = new Node<T>(obj->data);

        tr = obj;
        tc = copy;

        while(tr->next != nullptr)
        {
            tc->next = new Node<T>(tr->next->data);
            tc->next->prev = tc;
            tr = tr->next;
            tc = tc->next;
        }
        return copy;
    }

    template <class T>
    void erase(Node<T>*& obj)
    {
        Node<T>* temp;

        while(obj != nullptr)
        {
            temp = obj;
            obj = obj->next;
            delete temp;
            temp = nullptr;
        }
    }

    class Task : public Object 
    {
        private:
        static size_t igen;
        string id;
        string title;

        string genId()  // generates a ID:
        {
            stringstream out;
            out << igen;    // got righ of setw(4), setfil('0) stops from 0000-9999 4-digit:

            igen += 1;
            return out.str();
        }

        public:
        Task() : id(genId()), title("unknown") {}       // d.constr. titel init. to unkonwn
        Task(string name) : id(genId()), title(name) {} // overloaded constr.

        Task(const Task& obj)   // Copy constructor
        {
            id = obj.id;
            title = obj.title;
        }

        Task& operator=(const Task& rhs)
        {
            if(this != &rhs)
            {
                id = rhs.id;
                title = rhs.title;
            }
            return *this;
        }

        ~Task() {}  // destructtor does nothing

        string identifier() const {return id;}  // getter
        void identifier(string value) {id = value;}   // setter

        string header() const {return title;}   // getter
        void header(string value)  {title = value;}    // setter
        // overriden toString method:
        string toString() const override 
        {
            // returns concatenated "id : title", so it formatys like: "1.blahblahblah":
            return id + ". " + title;
        }
    };

    size_t Task::igen = 1;
}
#endif
