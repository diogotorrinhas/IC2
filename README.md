# IC2
Information and Coding project 2

## Comandos de Compilação e Resultados


### Ex1:
#### Comandos de compilação:
> g++ ex1.cpp -o ex1 $(pkg-config --libs --cflags opencv4)

>./ex1 image1.jpeg out.jpeg

### Ex2a:
#### Comandos de compilação:
> g++ ex2.cpp -o ex2 $(pkg-config --libs --cflags opencv4)

>./ex2 image1.jpeg out.jpeg

### Ex2b:
#### Comandos de compilação:
> g++ ex2b.cpp -o ex2b $(pkg-config --libs --cflags opencv4)

>./ex2b image1.jpeg out.jpeg h 

>./ex2b image1.jpeg out.jpeg v

### Ex2c:
#### Comandos de compilação:
> g++ ex2c.cpp -o ex2c $(pkg-config --libs --cflags opencv4)

>./ex2c image1.jpeg out.jpeg 90

>./ex2c image1.jpeg out.jpeg 180

>./ex2c image1.jpeg out.jpeg 270

### Ex2d:
#### Comandos de compilação:

> g++ ex2d.cpp -o ex2d $(pkg-config --libs --cflags opencv4)

>./ex2d image1.jpeg out.jpeg 100

>./ex2d image1.jpeg out.jpeg -100

### Ex3:
#### Comandos de compilação:

> g++ BitStream.cpp GolombTest.cpp -o testGolomb

>./testGolomb

### Ex4/5:
#### Comandos de compilação:

> g++ BitStream.cpp test_audioCodec.cpp -o test_audioCodec -lsndfile

> ./test_audioCodec sample.wav
