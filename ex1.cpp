// comando de compilação: g++ ex1.cpp -o ex1 $(pkg-config --libs --cflags opencv4)

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
    //Argumentos de linha de comando
    if(argc != 3){
        cout << "\033[1;31mError: Usage sintax is ./p4 <input filename> <output filename>\033[0m" << endl;
        return 0;
    }
    char *inputimage = argv[1];
    char *outputimage = argv[2];

    //Ler Imagem
    Mat image = imread(inputimage);

    //Verificar se imagem existe
    if(image.empty()){
        cout << "\033[1;31mError: Could not open image\033[0m" << endl;
        return 0;
    }

    //Criar objeto da imagem para output
    Mat output(image.rows, image.cols, image.type());

    //Acessar aos pixeis da imagem e copiar para a nova
    for(int i=0; i < image.rows; i++){
        for(int j=0 ; j < image.cols; j++)
            output.ptr<Vec3b>(i)[j] = Vec3b(image.ptr<Vec3b>(i)[j][0], image.ptr<Vec3b>(i)[j][1], image.ptr<Vec3b>(i)[j][2]);
    }

    //Salvar a imagem de output
    imwrite(outputimage ,output); 

    return 0;

}
