target=webDisk
src=$(wildcard *.cpp)
src_o=$(patsubst %.cpp, %.o, $(src))
include=-lpthread

CC=g++ -std=c++11

.PHONY:all
all:$(target)

$(target):$(src_o)
	$(CC) -o $@ $(filter $@.o, $(src_o)) $(include)


vpath %.cpp
%.o:%.cpp
	$(CC) -c $^

.PHONY:clean
clean:
	rm -f *.o $(target)