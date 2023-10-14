#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
#endif /* __PROGTEST__ */

const int MAX_FIB = 30; //fib(30)=832040, 31 = 1346269 ... vynechavame 0,, [28]=832040
class Encoder{
    unsigned long fib_nums[MAX_FIB] = {};
    ifstream in;
    ofstream out;

    void fib(){
        fib_nums[0] = 1;
        fib_nums[1] = 2;
        for ( unsigned long i = 2; i < MAX_FIB; i++ ) 
            fib_nums[i] = fib_nums[i-1] + fib_nums[i-2];
    }

    //getUtf8ToFib ... numToFib -> utf8 codepoint na fib code
    vector<bool> reverse(vector<bool>& vect) {
        int n = vect.size();
        vector<bool> revers(n);
        for (int i = 0; i < n; ++i) 
            revers[i] = vect[n - 1 - i];
        return revers;
    }
    void numToFib(unsigned int n, vector<bool> &code) {
        n++;
        int fib_idx = 0;
        while (fib_nums[fib_idx] <= n) 
            ++fib_idx;
        
        --fib_idx;

        unsigned long remainder = n;
        while (fib_idx >= 0) {
            if (fib_nums[fib_idx] <= remainder) {
                remainder -= fib_nums[fib_idx];
                code.push_back(true);
            } else 
                code.push_back(false);
            --fib_idx;
        }
        code = reverse(code);
        code.push_back(true); // musí být po reversu, protože před to dáváme na konec a po reversu by to bylo na začátku
    }
    bool writeUtfToFib(vector<bool>& code){
        int c = code.size() /8;
        for (int i = 0; i < c; ++i) {
            unsigned char bytik = 0;
            for (int k = 0; k < 8; k++) 
                bytik |= (code[k] << (7 - k));
            unsigned char reverse_bytik = 0;
            for (int k = 0; k < 8; k++) 
                reverse_bytik |= ((bytik >> k) & 1) << (7 - k);
            if (!out.is_open()) 
                return false;
            out.write(reinterpret_cast<const char *>(&reverse_bytik), sizeof(reverse_bytik));
            if(out.fail()) 
                return false;
            code.erase(code.begin(), code.begin() + 8);
        }
        return true;
    }
    bool validateFirstByte(unsigned char byte, int &count){//first b --sec --third --forth
        if (byte <= 127)                      count = 1; //0xxxxxxx
        else if (byte >= 194 && byte <= 223)  count = 2; //110xxxxx	10xxxxxx
        else if (byte >= 224 && byte <= 239)  count = 3; //1110xxxx	10xxxxxx 10xxxxxx
        else if (byte >= 240 && byte <= 247)  count = 4; //11110xxx	10xxxxxx 10xxxxxx 10xxxxxx
        else { out.close(); in.close(); return false; }
        return true;
    }
    bool getAndCheckBytes(unsigned int &num, int count, int i){
        unsigned char byte = 0;
        if (!in.is_open()) {out.close();return false; }
        if (in.good() && in.get((char&)byte)) {
            if ((byte & 192) != 128) { //128 = 10000000 (10xxxxxx), 192 = 11000000 (110xxxxx) ... trailing byty jsou v rozmezí 128-191
                out.close();
                in.close();
                return false;
            }
            switch(count) {
                case 2: num = ((num-192) << 6) | ((unsigned char)byte-128); break; //odečtu first byte "prefix", to samé u trailing bytu a sečtu je
                case 3: if (i==1) num = ((num-224) << 12 ) | ((unsigned char)byte-128) << 6;//shift o 6, jak odečtu základní trailing byte, tak max bude mít 6 čísel v bin
                    else num = (num ) | ((unsigned char)byte-128);
                    break;
                case 4: if (i==1) num = (num-240) << 18 | ((unsigned char)byte-128) << 12;
                    else if (i==2)  num = num | ((unsigned char)byte-128) << 6;
                    else num = num  | ((unsigned char)byte-128);
                    break;
            }
        }else {out.close();in.close();return false; }
        return true;
    }

