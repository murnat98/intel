CC=g++
CFLAGS=-c -Wall -std=c++11
SOURCES=main.cpp program.cpp cpu.cpp

all: cpu clean

cpu: $(SOURCES:.cpp=.o) ; $(CC) $(SOURCES:.cpp=.o) -o cpu

program.o: program.cpp ; $(CC) $(CFLAGS) program.cpp

cpu.o: cpu.cpp ; $(CC) $(CFLAGS) cpu.cpp

main.o: main.cpp ; $(CC) $(CFLAGS) main.cpp

clean: ; rm -rf *.o