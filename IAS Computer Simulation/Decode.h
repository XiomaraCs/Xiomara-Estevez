#ifndef DECODE_H
#define DECODE_H
#include "IAS.h"
#include <string>
#include <cmath>
using namespace std;

namespace osl
{
    class Decode
    {
        public:
        static void execute(IAS& obj)
        {
            // IR contains only the 8-bit opcode
            string opcode_bin = obj(1).read();

            // MAR contains the 12-bit address
            string Xaddress_bin = obj(3).read();

            int opcode = stoi(opcode_bin, nullptr, 2);
            
            switch(opcode)
            {
                case 0: HLT(obj); break;
                case 1: LDA(obj, Xaddress_bin); break;
                case 2: LDN(obj, Xaddress_bin); break;
                case 3: ALD(obj, Xaddress_bin); break;
                case 4: ALN(obj, Xaddress_bin); break;
                case 5: ADD(obj, Xaddress_bin); break;
                case 6: SUB(obj, Xaddress_bin); break;
                case 7: AAD(obj, Xaddress_bin); break;
                case 8: ASB(obj, Xaddress_bin); break;
                case 9: LDM(obj, Xaddress_bin); break;
                case 10: LMA(obj); break;
                case 11: MUL(obj, Xaddress_bin); break;
                case 12: DIV(obj, Xaddress_bin); break;
                case 13: BRL(obj, Xaddress_bin); break;
                case 14: BRR(obj, Xaddress_bin); break;
                case 15: BPL(obj, Xaddress_bin); break;
                case 16: BPR(obj, Xaddress_bin); break;
                
                case 18: STL(obj, Xaddress_bin); break;
                case 19: STR(obj, Xaddress_bin); break;
                case 20: LSH(obj); break;
                case 21: RSH(obj); break;
                case 33: STA(obj, Xaddress_bin); break;

                default:
                    cout << "Invalid opcode: " << opcode << endl;
                    break;
            }
        }


        private:

        static string toBinary(long value)
        {
            bool negative = (value < 0);

            if (negative) {value = -value;}

            string result = "";
            for(int i = 39; i >= 0; i--)
            {
                long power = pow(2, i);
                if(value >= power)
                {
                    result += "1";
                    value -= power;
                }
                else
                {
                    result += "0";
                }
            }

            if (negative) {result = negate(result);}

            return result;
        }

        static long toDecimal(string bin)
        {
            if (bin[0] == '0') {return stol(bin, nullptr, 2);}

            string pos = negate(bin);
            long value = stol(pos, nullptr, 2);
            return -value;
        }

        // ================= OPCODES =================

        static void HLT(IAS&) {}

        static void LDA(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            obj(5).write(obj[a].read());
        }

        static void LDN(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            obj(5).write(negate(obj[a].read()));
        }

        static void ALD(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            string value = obj[a].read();

            if (value[0] == '1') {value = negate(value);}

            obj(5).write(value);
        }

        static void ALN(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            string value = obj[a].read();

            if (value[0] == '1') {value = negate(value);}

            obj(5).write(negate(value));
        }

        static void ADD(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            long result = toDecimal(obj(5).read()) + toDecimal(obj[a].read());
            obj(5).write(toBinary(result));
        }

        static void SUB(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            long result = toDecimal(obj(5).read()) - toDecimal(obj[a].read());
            obj(5).write(toBinary(result));
        }

        static void AAD(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);

            long ac = toDecimal(obj(5).read());
            string value = obj[a].read();

            if (value[0] == '1') {value = negate(value);}

            long result = ac + stol(value, nullptr, 2);
            obj(5).write(toBinary(result));
        }

        static void ASB(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);

            long ac = toDecimal(obj(5).read());
            string value = obj[a].read();

            if (value[0] == '1') {value = negate(value);}

            long result = ac - stol(value, nullptr, 2);
            obj(5).write(toBinary(result));
        }

        static void LDM(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            obj(6).write(obj[a].read());
        }

        static void LMA(IAS& obj)
        {
            obj(5).write(obj(6).read());
        }

        static void MUL(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            long result = toDecimal(obj(6).read()) * toDecimal(obj[a].read());

            long high = result >> 40;
            long low  = result & ((1L << 40) - 1);

            obj(5).write(toBinary(high));
            obj(6).write(toBinary(low));
        }

        static void DIV(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            long ac = toDecimal(obj(5).read());
            long mem = toDecimal(obj[a].read());

            obj(6).write(toBinary(ac / mem));
            obj(5).write(toBinary(ac % mem));
        }

        static void BRL(IAS& obj, string addr)
        {
            obj(0).write(addr);
        }

        static void BRR(IAS& obj, string addr)
        {
            obj(0).write(addr);
        }

        static void BPL(IAS& obj, string addr)
        {
            if(toDecimal(obj(5).read()) >= 0) {obj(0).write(addr);}
        }

        static void BPR(IAS& obj, string addr)
        {
            if(toDecimal(obj(5).read()) >= 0) {obj(0).write(addr);}
        }

        static void LSH(IAS& obj)
        {
            long v = toDecimal(obj(5).read());
            obj(5).write(toBinary(v * 2));
        }

        static void RSH(IAS& obj)
        {
            long v = toDecimal(obj(5).read());
            obj(5).write(toBinary(v / 2));
        }

        static void STL(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            string ac = obj(5).read();
            string mem = obj[a].read();

            mem.replace(8, 12, ac.substr(28, 12));
            obj[a].write(mem);
        }

        static void STR(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            string ac = obj(5).read();
            string mem = obj[a].read();

            mem.replace(28, 12, ac.substr(28, 12));
            obj[a].write(mem);
        }

        static void STA(IAS& obj, string addr)
        {
            int a = stoi(addr, nullptr, 2);
            obj[a].write(obj(5).read());
        }
    };
}
#endif
