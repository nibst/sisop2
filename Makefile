./bin/main: src/*.cpp include/*.hpp
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/main src/profile.cpp src/RW_monitor.cpp src/profile_manager.cpp -lpthread 

.PHONY: clean run
clean:
	rm -f bin/main

run: ./bin/main
	cd bin && ./main