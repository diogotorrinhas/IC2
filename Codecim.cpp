#include "Codecim.hpp"
#include "Golomb.cpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


// logic
void predictor(Mat src, vector<int>& res);
Mat preditorInverse(vector<int>& res, int nrows, int ncols);
int Mideal(Golomb& g, vector<int>& resR, vector<int>& resG, vector<int>& resB);
int preditorJPEG1(int a);
int preditorJLS(int a, int b, int c);
int preditorJPEG4(int a, int b, int c);
int max(int num1, int num2);
int min(int num1, int num2);

Codecim::Codecim(){}

Codecim::Codecim(const char *filename){
    //Vec3b val;

    // val[0] = 0;   //B
    // val[1] = 0;   //G
    // val[2] = 255; //R

    Mat img = imread(filename, IMREAD_COLOR);
    if(img.empty()){
        throw new runtime_error("could not make mat");
    }
    split(img, rgb); //Dividir a imagem RBG em 3 canais
    // this->filename = (char*) filename;

    //  for(int i=0; i < img.rows; i++){
    //     for(int j=0 ; j < img.cols; j++)
    //         img.at<cv::Vec3b>(i,j)=val;
    // }
}

void Codecim::compress(const char *fileDst){
    // printf("started compress...\n");
    
    vector<int> resR, resG, resB;
    predictor(rgb[2], resR);
    predictor(rgb[1], resG);
    predictor(rgb[0], resB);


    int m = 0;
    Golomb g = Golomb(fileDst, 'e', m);
    m = Mideal(g, resR, resG, resB);

    g.setM(m);

    g.encodeM(m);
    g.encode(rgb[2].cols);
    g.encode(rgb[2].rows);

    for(int i = 0; i < rgb[2].cols * rgb[2].rows; i++){
        g.encode(resR[i]);
    }
    for(int i = 0; i < rgb[1].cols * rgb[1].rows; i++){
        g.encode(resG[i]);
    }
    for(int i = 0; i < rgb[0].cols * rgb[0].rows; i++){
        g.encode(resB[i]);
    }
    g.close();
}

void predictor(Mat src, vector<int>& res){
    int a,b,c;
    for (int lines = 0; lines < src.rows; lines++){
        for(int colunas = 0; colunas < src.cols; colunas++){
            if(colunas == 0 and lines == 0){    //Canto superior esquerdo
                a = 0;
                b = 0;
                c = 0;
            }else if(colunas > 0 and lines == 0){ //linha de cima
                a = src.at<uchar>(0,colunas-1);
                b = 0;
                c = 0;
            
            }else if(colunas == 0 and lines > 0){  // coluna mais à esquerda
                a = 0;
                b = src.at<uchar>(lines-1,0);
                c = 0; 
            }else{
                a = src.at<uchar>(lines, colunas-1);
                b = src.at<uchar>(lines-1,colunas);
                c = src.at<uchar>(lines-1, colunas-1);
            }
            //rn = xn - ^xn
            res.push_back(src.at<uchar>(lines, colunas) - preditorJLS(a, b, c));
        }
    }


}

void Codecim::decompress(const char *fileSrc){
    decompress(fileSrc, filename);
}

void Codecim::decompress(const char *fileSrc, const char *fileDst){
    // printf("started decompress...\n");
    Golomb g = Golomb(fileSrc, 'd', 0);

    int m = g.decodeM();
    g.setM(m);

    int ncols = g.decode();
    int nrows = g.decode();

    vector<int> resR, resG, resB;

    for(int i = 0; i < ncols*nrows; i++){
        resR.push_back(g.decode());
    }
    for(int i = 0; i < (ncols)*(nrows); i++){
        resG.push_back(g.decode());
    }
    for(int i = 0; i < (ncols)*(nrows); i++){
        resB.push_back(g.decode());
    }

    g.close();

    
    Mat R = Mat(nrows, ncols, CV_8UC1);
    Mat G = Mat(nrows, ncols, CV_8UC1);
    Mat B = Mat(nrows, ncols, CV_8UC1);

    

    R = preditorInverse(resR, rgb[2].rows, rgb[2].cols);
    G = preditorInverse(resG, rgb[1].rows, rgb[1].cols);
    B = preditorInverse(resB, rgb[0].rows, rgb[0].cols);

   

    std::vector<cv::Mat> array_to_merge;
    array_to_merge.push_back(B);
    array_to_merge.push_back(G);
    array_to_merge.push_back(R);

    Mat mImg(nrows,ncols, CV_8UC3);
    //cv::Mat mImg;

    merge(array_to_merge, mImg);

    imwrite(fileDst, mImg);

    
}

Mat preditorInverse(vector<int>& res, int nrows, int ncols){
    Mat img (nrows, ncols, CV_8UC1);
    int a,b,c;
    
    for (int lines = 0; lines < nrows; lines++){
        for(int colunas = 0; colunas < ncols; colunas++){
            if(colunas == 0 and lines == 0){    //Canto superior esquerdo
                a = 0;
                b = 0;
                c = 0;
            }else if(colunas > 0 and lines == 0){ //linha de cima
                a = (int)img.at<uchar>(0,colunas-1);
                b = 0;
                c = 0;
            
            }else if(colunas == 0 and lines > 0){  // coluna mais à esquerda
                a = 0;
                b = (int)img.at<uchar>(lines-1,0);
                c = 0; 
            }else{
                a = (int)img.at<uchar>(lines, colunas-1);
                b = (int)img.at<uchar>(lines-1,colunas);
                c = (int)img.at<uchar>(lines-1, colunas-1);
            }
            //xn = rn + ^xn
            img.at<uchar>(lines,colunas) = (uchar) ((int)res.at(lines*ncols + colunas) + preditorJLS(a,b,c));
        }
    }
    
    return img;

}

int Mideal(Golomb& g, vector<int>& resR, vector<int>& resG, vector<int>& resB){
   map<int, int> aux;
    double soma;
    soma = 0;
    double media = 0;

    for(auto i : resR){
        aux[i]++;
    } 
    for(auto i : resG){
        aux[i]++;
    } 
    for(auto i : resB){
        aux[i]++;
    } 

    int samples = 0;
    for(auto i : aux)
        samples = samples + i.second;

    for(auto i : aux)
        soma = soma + g.fold(i.first * i.second);

    media = soma / samples;

    

    return ceil(-1 / log2(media / (media + 1)));
}



int preditorJPEG1(int a){
    return a;
}

int preditorJLS(int a, int b, int c){
    if (c >= max(a, b))
        return min(a, b);
    else if (c < min(a, b))
        return max(a, b);
    else
        return a + b - c;
}

int preditorJPEG4(int a, int b, int c){
    return a + b - c;
}


int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}


int min(int num1, int num2) 
{
    return (num1 > num2 ) ? num2 : num1;
}