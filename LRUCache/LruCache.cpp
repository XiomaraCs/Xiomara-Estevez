#include<iostream>
#include<unordered_map>
using namespace std;

// used DLL:    modern version
class LRUCache
{
    private:
    struct Node
    {
        int key, value;
        Node* prev;
        Node*next;
        Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    unordered_map<int, Node*> table;
    Node* head;
    Node* tail;

    // remove node from list:
    void remove(Node* node)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void insertfront(Node* node)
    {
        node->next = head->next;
        node->prev = head;

        head->next->prev = node;
        head->next = node;
    }


    public:
    LRUCache(int CAP)
    {
        capacity = CAP;

        head = new Node(-1, -1);
        tail = new Node(-1, -1);

        head->next = tail;
        tail->prev = head;
    }

    // get value by key:
    int get(int key)
    {
        if(table.find(key) == table.end())
        {
            return -1;
        }

        Node* node = table[key];

        remove(node);

        insertfront(node);
        return node->value;
    }

    void put(int key, int value)
    {
        // insert/ update key:
        if(table.find(key) != table.end())
        {
            Node* node = table[key];
            node->value = value;

            remove(node);
            insertfront(node);
            return;
        }

        if(table.size() == capacity)
        {
            Node* lru = tail->prev;
            remove(lru);

            table.erase(lru->key);
            delete lru;
        }
        Node* newNode = new Node(key, value);

        insertfront(newNode);
        table[key] = newNode;
    }

    void display()
    {
        Node* current = head->next;
        cout << "Cache: ";

        while(current != tail)
        {
            cout << "[" << current->key << " : " << current->value << "]";
            current = current->next;
        }
        
    }

   ~LRUCache()
    {
        Node* current = head;

        while(current != nullptr)
        {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
};
int main()
{
    LRUCache cache(3);

    cache.put(1, 100);
    cache.put(2, 200);
    cache.put(3, 300);
    cache.display();

    cache.get(1);       // 1 becomes most recent
    cache.display();

    cache.put(4, 400);  // evicts key 2
    cache.display();

    cout << "Get 2: " << cache.get(2) << endl; // -1
    cout << "Get 4: " << cache.get(4) << endl; // 400

    return 0;
}