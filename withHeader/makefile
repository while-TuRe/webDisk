# target=main
# src=$(wildcard *.cpp)
# src_o=$(patsubst %.cpp, %.o, $(src))

# include=-I./
# lib=-lpthread

# CC=g++ -std=c++11

# .PHONY:all
# all:$(target)

# $(target):$(src_o)
# 	$(CC) -o $@ $(filter $@.o, $(src_o)) $(lib) $(include)


# vpath %.cpp
# %.o:%.cpp
# 	$(CC) -c $^

# .PHONY:clean
# clean:
# 	rm -f *.o $(target)





target=main
src=$(wildcard *.cpp)
# include_src=myDaemon.cpp menu.cpp socket.cpp
# src+=$(include_src)
src_o=$(patsubst %.cpp, %.o, $(src))
include=-I./ -lpthread
other_src_dir=./

CC=g++ -std=c++11

.PHONY:all
all:$(target)

$(target):$(src_o)
	$(CC) -o $@ $(filter $@.o, $(src_o)) $(patsubst %.cpp, %.o, $(src)) $(include)

vpath %.cpp $(other_src_dir)
%.o:%.cpp
	$(CC) -c $^

.PHONY:clean
clean:
	rm -f *.o $(target)