#include <iostream>
#include <vector>
using namespace std;

// ---------------- STUDENT CLASS ----------------
class Student
{
private:
    int id, age;
    string name;

public:
    Student(int stdID, int stdAGE, string stdNAME)
        : id(stdID), age(stdAGE), name(stdNAME) {}

    // Getters
    int getID() const { return id; }
    int getAGE() const { return age; }
    string getNAME() const { return name; }

    // Setters
    void setAGE(int stdAGE) { age = stdAGE; }
    void setNAME(string stdNAME) { name = stdNAME; }

    void display() const
    {
        cout << "ID: " << id
             << ", Age: " << age
             << ", Name: " << name << endl;
    }
};

// ---------------- STUDENT MANAGER CLASS ----------------
class StudentManager
{
private:
    vector<Student> students;

public:
    void addStudent()
    {
        int id, age;
        string name;

        cout << "Enter ID: ";
        cin >> id;

        cout << "Enter Age: ";
        cin >> age;

        cout << "Enter Name: ";
        cin >> name;

        students.push_back(Student(id, age, name));
        cout << "Student added successfully!\n";
    }

    void displayAll() const
    {
        if (students.empty())
        {
            cout << "No students available.\n";
            return;
        }

        cout << "\n--- Student List ---\n";
        for (const Student& s : students)
        {
            s.display();
        }
    }

    void searchStudent() const
    {
        int searchID;
        cout << "Enter ID to search: ";
        cin >> searchID;

        for (const Student& s : students)
        {
            if (s.getID() == searchID)
            {
                cout << "Student Found:\n";
                s.display();
                return;
            }
        }

        cout << "Student not found.\n";
    }

    void updateStudent()
    {
        int searchID;
        cout << "Enter ID to update: ";
        cin >> searchID;

        for (Student& s : students)
        {
            if (s.getID() == searchID)
            {
                int newAge;
                string newName;

                cout << "Enter new age: ";
                cin >> newAge;

                cout << "Enter new name: ";
                cin >> newName;

                s.setAGE(newAge);
                s.setNAME(newName);

                cout << "Student updated successfully!\n";
                return;
            }
        }

        cout << "Student not found.\n";
    }

    void deleteStudent()
    {
        int searchID;
        cout << "Enter ID to delete: ";
        cin >> searchID;

        for (int i = 0; i < students.size(); i++)
        {
            if (students[i].getID() == searchID)
            {
                students.erase(students.begin() + i);
                cout << "Student deleted successfully!\n";
                return;
            }
        }

        cout << "Student not found.\n";
    }
};

// ---------------- MAIN ----------------
int main()
{
    StudentManager manager;
    int option;

    do
    {
        cout << "\n--- MENU ---\n";
        cout << "1. Add Student\n";
        cout << "2. Display All Students\n";
        cout << "3. Search Student\n";
        cout << "4. Update Student\n";
        cout << "5. Delete Student\n";
        cout << "6. Exit\n";
        cout << "Enter option: ";
        cin >> option;

        switch (option)
        {
        case 1:
            manager.addStudent();
            break;
        case 2:
            manager.displayAll();
            break;
        case 3:
            manager.searchStudent();
            break;
        case 4:
            manager.updateStudent();
            break;
        case 5:
            manager.deleteStudent();
            break;
        case 6:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid option!\n";
        }

    } while (option != 6);

    return 0;
}
