CC = g++
CFLAGS = -Wall -Wpedantic -O2 -std=c++20
TARGET = sm
SOURCE = SimpleMeasurement.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)