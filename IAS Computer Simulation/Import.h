#ifndef IMPORT_H
#define IMPORT_H
#include "IAS.h"
#include<fstream>
#include<string>
#include<cctype>
using namespace std;

namespace osl
{
    void Import(IAS& obj, string filename)
    {
        ifstream file(filename);
        if (!file.is_open()) {return;}  // If file fails to open, return immediately

        string line;
        int memory_index = 0;

        while (getline(file, line) && memory_index < 4096)          // Read until invalid line or 4096 lines
        {
            if (!line.empty() && line.back() == '\r') {line.pop_back();}    // Remove carriage return

            if (line.length() == 0 || line.length() > 10) {break;}          // max 10 hex digits

            bool valid = true;  // Validate hex
            for (char c : line)
            {
                if (!isxdigit(c))
                {
                    valid = false;
                    break;
                }
            }
            if (!valid) {break;}

            long value = stol(line, nullptr, 16);                   // Convert hex string to long decimal

            string binary = "";                                     // Convert to 40-bit binary string
            for (int i = 39; i >= 0; i--) {binary += ((value >> i) & 1) ? '1' : '0';}

            obj[memory_index].write(binary);                        // Store into memory

            if (value == 0) {obj[memory_index].change(false);}       // Deactivate if zero
            memory_index++;
        }
        file.close();
    }
}
#endif