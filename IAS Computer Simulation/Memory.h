#ifndef MEMORY_H
#define MEMORY_H
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cctype>
#include "Object.h"

namespace osl
{
    bool isBinary(std::string str)
    {
        for(char c : str)
        {
            if(c != '0' && c != '1') {return false;}
        }
        return true;
    }

    bool isHex(std::string str)
    {
        char i;

        for(char c : str)
        {
            i = tolower(c);
            if(!isdigit(i) && (i < 'a' || i > 'f')) {return false;}
        }
        return true;
    }

    int toDecimal(std::string obj)
    {
        if(!isBinary(obj)) return -1;
        int rslt = 0;

        for(char i : obj)
        {
            rslt *= 2;
            if(i == '1') {rslt += 1;}
        }
        return rslt;
    }

    std::string toBinary(int obj)
    {
        if(obj < 0) {obj *= -1;}
        else if(obj == 0) {return "0";}

        std::string rslt;
        
        while(obj > 0)
        {
            rslt = std::to_string(obj % 2) + rslt;
            obj /= 2;
        }
        return rslt;
    }

    std::string negate(std::string obj)
    {
        if(!isBinary(obj)) {return "";}
        std::string cbj;
        bool carry = true;

        //inverts
        for(char c : obj)
        {
            if(c == '1') {cbj += '0';}
            else {cbj += '1';}
        }

        //adds 1
        for(int i = cbj.length() - 1;carry && i >= 0;i -= 1)
        {
            if(cbj[i] == '1') {cbj[i] = '0';}
            else
            {
                cbj[i] = '1';
                carry = false;
            } 
        }
        return cbj;
    }


    class Memory : public Object 
    {
        private:
        std::string content;
        std::string id;
        bool flag;
        static const std::string digits[10];
        static const std::string letters[6];

        std::string toBinary(std::string obj) const
        {
            char i;
            std::string rslt;

            for(char c : obj)
            {
                i = tolower(c);
                
                if(isdigit(i)) {rslt += digits[i-'0'];}
                else {rslt += letters[i-'a'];}
            }
            return rslt;
        }

        std::string toHex(std::string obj) const
        {
            int pd = (4 - obj.length() % 4) % 4;
            std::string cbj = std::string(pd,'0') + obj, sbj, rslt;
            std::string hdg = "0123456789ABCDEF";

            for(int i = 0;i < cbj.length();i += 4)
            {
                sbj = cbj.substr(i,4);
                rslt += hdg[toDecimal(sbj)]; 
            }
            return rslt;
        }

        public:
        //Default Constructor
        Memory() 
        {
            content = "0";
            id = "name";
            flag = false;
        }

        //Copy Constructor
        Memory(const Memory& obj)
        {
            content = obj.content;
            id = obj.id;
            flag = obj.flag;
        }

        //Assignment Operator
        Memory& operator=(const Memory& rhs)
        {
            if(this != &rhs)
            {
                content = rhs.content;
                id = rhs.id;
                flag = rhs.flag;
            }
            return *this;
        }

        //Destructor
        virtual ~Memory() {}

        std::string read() 
        {
            flag = false;
            return content;
        }

        void write(std::string obj)
        {
            if(isBinary(obj))
            {
                flag = true;
                content = obj;
            }
        }

        std::string name() const {return id;}

        void name(std::string obj) {id = obj;}

        bool active() const {return flag;}

        void change(bool obj) {flag = obj;}

        friend std::istream& operator>>(std::istream& i,Memory& obj)
        {
            std::string str;
            i >> str;

            if(isBinary(str)) {obj.content = str;}
            else if(isHex(str)) {obj.content = obj.toBinary(str);}
            else {obj.content = std::string(8,'0');}
            return i;
        }

        std::string toString() const override 
        {
            return id + ": " + toHex(content);
        }

    };

    const std::string Memory::digits[10] = {"0000","0001","0010","0011","0100","0101","0110","0111","1000","1001"};
    const std::string Memory::letters[6] = {"1010","1011","1100","1101","1110","1111"};
        
}
#endif
