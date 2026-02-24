#include "Memory.h"
#include "IAS.h"
#include "Decode.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <cctype>

using namespace std;
using namespace osl;

// Helper: Convert binary string to long long
long long binToDec(string s) {
    long long res = 0;
    int len = s.length();
    for (int i = 0; i < len; i++) {
        if (s[len - 1 - i] == '1') {
            res += pow(2, i);
        }
    }
    return res;
}

// Helper: Convert long long to binary string with padding
string decToBin(long long val, int bits) {
    string s = "";
    for (int i = bits - 1; i >= 0; i--) {
        long long p = pow(2, i);
        if (val >= p) {
            s += '1';
            val -= p;
        } else {
            s += '0';
        }
    }
    return s;
}

void simulate(IAS& obj, ofstream& outFile)
{
    bool running = true;

    while (running)
    {
        // FETCH

        // If IBR is empty (all zeros)
        if (obj(2).read() == string(20, '0'))
        {
            // MAR ← PC
            obj(3).write(obj(0).read());
            outFile << "MAR = PC\n" << obj.toString() << endl;

            // MBR ← M[MAR]
            int address = stoi(obj(3).read(), nullptr, 2);
            obj(4).write(obj[address].read());
            outFile << "MBR = M(MAR)\n" << obj.toString() << endl;

            string mbr = obj(4).read();

            string left  = mbr.substr(0, 20);
            string right = mbr.substr(20, 20);

            // IBR ← right instruction
            obj(2).write(right);
            outFile << "IBR = MBR(20:39)\n" << obj.toString() << endl;

            // IR ← left opcode
            obj(1).write(left.substr(0, 8));
            outFile << "IR = MBR(0:7)\n" << obj.toString() << endl;

            // MAR ← left address
            obj(3).write(left.substr(8, 12));
            outFile << "MAR = MBR(8:19)\n" << obj.toString() << endl;

            // PC ← PC + 1
            int pcVal = stoi(obj(0).read(), nullptr, 2);
            pcVal++;
            
            string pcBin = "";
            int temp = pcVal;

            for (int i = 11; i >= 0; i--)
            {
            int power = 1 << i;
            if (temp >= power)
            {
                pcBin += "1";
                temp -= power;
            }
            else
            {
                pcBin += "0";
            }
            }

            obj(0).write(pcBin);

            outFile << "PC = PC + 1\n" << obj.toString() << endl;
        }
        else    //RIGHT
        {
            // IR ← IBR(0:7)
            string ibr = obj(2).read();
            obj(1).write(ibr.substr(0, 8));
            outFile << "IR = IBR(0:7)\n" << obj.toString() << endl;

            // MAR ← IBR(8:19)
            obj(3).write(ibr.substr(8, 12));
            outFile << "MAR = IBR(8:19)\n" << obj.toString() << endl;

            // Clear IBR
            obj(2).write(string(20, '0'));
            outFile << "IBR cleared\n" << obj.toString() << endl;
        }

        // verify HALT:
        string ir = obj(1).read();
        int opcode = stoi(ir.substr(0, 8), nullptr, 2);

        if (opcode == 0)
        {
            outFile << "HALT instruction encountered.\n";
            outFile << obj.toString() << endl;
            break;
        }

        // ================= EXECUTE =================

        Decode::execute(obj);
        outFile << "After Execute:\n" << obj.toString() << endl;
    }
}


int main() {
    IAS iasObj;
    ofstream outFile("output.txt"); 

    if (outFile.is_open())
    {

        // Run the simulation
        simulate(iasObj, outFile);
        outFile.close();
    }

    // Output results stored in output.txt, not to show up in terminal:
    return 0;
}