    //getFibToUtf ... fib to num -> zpet na codepoint z fib code a numToUtf8 -> codepoint utf8 na ty 1-4 bajty
    bool fibToNum(vector<bool> &seq_of_bits, vector<unsigned int> &sequence_nums){
        bool prev_is_one = false;
        unsigned long sum = 0;
        for (int i = 0; i < (int)seq_of_bits.size(); i++ ){
            if (prev_is_one && seq_of_bits[i]){
                seq_of_bits.erase(seq_of_bits.begin(), seq_of_bits.begin()+i+1); //smazu i tu koncici 1
                if (sum > 1114112) {in.close(), out.close(); return false;} //stejny jak i=28, ale tak whatever... 1114112 misto 1114111, cuz mame mame +1 kvuli nule
                sequence_nums.push_back(--sum);
                sum = 0;
                prev_is_one = false;
                i=-1;
                continue;
            }else {
                if (seq_of_bits[i]) {
                    prev_is_one = true;
                    sum += fib_nums[i];
                }else prev_is_one = false;
            }
            if (i>28) {in.close(), out.close(); return false;} //mimo fib index, tedy celkově
        }
        return true;
    }
    bool numToUtf8(unsigned int orig_utf8, string &result){
        if (orig_utf8 <= 127) result += orig_utf8; //ascii 0-127
        else if (orig_utf8 <= 2047){
            result += (unsigned char) ((orig_utf8 >> 6) | 0xc0); //c0 = 192
            result += (unsigned char) ((orig_utf8 & 0x3f) | 0x80); //3f=63=00111111, 0x80 = 128
        } //128-2047
        else if (orig_utf8 <= 65535){
            result += (unsigned char) ((orig_utf8 >> 12) | 0xe0); //e0=224
            result += (unsigned char) (((orig_utf8 >> 6) & 0x3f) | 0x80);
            result += (unsigned char) ((orig_utf8 & 0x3f) | 0x80);
        } //2048 - 65535
        else if (orig_utf8 <= 0x10ffff){
            result += (unsigned char) ((orig_utf8 >> 18) | 0xf0); //f0 = 240
            result += (unsigned char) (((orig_utf8 >> 12) & 0x3f) | 0x80);
            result += (unsigned char) (((orig_utf8 >> 6) & 0x3f) | 0x80);
            result += (unsigned char) ((orig_utf8 & 0x3f) | 0x80);
        } ///65536-1114111 -- codepointy
        else {in.close(), out.close(); return false;}
        return true;
    }
    bool writeFibToUtf(string &result){
        for (char & x : result) {
            if (out.fail()) { out.close(); in.close();return false;}
            if (!out.is_open()) {in.close();return false; }
            out.write(&x, 1);
            if (out.fail()) { out.close(); in.close();return false;}
        }
        return true;
    }

public:
    Encoder(const char * &_file, const char * &_out_file){
        fib();
        in.open(_file, ios::binary);
        out.open(_out_file, ios::binary);
    }

    bool getUtf8ToFib(){
        unsigned char byte;
        vector<bool> fib_bits_waiting_for_write; //napr prebyvajici bity, tedy %8!=8, ktere bud doplnim nulama nebo dalsim znakem podle situace
        vector<bool> fib_byte_waiting_for_write; //musim reversnout před psaním
        while(in.good() && in.get((char&)byte)){
            vector<bool> code;
            int count  = 0;

            if (!validateFirstByte(byte, count)) return false;

            unsigned int num = byte;
            for (int i = 1; i < count; i++) if (!getAndCheckBytes(num, count, i)) return false;
//            cout << hex << (int)(unsigned char) num << endl;
            numToFib(num, code);
            cout << hex << (unsigned int) num << dec <<  " - " << num << " ---> ";
            for (auto x : code) cout << x;
            cout << endl;
            fib_bits_waiting_for_write.insert(fib_bits_waiting_for_write.end(), code.begin(), code.end());
            if (fib_bits_waiting_for_write.size() >= 8) if (!writeUtfToFib(fib_bits_waiting_for_write)) { out.close();in.close();return false; }
        }

        if ((in.bad() || in.fail()) && !in.eof()){out.close();in.close();return false; }
        if (fib_bits_waiting_for_write.size() > 0){//kdyz nejake bity zbydou, vypisu je a na konec pridam 0 (melo by jich byt 1-7)
            fib_bits_waiting_for_write.insert(fib_bits_waiting_for_write.end(), 8-fib_bits_waiting_for_write.size(), false);
            if (!writeUtfToFib(fib_bits_waiting_for_write)) { out.close();in.close();return false; }
        }

        out.close();
        in.close();
        return true;
    }

