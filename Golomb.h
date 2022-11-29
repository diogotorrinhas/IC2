#define GOLOMB_H

#include "BitStream.h"
#include <math.h> 

using namespace std;

/**
 * Class for encoding and decoding values using golomb code.
 */
class Golomb {
    public:
        Golomb();

        /**
         * Golomb Class Constructor
         * @param filename Path to a file were to store the enconded value or to read a value to be decoded.
         * @param mode character 'e' for encoding a value 'd' for decoding a value.
         * @param mvalue value of m that will be used in golomb encoding and decoding .
         */
        Golomb(const char *filename, char mode, int mvalue);

        /**
         * Encode a number (positive or negative) using golomb code.
         * @param n Integer number, positive or negative to be encoded.
         * @return number of bits of the output encoded number.
         * */
        int encode(int n);

        /**
         * Decode a number using golomb code. It has no parameters because the value to be decoded is read from the file 
         * given when the constructor of the class Golomb is initialized.
         * @return the decoded value.
         * */
        int decode();

        /**
         * Convert an integer (positive or negative) into positive using folding tecnique
         * @param n integer number to be converted
         * @return converted value
         * */
        int fold(int n);


        /**
         * Convert a positive number into integer according to the folding tecnique
         * @param n integer number to be converted
         * @return converted value
         * */
        int unfold(int n);

        /**
         * Close the golomb stream.
         * */
        void close();

        /**
         * Encode the golomb parameter m in binary (write it in the binary file)
         * @param n integer value that represents the m
         * */
        void encodeM(int n);

        /**
         * Read in binary the value of the golomb m
         * @return an integer that represents the m
         * */
        int decodeM();

        /**
         * Set value of m
         * @param mi integer that represents the m
         * */
        void setM(int mi);

        /**
         * Encode in binary the header of a soundfile
         * @param nFrames number of frames of the audio file
         * @param sampleRate of the audio file
         * @param Channels of the audiofile
         * @param format of the audiofile
         * @param lossy boolean that indicates if coding option is lossy or lossless
         **/
        void encodeHeaderSound(int nFrames, int sampleRate, int Channels, int format, bool lossy);

        /**
         * Encode quantization step in binary to save it in the header of encoded file
         * @param shamt quantization step
         **/
        void encondeShamt(int shamt);

        /**
         * Decode quantization step
         * @return quantization step
         **/
        int decodeShamt();

        /**
         * Decode the binary header of the soundfile
         * @param arr integer array of size 5 that will contain codecopetion(lossy, lossless), nFrames, sampleRate, numChannels and format
         * 
         **/
        void decodeHeaderSound(int arr[]);

        /**
         * Encode mode image header
         * @param mode 0 -> lossless, 1 -> lossy
         * */
        void encodeMode(int mode);

        /**
         * Decode mode image header
         * @return mode  0 -> lossless, 1 -> lossy
         * */
        int decodeMode();

    private:
        BitStream Gfile; 
        int m; 
        int b;
};