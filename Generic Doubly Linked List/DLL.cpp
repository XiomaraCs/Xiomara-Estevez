#include <iostream>
#include <stdexcept>
using namespace std;

template <class T>
class DLL
{
    private:
    struct Node
    {
        T data;
        Node* next;
        Node* prev;

        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    int count;

    public:
    DLL() : head(nullptr), tail(nullptr), count(0) {}

    void display() const
    {
        Node* current = head;

        while(current != nullptr)
        {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }

    T front() const
    {
        if(head == nullptr)
        {
            throw runtime_error("List is empty");
        }
        return head->data;
    }

    T back() const
    {
        if(tail == nullptr)
        {
            throw runtime_error("List is empty");
        }
        return tail->data;
    }

    void pop_back()
    {
        if(head == nullptr)
        {
            throw runtime_error("List is empty");
        }

        Node* temp = tail;

        if(head == tail)
        {
            head = tail = nullptr;
        }
        else
        {
            tail = tail->prev;
            tail->next = nullptr;
        }
        delete temp;
        count--;
    }

    void insertend(const T& value)
    {
        Node* newNode = new Node(value);

        if(head == nullptr)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }

        count++;
    }

    T at(int index) const
    {
        if(index < 0 || index >= count)
        {
            throw out_of_range("Invalid index");
        }

        Node* current = head;

        for(int i = 0; i < index; i++)
        {
            current = current->next;
        }

        return current->data;
    }

    int size() const
    {
        return count;
    }

    ~DLL()
    {
        while(head != nullptr)
        {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

int main()
{
    try
    {
        DLL<int> list;

        list.insertend(345);
        list.insertend(643);
        list.insertend(25);
        list.insertend(45);
        list.insertend(12);

        cout << "Print: ";
        list.display();

        cout << "Size: " << list.size() << endl;
        cout << "Front: " << list.front() << endl;
        cout << "Back: " << list.back() << endl;
        cout << "At index 3: (0-indexed)" << list.at(3) << endl;

        cout << "Remove from end" << endl;
        list.pop_back();

        cout << "Print after removal: ";
        list.display();
    }
    catch(exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
    return 0;
}
