#include "BitStream.h"

#include <math.h>
#include <string>

using namespace std;

BitStream::BitStream(){}

BitStream::BitStream(const char *filename, char modein){
    if(modein == 'w'){ // write, mode = 0
        file = fstream(filename, ios::binary | ios::out);
        pointer = 8;
        mode = 0;
    }else if(modein == 'r'){  // read, mode = 1
        file = fstream(filename, ios::binary | ios::in);
        pointer = 0;
        mode = 1;
        file.seekg(0, ios::end);
        size = file.tellg();
        file.seekg(0, ios::beg);
    }else{
        throw runtime_error("Wrong file open mode");
    }
    buffer = 0;
    if(!file.is_open())
        throw runtime_error("Could not open file");
}

unsigned char BitStream::readBit(){
    if(mode == 0) throw runtime_error("Cannot read in 'w' mode");
    if(pointer == 0){
        pointer = 8;
        file.read(reinterpret_cast<char*>(&buffer), 1);
    }
    pointer--;
    return (buffer >> pointer) & 0x01;
}

void BitStream::writeBit(char bit){
    if(mode == 1) throw runtime_error("Cannot write in 'r' mode");
    if(pointer == 1){
        buffer |= (bit & 0x01);
        file.write(reinterpret_cast<char*>(&buffer), 1);
        buffer = 0;
        pointer = 8;
        return;
    }
    pointer--;
    buffer |= ((bit & 0x01) << pointer);
}

void BitStream::readNbits(char* bits, int n){
    if(mode == 0) throw runtime_error("Cannot read in 'w' mode");
    for(int i = 0; i < n; i++){
        bits[i] = readBit();
    }
}

void BitStream::writeNbits(char* bits, int n){
    if(mode == 1) throw runtime_error("Cannot write in 'r' mode");
    for(int i = 0; i < n; i++){
        writeBit(bits[i]);
    }
}

bool BitStream::eof(){
    if(mode == 0) throw runtime_error("Cannot access eof() in 'w' mode");
    if(file.tellg() < size) return false;
    if(pointer > 0) return false;
    return true;
}

// write remain buffer on file
void BitStream::close(){
    if(mode == 0) // if write
        if (pointer != 8)
            file.write(reinterpret_cast<char*>(&buffer), 1);
    file.close();
}