    bool getFibToUtf(){ //fibToUtf8
        unsigned int byte = 0;
        vector<bool> seq_of_bits = {};

        while(in.good() && in.get((char&)byte)){
            vector<bool> fib_byte = {};
            vector<unsigned int> sequence_nums = {};
            for (int i  = 0; i < 8; i++) {
                bool bit = byte & (1 << i);
                fib_byte.push_back(bit);
            }

            seq_of_bits.insert(seq_of_bits.end(), fib_byte.begin(), fib_byte.end());

            if(!fibToNum(seq_of_bits,sequence_nums)) 
                return false;

            int size = sequence_nums.size();
            for (int i = 0; i < size; i++){
                cout << hex << (unsigned int) sequence_nums[i] << endl;
                string res;
                if (!numToUtf8(sequence_nums[i], res)) return false;
                if (!writeFibToUtf(res)) return false;
            }
        }
        in.close();
        out.close();
        if ((in.fail() || in.bad()) && !in.eof()) return false;
        int rest = 0;
        for (auto x : seq_of_bits) rest+=x; //kdyby tam byl nejake jednickove bity, ale nebyly ukoncene, tzn nemely 2x 1 na konci
        if (rest > 0) return false;
        return true;
    }
};

bool utf8ToFibonacci(const char* inFile, const char* outFile){
    Encoder enc(inFile, outFile);
    return enc.getUtf8ToFib();
}

bool fibonacciToUtf8(const char* inFile, const char* outFile){
    Encoder enc(inFile, outFile);
    return enc.getFibToUtf();
}

#ifndef __PROGTEST__
bool identicalFiles(const char* file1, const char* file2){
    ifstream f1(file1, ios::binary), f2(file2, ios::binary);
    return equal(istreambuf_iterator<char>(f1.rdbuf()), istreambuf_iterator<char>(), istreambuf_iterator<char>(f2.rdbuf()));

}

int main ( int argc, char * argv [] ){
//    assert ( ! fibonacciToUtf8( "example2/napovedaFalse.bin", "output.utf8" ) );
//
//    assert (  fibonacciToUtf8 ( "example2/fibToUtfTrue.bin", "output.utf8" ) );
//    assert ( ! utf8ToFibonacci( "example2/utfToFib1.bin", "output.utf8" ) );
//    assert ( ! utf8ToFibonacci( "example2/utfToFib2.bin", "output.utf8" ) );
//    assert ( ! utf8ToFibonacci( "example2/utfToFib3.bin", "output.utf8" ) );
//
//    assert ( utf8ToFibonacci ( "example2/in_w_ref.bin", "output.fib" )
//             && identicalFiles ( "output.fib", "example2/ref.bin" ) );
//
//
//    assert ( utf8ToFibonacci ( "example/src_0.utf8", "output.fib" )
//             && identicalFiles ( "output.fib", "example/dst_0.fib" ) );
//    assert ( utf8ToFibonacci ( "example/src_1.utf8", "output.fib" )
//             && identicalFiles ( "output.fib", "example/dst_1.fib" ) );
//    assert ( utf8ToFibonacci ( "example/src_2.utf8", "output.fib" )
//             && identicalFiles ( "output.fib", "example/dst_2.fib" ) );
//    assert ( utf8ToFibonacci ( "example/src_3.utf8", "output.fib" )
//             && identicalFiles ( "output.fib", "example/dst_3.fib" ) );
//    assert ( utf8ToFibonacci ( "example/src_4.utf8", "output.fib" )
//             && identicalFiles ( "output.fib", "example/dst_4.fib" ) );
//    assert ( ! utf8ToFibonacci ( "example/src_5.utf8", "output.fib" ) );
//
//
//
    assert ( fibonacciToUtf8 ( "example/src_6.fib", "output.utf8" )
             && identicalFiles ( "output.utf8", "example/dst_6.utf8" ) );
//    assert ( fibonacciToUtf8 ( "example/src_7.fib", "output.utf8" )
//             && identicalFiles ( "output.utf8", "example/dst_7.utf8" ) );
//    assert ( fibonacciToUtf8 ( "example/src_8.fib", "output.utf8" )
//             && identicalFiles ( "output.utf8", "example/dst_8.utf8" ) );
//    assert ( fibonacciToUtf8 ( "example/src_9.fib", "output.utf8" )
//             && identicalFiles ( "output.utf8", "example/dst_9.utf8" ) );
//    assert ( fibonacciToUtf8 ( "example/src_10.fib", "output.utf8" )
//             && identicalFiles ( "output.utf8", "example/dst_10.utf8" ) );
//    assert ( ! fibonacciToUtf8 ( "example/src_11.fib", "output.utf8" ) );

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
