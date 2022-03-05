CC := gcc
CFLAGS := -Wall
TARGET := chimp_sim
	
.PHONY: all clean

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.c)
# Substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

# Remove all .o files and target
clean:
	rm -rf $(TARGET) *.o
