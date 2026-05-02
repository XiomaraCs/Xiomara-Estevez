#include<iostream>
#include<string>
#include "Scheduler.h"
using namespace std;
using namespace dsl;

int main()
{
    TaskList list;

    cout << "To Do Tasks:" << endl;

    // IDS:
    list.add(Task("Meeting"));
    list.add(Task("Pickup groceries"));
    list.add(Task("Pickup kids"));
    list.add(Task("Pilates class"));

    // display:
    cout << list.toString() << endl;

    cout << "\n";

    cout << "Deletig from the list: (4. Pilates class)" << endl;
    list.erase("4");
    cout << list.toString() << endl;

    // cout << "\n";

    // cout << "Test increment(): " << endl;
    // list.increment("0003");
    // cout << list.toString() << endl;

    // cout << "\n";

    // cout << "Test decrement(): " << endl;
    // list.decrement("0002");
    // cout << list.toString() << endl;

    // cout << "Test rename head: " << endl;
    // list.rename("0004", "betty crocker bluey gummies");
    // cout << list.toString() << endl;

    // cout << "\n";
    // cout << "Test empty: " << endl;
    // if(list.empty())
    // {
    //     cout << "List is empty\n";
    // }
    // else 
    // {
    //    cout << "List is full\n";
    // }

    cout << "\n";
    cout << "Inserting: (Cat food at index: 3): " << endl;
    list.insert(Task("Buy cat food"), 2);
    cout << list.toString() << endl;
    return 0;;
}