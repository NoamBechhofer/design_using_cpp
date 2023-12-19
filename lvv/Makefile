UNAME_S := $(shell uname -s)

CC = g++
CFLAGS = -g -Wall -Wpedantic -std=c++23
TARGET = lvv
SOURCE = lvv.cpp
LIBS :=
ifeq ($(UNAME_S),Linux)
    LIBS += -lpthread
endif

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)