#include "audioCodec.hpp"
#include "Golomb.cpp"
#include <iostream>
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include <vector>

using namespace std;

audioCodec::audioCodec(){}

audioCodec::audioCodec(const char *filename){
    SNDFILE *infile;
    int readcount;
    short ch[2];


	if (! (infile = sf_open (filename, SFM_READ, &sfinfo))) {
        cout << "File doesn't exists" << endl;
        exit(EXIT_FAILURE);
	}

    
    while ((readcount = (int) sf_readf_short (infile, ch, 1)) > 0) {
        //printf("%hn", ch);
        chs.push_back(ch[0]); //channel 0
        chs.push_back(ch[1]); //channel 1  
    }
    
	sf_close (infile) ; 
}

//calcular a media, ir buscar p, e tirar o valor do m atraves da formula de golomb
//Perguntas:
//1-Qual é o resultado que temos de obter no exercicio 4 e 5? O ficheiro de audio de saida tem de ser igual ao ficheiro de audio de entrada, usando o lossless
//2-Para o preditor temos aquelas 4 equaçoes, temos de passar como parametro um numero para escolhermos qual dessas usar? Sim
//3-Tendo o preditor e o golomb como se faz a compressao do ficheiro de audio? Determina-se o m e dps usamos a função do encode do golomb
//4- O que é inter-channel prediction? Ver o q está comentado no predict lossless

void audioCodec::compress(const char *fileDst, int num, bool lossy, int shamt) {
    ninput = num;
    if (lossy){
        preditorLossy(chs, shamt);
    }else{
        preditor(chs);
    }
    
    cout << "Start encoding..." << endl;

    //Calculos do Golomb
    Golomb g(fileDst, 'e', 0);

    double soma = 0;
    double m = 0;
    double media = 0;
    for(int i = 0; i < rn.size();i++){
        soma = soma + g.fold(rn[i]);    //no golomb apenas passam inteiros positivos
    }
    
    //calcular a media
    media = soma/rn.size();
    
    m = (int) ceil(-1/(log2(media/(media+1))));
    //printf("%f", m);

    g.setM(m);
    g.encodeM(m);
    g.encodeHeaderSound(sfinfo.frames, sfinfo.samplerate, sfinfo.channels, sfinfo.format, lossy);

    if(lossy){
        g.encondeShamt(shamt);
    }

    for(int i = 0; i < rn.size(); i++){
        g.encode(rn[i]);
    }
    g.close();

}

void audioCodec::decompress(const char *fileSrc) {
    cout << "start decoding..." << endl;

    Golomb g(fileSrc, 'd', 0);

    int m = g.decodeM();

    g.setM(m);

    int decodeInfo[5];
    g.decodeHeaderSound(decodeInfo);

    //Variaveis vectors
    vector<short> resultChs, resultMid, resultSide, resultXMid, resultXSide, resultXN, resultChapeuXm, resultChapeuXs;

    if (decodeInfo[0] == 1){ //lossy
        int shamt = g.decodeShamt();
        //frames*format
        for(int i = 0; i < decodeInfo[1]*decodeInfo[4]; i++){ 
            resultChs.push_back(g.decode() << shamt);
        }
    }else{ //lossless
        for(int i = 0; i < decodeInfo[1]*decodeInfo[4]; i++){
            resultChs.push_back(g.decode());
        }
    }

    for(int i = 0; i < resultChs.size()-1; i= i+2){
        resultMid.push_back(resultChs[i]);    //l -> MID channel
        resultSide.push_back(resultChs[i+1]);  //r -> SIDE channel
    }

    g.close();

    //Voltar a fazer o msm "processo" do preditor para achar o Xn (mensagem inicial) xn = rn + xChapeuN!
    if(ninput == 1){
        resultChapeuXm.push_back(0);
        resultChapeuXs.push_back(0);
        resultXMid.push_back(resultMid[0]);
        resultXSide.push_back(resultSide[0]);
        resultXN.push_back(resultMid[0]);
        resultXN.push_back(resultSide[0]);
        for(int i = 1; i < resultMid.size(); i++){
            resultChapeuXm.push_back((int)resultXMid[i-1]);
            resultChapeuXs.push_back((int)resultXSide[i-1]);
            //xn = rn + xnChapeu
            resultXMid.push_back((short) resultMid[i] + resultChapeuXm[i]);
            resultXSide.push_back((short) resultSide[i] + resultChapeuXs[i]);
            resultXN.push_back(resultXMid[i]);
            resultXN.push_back(resultXSide[i]);
        }
    }else if (ninput==2){
        for(int i = 0; i < 2; i++){
            resultChapeuXm.push_back(0);
            resultChapeuXs.push_back(0);
            resultXMid.push_back(resultMid[i]);
            resultXSide.push_back(resultSide[i]);
            resultXN.push_back(resultXMid[i]);
            resultXN.push_back(resultXSide[i]);
        }
        for(int i = 2; i < resultMid.size(); i++) {
            resultChapeuXm.push_back((int) (2*resultXMid[i-1] - resultXMid[i-2]));
            resultChapeuXs.push_back((int) (2*resultXSide[i-1] - resultXSide[i-2]));
            //xn = rn + xnChapeu
            resultXMid.push_back((short) resultMid[i] + resultChapeuXm[i]);
            resultXSide.push_back((short) resultSide[i] + resultChapeuXs[i]);
            resultXN.push_back(resultXMid[i]);
            resultXN.push_back(resultXSide[i]);
        }
    }else{
        for(int i = 0; i < 3; i++) {
            resultChapeuXm.push_back(0);
            resultChapeuXs.push_back(0);
            resultXMid.push_back(resultMid[i]);
            resultXSide.push_back(resultSide[i]);
            resultXN.push_back(resultXMid[i]);
            resultXN.push_back(resultXSide[i]);
        }
        for(int i = 3; i < resultMid.size(); i++) {
            resultChapeuXm.push_back((int) (3*resultXMid[i-1] - 3*resultXMid[i-2] + resultXMid[i-3]));
            resultChapeuXs.push_back((int) (3*resultXSide[i-1] - 3*resultXSide[i-2] + resultXSide[i-3]));
            //xn = rn + xnChapeu
            resultXMid.push_back((short) resultMid[i] + resultChapeuXm[i]);
            resultXSide.push_back((short) resultSide[i] + resultChapeuXs[i]);
            resultXN.push_back(resultXMid[i]);
            resultXN.push_back(resultXSide[i]);
        }
    }

    SF_INFO sfinfoOut;

    sfinfoOut.frames = decodeInfo[1];
    sfinfoOut.samplerate = decodeInfo[2];
    sfinfoOut.format = decodeInfo[3];
    sfinfoOut.channels = decodeInfo[4];

    SNDFILE* outFile = sf_open("out.wav", SFM_WRITE, &sfinfoOut);
    sf_count_t count = sf_write_short(outFile, &resultXN[0], resultXN.size());
    sf_write_sync(outFile);
    sf_close(outFile);

}


