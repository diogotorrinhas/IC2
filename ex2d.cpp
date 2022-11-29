#include <iostream>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    //Argumentos de linha de comando
    if(argc != 4){
        cout << "\033[1;31mError: Usage sintax is ./p4 <input filename> <output filename> <intensity(example -50 or 50)>\033[0m" << endl;
        return 0;
    }
    char *inputimage = argv[1];
    char *outputimage = argv[2];
    
    int valor = atoi(argv[3]);

    //Ler Imagem
    Mat image = imread(inputimage);
    //Verificar se imagem existe
    if(image.empty()){
        cout << "\033[1;31mError: Could not open image\033[0m" << endl;
        return 0;
    }

   image.convertTo(image, -1, 1, valor); //increase or decrease value

    //Salvar a imagem de output
    imwrite(outputimage ,image); 
    return 0;
}
