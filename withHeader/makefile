TAR= main

.PHONY: all clean

CC = g++
VERSION = -std=c++14
CFLAG = -c -Wall $(VERSION)


INC_PATH = ./
OBJ_PATH = ./objs/
SOURCE=$(wildcard *.cpp)
OBJ=$(patsubst %.cpp,$(OBJ_PATH)%.o,$(SOURCE))

HEADER = $(wildcard $(INC_PATH)*.h)


all:$(TAR)
	

$(TAR):$(OBJ)
	$(CC) -Wall $(VERSION) -I$(INC_PATH) -o $@  $^ -lpthread 


$(OBJ):$(OBJ_PATH)%.o:%.cpp $(HEADER)
	$(CC) $(CFLAG) -I$(INC_PATH) -o $@ $<

clean:
	rm -f $(TAR) $(OBJ)