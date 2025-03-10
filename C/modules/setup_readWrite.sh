mkdir -p bin
gcc -o obj/readWriteTensors.o -c -g -std=c99 src/readWriteTensorsFromStandardIO.c -I include/ -I ../../C/mempool/include/ -I ../../C/primitives/include/
gcc -o obj/testReadWriteTensors.o -c -g util/testStdIO.c -I ../../C/mempool/include/ -I ../../C/primitives/include/ -I include/ -I src/
gcc -o bin/testStdIO obj/mempool.o obj/tensor.o obj/createTensor.o obj/readWriteTensors.o obj/testReadWriteTensors.o