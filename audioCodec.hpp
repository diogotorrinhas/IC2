#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include <stdio.h>
#include <sndfile.h>
#include <vector>

/**
 * Class for encoding and decoding audio files.
 */

class audioCodec{
    private:
        char* filename;
        SF_INFO sfinfo;
        int ninput;
        std::vector<short> chs = {};
        std::vector<short> rn = {};
        
    public:
        audioCodec();

        /**
         * audioCodec Class Constructor
         * @param filename path to an audio file.
         */
        audioCodec(const char *filename);

         /**
         * Compress audio file
         * @param filename path to a file were to store the enconded value.
         * @param num value to choose the order of the predictor that will be use.
         * @param lossy lossless (0) and lossy (1)
         * @param shamt number of bits to be quantized
         */
        void compress(const char *fileDst, int num, bool lossy, int shamt);

        /**
         * Decompress audio file
         * @param fileSrc path to a file were the stored enconded value is.
         */
        void decompress(const char *fileSrc); 
        
        /**
         * Lossless Preditor 
         * @param vetSrc vector that contains all the samples of the audio file.
         */        
        void preditor(std::vector<short> vetSrc);

        /**
         * Lossy Preditor 
         * @param vetSrc vector that contains all the samples of the audio file.
         * @param shamt number of bits to be quantized
         */
        void preditorLossy(std::vector<short> vetSrc, int shamt);

};

#endif