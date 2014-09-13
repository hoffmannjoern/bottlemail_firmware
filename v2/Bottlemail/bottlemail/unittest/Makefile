CC=g++
CXX_INCLUDE=../
CXX_TESTGEN=cxxtestgen
CFLAGS=-c -Wall
LDFLAGS=

all: BMMessageTest

BMMessageTest: BMMessageTest.cpp crc.o BMMessage.o
	$(CC) -o BMMessageTest -I$(CXX_INCLUDE) BMMessageTest.cpp crc.o BMMessage.o

BMMessageTest.cpp:
	$(CXX_TESTGEN) --error-printer -o BMMessageTest.cpp BMMessageTest.hpp

BMMessage.o:
	$(CC) $(CFLAGS) -I$(CXX_INCLUDE) ../BMMessage.cpp 

crc.o:
	$(CC) $(CFLAGS) -I$(CXX_INCLUDE) ../crc.cpp

cobs.o:
	$(CC) $(CFLAGS) -I$(CXX_INCLUDE) ../cobs.cpp

clean:
	rm -rf *.o BMMessageTest


