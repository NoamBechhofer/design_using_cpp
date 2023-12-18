UNAME_S := $(shell uname -s)

CC = g++
CFLAGS = -g -Wall -Wpedantic -O2 -std=c++20
TARGETS = lvv test_n csv
# SOURCES = lvv.cpp test_n.cpp csv.cpp
LIBS :=
ifeq ($(UNAME_S),Linux)
    LIBS += -lpthread
endif

all: $(TARGETS)

lvv: lvv.cpp
	$(CC) $(CFLAGS) -o lvv lvv.cpp $(LIBS)

csv: csv.cpp
	$(CC) $(CFLAGS) -o csv csv.cpp $(LIBS)

test_n: test_n.cpp
	$(CC) $(CFLAGS) -o test_n test_n.cpp $(LIBS)

clean:
	rm -f $(TARGETS)