// DLL Program to simulate browser history. Each website visited is a node, clicking a link adds a nodeto end of the list. 
// "Back" button moves to NEXT() node.
// "current" pointer tracks webpage user is currently viewing.
#include<iostream>
#include<string>

// Node struct class: url string, *prev, *next pointers, default constructor:
struct Node
{
    std::string url;
    Node* prev, *next;
    Node(std::string u) : url(u), prev(nullptr), next(nullptr) {}
};

class Browser_History   // circular doubly linked list:
{
    private:
    // Hold current/head nodes:
    Node* head;
    Node* current;

    public:
    // default constructor initialized with homepage:
    Browser_History(std::string homepage)
    {
        // uses head to assign a new node holding homepage:
        head = new Node(homepage);
        head->next = head;          // point to self to start circle cycle:
        head->prev = head;          // makes end point back to start/ head:
        current = head;             // updates it.
    }

    // visit a new url, clears everything FORWARD from current postion:
    void visit(std::string url)
    {
        Node* temp = current->next; // delete everything btw. current and head:

        while(temp != head)
        {
            Node* del = temp;   // to delete
            temp = temp->next;
            delete del; // deletes everything through assign of del tmep
        }

        // insert new node and close circle:
        Node* newNode = new Node(url);  // close for current
        current->next = newNode;
        newNode->prev = current;

        newNode->next =head;    // head next prev, assign current tp newNode
        head->prev = newNode;

        current = newNode;
        std::cout << "Visited: " << current->url << "\n";
    }

    // move back: int param has to decrements while param > 0 and curent->prev != null!!!
    void back(int steps)
    {
       for(int i = 0; i < steps; i++)
        {
            current = current->prev;
        }
        std::cout << "Back to: " << current->url << std::endl;    // returns the previous url now!!!
    }

    // move forward: uses same logic as back but with next nodes:
    void forward(int steps)
    {
        for(int i = 0; i < steps; i++)
        {
            current = current->next;
        }
        std::cout << "Forwarded to: " <<  current->url << std::endl;
    }

    // function to remove nodes/ delete:
    void clear(Node* node)
    {
        while(node != nullptr)
        {
            Node* temp = node->next;
            delete node;
            node = temp;
        }   
    }
    
    // delete destructor since not smart pointers: Cleans up ALL the nodes:
    ~Browser_History()
    {
        while(current->prev != nullptr)
        {
            current = current->prev;
        }
        // delete everything: Use clear/remove fucntion here:
        clear(current);
        std::cout << "Memory Cleared.\n";
    }

    // print function:
    void print()
    {
        Node* temp = current;

        std::cout << "\n---Circular doubly linked list implementation---\n\n";

        do 
        {
            if(temp == current)
            {
                std::cout << "[" << temp->url << " ]*" << std::endl;
            }
            else
            {
                std::cout << temp->url << " ";
            }
            std::cout << "<-> ";

            temp = temp->next;
        }
        while(temp != head);    // uses ;
        {
            std::cout << "(Back to " << head->url << ")\n";
        }
    }
};
int main()
{
    Browser_History* MyBrowser = new Browser_History("www.Google.com"); // assign to 1

    // Visited urls:
    MyBrowser->visit("www.Github.com");                                 // 2 assign
    MyBrowser->visit("www.Cppreference.com");                           // 3 assign
    MyBrowser->visit("www.Leetcode.com");                               // 4 assign
    MyBrowser->visit("www.Stackoverflow.com");                          // 5 assign

    // using  back and forward function should take us to each respective url:
    MyBrowser->print();

    std::cout << "Moving forward 1 step from end: " << std::endl;
    MyBrowser->forward(1);  // wraps  around to google expected:

    MyBrowser->forward(2);  // should output: cppreference
    // MyBrowser->back(1);         // loops back to end at url stakcoverflow

    MyBrowser->back(4);         // should output to leetcode
    return 0;
}
