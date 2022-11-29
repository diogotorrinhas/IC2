// g++ BitStream.cpp GolombTest.cpp -o testGolomb

#include "Golomb.cpp"



int main(int argc, char* argv[]){
    //Get m and n
    int m, n;
    cout << "Insert m: ";
    cin >> m;
    cout << "Insert n: ";
    cin >> n;

    cout << endl  << "Encoding a ser feito..." << endl;
    //Initialize Golomb object for enconding a number
    Golomb gb("testGolomb.bin", 'e', m);
    //Encode number
    int size = gb.encode(n);
    //Close stream
    gb.close();
    
    //BitStream to read result
    BitStream bs("testGolomb.bin", 'r');
    char byte[size+1];    
    bs.readNbits(byte, size+1);
    //Print in Binary
    printf("Encoded Value: ");
    for (int i = 0; i < size; i++){
        printf("%d", byte[i]);
    }
    printf("\n");
    //Close BitStream
    bs.close();


    cout << endl << "Decoding a ser feito..." << endl;
    //Initialize Golomb object for decoding value
    Golomb g("testGolomb.bin", 'd', m);
    //Decode number
    cout << "Decoded Value: " << g.decode() << endl;

    //Close stream
    g.close();
    
    return 0;
}