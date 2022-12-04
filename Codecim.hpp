#ifndef CODECIM_H
#define CODECIM_H

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;


class Codecim{
    private:
        char* filename;
        Mat rgb[3];
        // ideal m

    public:
        Codecim();
        /**
         * Codecaud Class Constructor
         * @param filename path to an image file.
         * @param verb variable to print histograms, images and entropys
         */
        Codecim(const char *filename);
        /**
         * Compress an input file using lossless
         * @param fileDst path to the image file to be compressed
         */
        void compress(const char *fileDst);
        /**
         * Decompress an input audio file
         * @param fileSrc path to the image file to be decompressed
         */
        void decompress(const char *fileSrc);
        /**
         * Decompress an input audio file
         * @param fileSrc path to the image file to be decompressed
         * @param fileDst name to be saved in the ouput decompressed image file
         */
        void decompress(const char *fileSrc, const char *fileDst);
};

#endif