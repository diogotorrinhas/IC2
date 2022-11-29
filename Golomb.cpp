#include "Golomb.h"
#include "BitStream.h"
#include <math.h>
#include <cstdlib>

using namespace std;

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
    printf("%d\n", n);
    // if (n<0)
    //     n = fold(n);
    //cout << "Value to be decoded after Folding: " << n << endl;
    int q = floor((int)(abs(n) / m));
    int r = abs(n) - q*m;

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
     
    for(int i = numbits-1; i >= 0; i-- ){
        Gfile.writeBit(aux[i]);
    }
    if(n<0){
        Gfile.writeBit('1');
        size++;
    }
    else if(n>0){
        Gfile.writeBit('0');
        size++;
    }

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

    //If m is power of 2
    if (m!=0 && (m & (m-1)) == 0){
        //cout << "Value tem " << (A+b+1) << " bits" << endl;

        char binary[b];
        Gfile.readNbits(binary,b+1);

        //Get b+1 Lsbits of the original value to be decoded (R = (0)+b Lsbit in decimal)
        int aux = 0;
        for( int i = b-1; i >= 0; i--){
            if(binary[i] != 0x0)
                R+= pow(2, aux);
            aux++;
        }
        //printf("BINARY VALOR: %d\n", binary[b]);
        //Calculate decoded value
        // if (flag==1)
        //     return unfold(m*A + R);  // para numeros negativos
        if(binary[b]==0){
            return m*A + R;
        }
        else{
            return (m*A + R) * -1;
        }
        //return m*A + R;
    }
    //If m is not power of 2
    else{
        int aux = 0;
        int R = 0;
        char binary[b];
        // Gfile.readNbits(binary,b-1);
        Gfile.readNbits(binary,b+1);
        //binary[b-1] = 0;
        
        

        //printf("Nº de bits: %d\n", b);
        //Extract b-1 Msbs of the original word without the first zeros and the first one and calculate R in decimal
        for (int i = b-2; i >= 0; i--){
            if(binary[i] != 0x0){
                R = R + pow(2, aux);
                //printf("entrou");
            }
            aux++;
        } 
        // printf("Valor R: %d\n",R);
        // R = R + 1; // corrigir R
        // printf("Valor R: %d\n",R);
        //printf("Valor de aux: %d\n", aux);
        //printf("Valor de R: %d\n", R);
        //printf("Valor de BINARY: %d\n", binary[3]);
        if(R < pow(2, b) - m){
            // printf("Entrouuuu");
            // printf("Valor de BINARY: %d\n", binary[b-1]);
            //cout << "Value has " << (A+1) + b - 1 <<" bits"<< R <<endl;
            // if (flag==1)
            //     return unfold(m*A + R);
            if(binary[b-1]==0){
                return m*A + R;
            }
            else{
                return (m*A + R) * -1;
            }
            //return m*A + R;
        }
        //Extract b Msbs of the original word without the first zeros and the first one and calculate R in decimal
        else{
            // printf("Entrouuuu");
            binary[b-1] = Gfile.readBit();
            //cout << "Values has " << (A+1+b) << " bits"<< endl;
            R=0, aux=0;
            for (int i = b-1; i >= 0; i--){
                if(binary[i] != 0x0)
                    R+= pow(2, aux);
                aux++;
            }
            // printf("Valor R: %d\n",R);
            R = R + 1; // corrigir R
            // printf("Valor R: %d\n",R);
            // if (flag==1)
            //     return unfold(m*A + R - (pow(2, b) - m)); 
            if(binary[b]==0){
                return m*A + R - (pow(2, b) - m);
            }
            else{
                return (m*A + R - (pow(2, b) - m)) * -1;
            }
            //return m*A + R - (pow(2, b) - m);
        }
    }
    return 0;
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
