#include <iostream>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    //Argumentos de linha de comando
    if(argc != 4){
        cout << "\033[1;31mError: Usage sintax is ./p4 <input filename> <output filename> <rotation(90,180,270)>\033[0m" << endl;
        return 0;
    }
    char *inputimage = argv[1];
    char *outputimage = argv[2];
    char *typee = argv[3];

    string tipo = typee;

    //Ler Imagem
    Mat image = imread(inputimage);

    //Verificar se imagem existe
    if(image.empty()){
        cout << "\033[1;31mError: Could not open image\033[0m" << endl;
        return 0;
    }

    if(tipo == "90"){
        transpose(image, image);        //rotate 90
        flip(image, image, +1);
    }
    if(tipo == "180"){                  //rotate 180
        flip(image, image, -1);
    }
    if(tipo == "270"){                  //rotate 270
        transpose(image, image);
        flip(image, image, 0);
    }

    //Salvar a imagem de output
    imwrite(outputimage ,image); 
    return 0;
}
