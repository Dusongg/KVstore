CC = g++
FLAGS = -std=c++20
SRCS = kvstore.cc server.cc
TARGET = kvserver
OBJS = $(SRCS:.cc=.o)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(FLAGS) 
%.o: %.c
	$(CC) $(FLAGS) -c $@ -o $^ 


.PHONY:clean
clean:
	rm -rf $(TARGET) $(OBJS)