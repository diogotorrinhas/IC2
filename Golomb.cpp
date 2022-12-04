#include "Golomb.h"
#include "BitStream.h"
#include <math.h>
#include <cstdlib>

using namespace std;

//Convert binary char array to int
int binaryToInt(char arr[], int size){
    int res=0;
    for(int i = 0 ;i < size; i++){
        if(arr[i] != 0x0){
            res = res + pow(2, i);
        }
    }
    return res;
}

// Function that convert Decimal to string in Binary
string decToBinary(int n, int numBits)
{
    string numberBin = "";
    // Size of an integer is assumed to be 32 bits
    for (int i = 0; i < numBits; i++) {
        int k = n >> i;
        if (k & 1)
            //cout << "1";
            numberBin = numberBin + "1";
        else
            //cout << "0";
            numberBin = numberBin + "0";
    }
    return numberBin;
}



int flag = 0;
// int ngeral = 0;

Golomb::Golomb(){}

Golomb::Golomb(const char *filename, char mode, int mvalue){
    if (mode != 'd' && mode != 'e'){
        cout << "ERROR: invalid mode!" << endl;
        exit(EXIT_FAILURE);
    }
    if (mode == 'd')
        Gfile = BitStream(filename, 'r');
    else
        Gfile = BitStream(filename, 'w');
    m = mvalue;
    b =  ceil(log2(m));

}


int Golomb::encode(int n){
    //printf("Value to be encoded before Folding: %d \n", n);
    n = fold(n);
    //printf("Value to be encoded after Folding: %d \n", n);
    
    
    int q = floor((int)(n / m));
    int r = n - q*m;

    // Variable to store number of bits of the binary part
    int numbits;
    //Value to be coded in the binary part
    int value;

    //If m is power of two 
    if (m!=0 && (m & (m-1)) == 0){
        value = r;
        numbits = b;
    }
    //If m is not power of two
    else{
        if (r < pow(2, b) - m){
            value =  r; 
            numbits = b-1;
        }
        else{
            value = r + pow(2, b) - m;
            numbits = b;
        }
    }
    
    //Calculate Binary part
    string aux;
    int numbitsAux = numbits;

    while (value != 0){
        aux += ( value % 2 == 0 ? '0' : '1' );
        value /= 2;
        numbitsAux --;
    }
    while(numbitsAux != 0){
        aux+= '0';
        numbitsAux--;
    }

    //Size of the word to return in the end
    int size = numbits;

    //Calculate unary part and write it
    size++;
    for (int i = 0 ; i < q; i++){
        Gfile.writeBit('0');
        size++;
    }
    Gfile.writeBit('1');

    //Write binary value in Bitstream
    for(int i = numbits-1; i >= 0; i-- )
        Gfile.writeBit(aux[i]);
    return size;
}

int Golomb::decode(){
    int A = 0;
    int R = 0;
    // int size = 0;

    char c;
    //Count number of 0s before the first 1 (Msbits) -> A
    while(true){
        c = Gfile.readBit();
        if((c & 0x01) == 0x01)
            break;
        A++;
    }

    //If m is power of two 
    if (m!=0 && (m & (m-1)) == 0){

        char binary[b];
        Gfile.readNbits(binary,b);
    
        //Get b+1 Lsbits of the origingal value to be decoded (R = (0)+b Lsbit in decimal)
        int aux = 0;
        for( int i = b-1; i >= 0; i--){
            if(binary[i] != 0x0)
                R+= pow(2, aux);
            aux++;
        }
        //Calculate decoded value
        return unfold(m*A + R);
    }
    //If m is not power of two
    else{
        int aux = 0;
        char binary[b];
        Gfile.readNbits(binary,b-1);
        binary[b-1] = 0;

        //Extract b-1 Msbs of the original word without the first 0s and the first 1and calculate R in decimal
        for (int i = b-2; i >= 0; i--){
            if(binary[i] != 0x00)
                R+= pow(2, aux);
            aux++;
        } 
 
        if(R < pow(2, b) - m){
            return unfold(m*A + R);
        }
        //Extract b Msbs of the original word without the first 0s and the first 1 and calculate R in decimal
        else{
            binary[b-1] = Gfile.readBit();
            R=0, aux=0;
            for (int i = b-1; i >= 0; i--){
                if(binary[i] != 0x0)
                    R+= pow(2, aux);
                aux++;
            }
            return unfold(m*A + R - (pow(2, b) - m)); 
        }
    }
    return 0;
}

void Golomb::encodeM(int m){
    string aux = decToBinary(m,32);
    char res[32];
    for(int i = 0; i < 32; i++){
        res[i] = aux[i];
    }
    Gfile.writeNbits(res,32);
}

int Golomb::decodeM(){
    char res[32];
    Gfile.readNbits(res, 32);
    return binaryToInt(res, 32);
}

void Golomb::setM(int mi){
    this->m = mi;
    b =  ceil(log2(m));
}


void Golomb::encodeHeaderSound(int nFrames, int sampleRate, int Channels, int format, bool lossy){
    string header;
    if(lossy)
        header = '1';
    else
        header = '0';
    header += decToBinary(nFrames, 32);
    header += decToBinary(sampleRate, 32);
    header += decToBinary(format,32);
    header += decToBinary(Channels, 4);

    char res[101];
    for(int i=0; i < 101; i++)
        res[i] = header[i];
    Gfile.writeNbits(res, 101);
}

void Golomb::encondeShamt(int shamt){
    //Shamt -> 5 bits;
    string sh = decToBinary(shamt, 5);
    char res[5];
    for(int i=0; i < 5; i++)
        res[i] = sh[i];
    Gfile.writeNbits(res, 5);
}

int Golomb::decodeShamt(){
    char res[5];
    Gfile.readNbits(res, 5);
    return binaryToInt(res,5);
}

void Golomb::decodeHeaderSound(int arr[]){
    //Arr[0] -> codecOption (lossy or lossless)
    char option = Gfile.readBit();
    arr[0] = int(option&0x1);    
    char rd[32];
    //Arr[1] -> num of samples
    Gfile.readNbits(rd, 32);
    arr[1] = binaryToInt(rd,32);
    //Arr[2] -> Sample Rate
    Gfile.readNbits(rd, 32);
    arr[2] = binaryToInt(rd,32);
    //Arr[3] -> num of samples
    Gfile.readNbits(rd, 32);
    arr[3] = binaryToInt(rd,32);
    //Arr[4] -> num of channels
    char nc[4];
    Gfile.readNbits(nc, 4);
    arr[4] = binaryToInt(nc,4);
}

int Golomb::fold(int n){
    flag = 1;
    if (n >= 0)
        return n*2;
    else
        return abs(n)*2-1;

}


int Golomb::unfold(int n){
    flag = 0;
    if (n % 2 == 0)
        return n/2;
    else
        return (-1)*ceil(n/2)-1;

}

void Golomb::close(){
   Gfile.close();
}