void audioCodec::preditor(vector<short> vetSrc) {
    //side tem de ser apenas a diferença [L-R]
    //substituir o left pelo SIDE e o right pelo MID (guiao passado), para ser inter-channel prediction
    vector<short> mid; //l
    vector<short> side;    //r
    for(int i = 0; i < chs.size()-1; i+=2){
        mid.push_back((chs[i]+chs[i+1])/2);
        side.push_back(chs[i]-chs[i+1]);
    }

    vector<short> xnl, xnr;

    if(ninput == 1) {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0) {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else {
                xnl.push_back(mid[i-1]);
                xnr.push_back(side[i-1]);
            }
            rn.push_back(mid[i]-xnl[i]);
            rn.push_back(side[i]-xnr[i]);
        }
    }
    else if(ninput == 2) {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0 || i == 1) {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else {
                xnl.push_back(2*mid[i-1] - mid[i-2]);
                xnr.push_back(2*side[i-1] - side[i-2]);
            }
            rn.push_back(mid[i]-xnl[i]);
            rn.push_back(side[i]-xnr[i]);
        }
    }
    else {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0 || i == 1 || i == 2) {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else {
                xnl.push_back(3*mid[i-1] - 3*mid[i-2] + mid[i-3]);
                xnr.push_back(3*side[i-1] - 3*side[i-2] + side[i-3]);
            }
            rn.push_back(mid[i]-xnl[i]);
            rn.push_back(side[i]-xnr[i]);
        }
    }
}

void audioCodec:: preditorLossy(vector<short> vetSrc, int shamt) {
    //mudar tb para SIDE e MID em vez de left e right
    vector<short> mid;     
    vector<short> side;    
    for(int i = 0; i < chs.size()-1; i+=2){
        mid.push_back((chs[i]+chs[i+1])/2);
        side.push_back(chs[i]-chs[i+1]);
    }

    vector<int> xnr, xnl;
    
    if(ninput == 1) {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0) {
                xnr.push_back(0);
                xnl.push_back(0);
            }
            else {
                xnl.push_back(mid[i-1]);
                xnr.push_back(side[i-1]);
            }
            rn.push_back(((mid[i]-xnl[i]) >> shamt)); //esquecer alguns bits
            rn.push_back(((side[i]-xnr[i]) >> shamt)); //esquecer alguns bits
            //Alterar tb a sequencia original para os resultados não serem "diferentes"
            //Esquerda -> xn = rn + xnl
            //Direita -> xn = rn + xnr
            //"Decoder" dentro de um encoder
            mid[i] = (rn[2*i] << shamt) + xnl[i];
            side[i] = (rn[2*i+1] << shamt) + xnr[i];
        }
    }
    else if(ninput == 2) {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0 || i == 1) {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else {
                xnl.push_back(2*mid[i-1] - mid[i-2]);
                xnr.push_back(2*side[i-1] - side[i-2]);
            }
            rn.push_back(((mid[i]-xnl[i]) >> shamt)); //esquecer alguns bits
            rn.push_back(((side[i]-xnr[i]) >> shamt)); //esquecer alguns bits
            //Alterar tb a sequencia original para os resultados não serem "diferentes"
            //Esquerda -> xn = rn + xnl
            //Direita -> xn = rn + xnr
            //"Decoder" dentro de um encoder
            mid[i] = (rn[2*i] << shamt) + xnl[i];
            side[i] = (rn[2*i+1] << shamt) + xnr[i];
        }
    }
    else {
        for(int i = 0; i < mid.size(); i++) {
            if(i == 0 || i == 1 || i == 2) {
                xnl.push_back(0);
                xnr.push_back(0);
            }
            else {
                xnl.push_back(3*mid[i-1] - 3*mid[i-2] + mid[i-3]);
                xnr.push_back(3*side[i-1] - 3*side[i-2] + side[i-3]);
            }
            rn.push_back(((mid[i]-xnl[i]) >> shamt)); //esquecer alguns bits
            rn.push_back(((side[i]-xnr[i]) >> shamt)); //esquecer alguns bits
            //Alterar tb a sequencia original para os resultados não serem "diferentes"
            //Esquerda -> xn = rn + xnl
            //Direita -> xn = rn + xnr
            //"Decoder" dentro de um encoder
            mid[i] = (rn[2*i] << shamt) + xnl[i];
            side[i] = (rn[2*i+1] << shamt)  + xnr[i];
        }
    }
}

