// g++ test_codecimg.cpp -o test_codecimg  $(pkg-config --libs --cflags opencv4) -I/usr/include/python3.8 -lpython3.8
#include "Codecim.cpp"

int main(int argc, char* argv[]){

    //Command line arguments processing
    if(argc != 2){
        cout << "\033[1;31mError: Following argument is required <input filename>\033[0m" << endl;
        return 0; 
    }
    char *inputfile = argv[1];

    Codecim c(inputfile);
    
    c.compress("compressed_img.bin");
    printf("... compress done ...\n\n");

    c.decompress("compressed_img.bin", "outdecompressed.jpeg");
    printf("... decompress done ...\n\n");

    return 0;
}