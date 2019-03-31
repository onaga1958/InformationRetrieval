FLAGS=-Wno-write-strings -Ofast -std=c++11 -msse -msse2 -msse3 -mfpmath=sse -mpopcnt

all:
	g++ $(FLAGS) calc_group_stats.cpp -o bin.bin

second:
	g++ $(FLAGS) calc_group_sizes.cpp -o bin.bin

clean:
	rm -rf *.o bin.bin
