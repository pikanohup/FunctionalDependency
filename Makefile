CPP = g++

## debug flag
DBG_ENABLE = 1

## source file path
SRC_PATH := .

## target exec file name
TARGET := tane

## get all source files
SRCS += $(wildcard $(SRC_PATH)/*.cpp)

## all .o based on all .c
OBJS := $(SRCS:.cpp=.o)

## used headers  file path
INCLUDE_PATH := .

## debug for debug info, when use gdb to debug
ifeq (1, ${DBG_ENABLE})
CFLAGS += -D_DEBUG -O0 -g -DDEBUG=1
endif

## get all include path
CFLAGS += $(foreach dir, $(INCLUDE_PATH), -I$(dir))

## c++11 support
CFLAGS += -std=c++11

all: build

build:
	$(CPP) -c $(CFLAGS) $(SRCS)
	$(CPP) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(OBJS) $(TARGET)