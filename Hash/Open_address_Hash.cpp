// Open address has version (simple):
#include<iostream>
#include<unordered_map>
using namespace std;

class Hash
{
    private:
    static const int size = 10;
    int table[size];

    public:
    Hash()
    {
        for(int i = 0; i < size; i++)
        {
            table[i] = -1;  // means empty
        }
    }

    int hash(int key)
    {
        return key % size;
    }

    void insert(int key)
    {
        int index = hash(key);

        while(table[index] != -1)   // while table index isnt empty:
        {
            index = (index + 1) % size;
        }
        table[index] = key;
    }

    bool search(int key)
    {
        int index = hash(key);
        int start = index;

        while(table[index] != -1)
        {
            if(table[index] == key)
            {
                return true;
            }
            index = (index + 1) % size;

            if(index == start)
            {
                break;
            }
        }
        return false;
    }

    void display()
    {
        for(int i = 0; i < size; i++)
        {
            cout << i << " : ";

            if(table[i] == 1)
            {
                cout << "empty";
            }
            else{
                cout << table[i];
            }
            cout << endl;
        }
    }
};
int main()
{
    Hash hash;

    hash.insert(10);
    hash.insert(20);
    hash.insert(30);
    hash.insert(40);
    hash.insert(50);
    hash.insert(60);
    
    cout << "Displaying the list(Open address version): ";
    hash.display();

    cout << "\n";

    cout << "Find 50: " << (hash.search(50) ? "Yes" : "N0") << endl;

    //testing for 100: outpuit shoudl result for - 1:
    cout << "Testing for find 100, (output should give No): " << endl;
    cout << "Find 100: " << (hash.search(100) ? "Yes" : "No") << endl;
    return 0;
}