#include "audioCodec.cpp"
//g++ BitStream.cpp test_audioCodec.cpp -o test_audioCodec -lsndfile
int main(int argc, char* argv[]){

    if (argc != 2){
        cout << "\033[1;31mError: Following argument is required <input filename>\033[0m" << endl;
        return 0;
    }

    char* inputFile = argv[1];

    audioCodec aC(inputFile);

    int lossy = -1;
    while(!(lossy == 0 || lossy == 1)){
        cout << "Choose codec option (0 for lossless or 1 for lossy): ";
        cin >> lossy;
    }

    int predictor = 0;
    while(predictor < 1 || predictor > 3){
        cout << "Choose the predictor (1, 2 or 3): ";
        cin >> predictor;
    }

    int bits = -1;
    if(lossy == 1){
        while(bits < 0 || bits > 15){
            cout << "Choose quantization step (number of bits to remove): ";
            cin >> bits;
        }
    }

    aC.compress("compress.bin", predictor, lossy, bits);
    printf("... done encoding\n\n");

    aC.decompress("compress.bin");
    printf("... done decompress\n\n");

    return 0;
}