CC = g++

SRC_DIR := src/
OBJ_DIR := obj/
BIN_DIR := bin/
INC_DIR := include/
HIST_DIR := bin/hist/

all: ${BIN_DIR}client ${BIN_DIR}server
./bin/client: src/*.cpp include/*.hpp
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/client src/client_run.cpp src/client.cpp  src/communication_manager.cpp src/notification_manager.cpp src/client_interface.cpp src/profile.cpp src/RW_monitor.cpp src/profile_manager.cpp  -lpthread -lncurses  

./bin/server: src/*.cpp include/*.hpp
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/server src/server_run.cpp src/server.cpp src/client.cpp src/client_interface.cpp src/communication_manager.cpp src/notification_manager.cpp src/profile.cpp src/RW_monitor.cpp src/profile_manager.cpp  -lpthread -lncurses
 

clean:
	rm -f bin/server && rm -f bin/client
