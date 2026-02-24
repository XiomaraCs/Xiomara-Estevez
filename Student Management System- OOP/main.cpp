#include<iostream>
#include<vector>
using namespace std;

class Student
{
    private:
    int id, age;
    string name;

    public:
    Student(int stdID, int stdAGE, string stdNAME) : id(stdID), age(stdAGE), name(stdNAME) {}
    // set id:
    void setID(int stdID) {id = stdID;}
    // get id:
    int getID() {return id;}

    // set age:
    void setAGE(int stdAGE) {age = stdAGE;}
    // get age:
    int getAGE() {return age;}

    // set name:
    void setNAME(int stdNAME) {name = stdNAME;}
    // get name:
    string getNAME() {return name;}
};

void addNewStu(vector<Student> stu)
{
    int id, age;
    string name;

    cout << "Enter id: ";
    cin >> id;

    cout << "Enter age: ";
    cin >> age;

    cout << "Enter name: ";
    cin >> name;

    Student.newStudent;
    
}
int main()
{
    vector<Student> stu;
    stu.push_back(Student(1, 20, "Abby"));  // format: id, age, name, call to constructor

    int option;
    // OPTIONS menu: add new student, display all, search, update, delete:
    switch (option)
    {
    case 1:
        cout << "1. Add New Student.\n";
        break;
    case 2: 
        cout << "2. Display All Students.\n";
        break;
    case 3:
        cout << "3. Search For Student.\n";
        break;
    case 4:
        cout << "4. Update Student Info.\n";
        break;
    case 5:
        cout << "5. Delete Student Info.\n";
        break;
    default:
        break;
    }


    return 0;
}