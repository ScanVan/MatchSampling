all:directory
	g++ -o bin/MatchSampling src/MatchSampling.cpp -O3 -std=c++11

clean:
	rm -f bin/*

directory:
	mkdir -p bin
