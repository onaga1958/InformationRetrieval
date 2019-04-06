FLAGS=-Wno-write-strings -Ofast -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -mpopcnt

all: encoding
	g++ $(FLAGS) experiment.cpp encoding.o -o exp.bin

test: encoding
	g++ $(FLAGS) test.cpp encoding.o -o test.bin

encoding:
	g++ $(FLAGS) -c encoding.cpp -o encoding.o

clean:
	rm -rf exp.bin test.bin encoding.o
