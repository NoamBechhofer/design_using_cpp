CC = g++
CFLAGS = -Wall -Wpedantic -O2 -std=c++20
TARGET = lvv
SOURCE = ListVsVector.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)