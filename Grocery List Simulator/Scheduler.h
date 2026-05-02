#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<iostream>
#include<string>
#include<sstream>
#include<cctype>
#include<stdexcept>
#include "Tools.h"
using namespace std;

namespace dsl
{
    // DLL w. head, tail
    class TaskList : public Object
    {
        private:
        Node<Task>* head;
        Node<Task>* tail;
        unsigned int size;  // counter

        void refreshIds()
        {
            Node<Task>* current = head;
            unsigned int id = 1;

            while(current != nullptr)
            {
                current->data.identifier(to_string(id));
                current = current->next;
                id++;
            }
        }

        public:
        TaskList() : head(nullptr), tail(nullptr), size(0) {}   //d. constr.

        ~TaskList()
        {
            // use erase function from tools to erase ANY node:
            while(head != nullptr)  // while head not empty:
            {
                dsl::erase(head);
            }
            // move cleanup logic
            tail = nullptr;
            size = 0;
        }

        void add(const Task& data)   // add at the end of list
        {
            Node<Task>* newNode = new Node<Task>(data);

            if(tail == nullptr)
            {
                head = tail = newNode;
            }
            else 
            {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            size++;
            refreshIds();
        }

        void insert(const Task& data, unsigned int index)
        {
            if(index > size)
            {
                throw out_of_range("Index out of range");
            }

            if(index == size)
            {
                add(data);
                return;
            }

            Node<Task>* newNode = new Node<Task>(data);

            if(index == 0)
            {
                newNode->next = head;

                if(head != nullptr)
                {
                    head->prev = newNode;
                }
                else
                {
                    tail = newNode;
                }

                head = newNode;
                size++;
                refreshIds();
                return;
            }

            Node<Task>* current = head;
            for(unsigned int i = 0; i < index; i++)
            {
                current = current->next;
            }

            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;
            size++;
            refreshIds();
        }

        // removes head of list:
        void remove()
        {
            if(head == nullptr)
            {
                return;
            }

            Node<Task>* temp = head;
            head = head->next;  // move to head's next node:

            // i head's empty:
            if(head == nullptr)
            {
                tail = nullptr; // assign tail to null
            }
            else 
            {
                // prev node to null:
                head->prev = nullptr;
            }
            delete temp;
            size--;
            refreshIds();
        }

        void erase(string e)
        {
            Node<Task>* current = head;
            while(current != nullptr)
            {
                if(current->data.identifier() == e)
                {
                    if(current == head)
                    {
                        // remove it:
                        remove();
                        return;
                    }

                    if(current == tail)
                    {
                        tail = tail->prev;
                        tail->next = nullptr;
                    }
                    else 
                    {
                        current->prev->next = current->next;
                        current->next->prev = current->prev; 
                    }
                    delete current;
                    size--;
                    refreshIds();
                    return;
                }
                current = current->next;
            }
        }

        // return const head content w. Task&: uses head data: uses unvalid arg for empty list
        const Task& current() const 
        {
            if (head == nullptr) 
            {
                throw invalid_argument("List is empty");
            }
            return head->data;
        }
        
        // rename updates heade(title of a task by the ID:):
        void rename(string id, string newTitle)
        {
            Node<Task>* current = head;

            while(current != nullptr)
            {
                if(current->data.identifier() == id)
                {
                    current->data.header(newTitle);
                    return;
                }
                current = current->next;
            }
        }

        // checks if list is empty:
        bool empty() const
        {
            return head == nullptr;
        }

        // move a node up towards head:
        void increment(string e)
        {
            Node<Task>* curr = head;
            while (curr != nullptr)
            {
                if (curr->data.identifier() == e)
                {
                    if (curr == head) return;
                    Node<Task>* p = curr->prev;

                    if (p->prev != nullptr) p->prev->next = curr;
                    else head = curr;

                    if (curr->next != nullptr) curr->next->prev = p;
                    else tail = p;

                    p->next = curr->next;
                    curr->prev = p->prev;
                    curr->next = p;
                    p->prev = curr;
                    refreshIds();
                    return;
                }
                curr = curr->next;
            }
        }

        // DECREMENT: Move node down (toward tail)
        void decrement(string e)
        {
            Node<Task>* curr = head;
            while (curr != nullptr)
            {
                if (curr->data.identifier() == e)
                {
                    if (curr == tail) return;
                    Node<Task>* n = curr->next;

                    if (curr->prev != nullptr) curr->prev->next = n;
                    else head = n;

                    if (n->next != nullptr) n->next->prev = curr;
                    else tail = curr;

                    curr->next = n->next;
                    n->prev = curr->prev;
                    n->next = curr;
                    curr->prev = n;
                    refreshIds();
                    return;
                }
                curr = curr->next;
            }
        }

        // TOSTRING: Overridden display method
        string toString() const override
        {
            if (head == nullptr) return "";
            stringstream out;
            Node<Task>* curr = head;
            while (curr != nullptr)
            {
                out << curr->data.toString();
                if (curr->next != nullptr) out << "\n";
                curr = curr->next;
            }
            return out.str();
        }
    };
}

#endif
