CC=gcc
GC=g++

CXX_INCLUDE=../
CXX_TESTGEN=cxxtestgen
CFLAGS=-c -Wall
LDFLAGS=

all: BMMessageTest CobsFrameTest

BMMessageTest: BMMessageTest.cpp crc.o BMMessage.o
	$(GC) -o BMMessageTest -I$(CXX_INCLUDE) BMMessageTest.cpp crc.o BMMessage.o

BMMessageTest.cpp:
	$(CXX_TESTGEN) --error-printer -o BMMessageTest.cpp BMMessageTest.h

BMMessage.o:
	$(GC) $(CFLAGS) -I$(CXX_INCLUDE) ../BMMessage.cpp 

CobsFrameTest: CobsFrameTest.cpp cobs.o CobsFrame.o
	$(GC) -o CobsFrameTest -I$(CXX_INCLUDE) CobsFrameTest.cpp cobs.o CobsFrame.o

CobsFrameTest.cpp:
	$(CXX_TESTGEN) --error-printer -o CobsFrameTest.cpp CobsFrameTest.h

CobsFrame.o:
	$(GC) $(CFLAGS) -I$(CXX_INCLUDE) ../CobsFrame.cpp 

crc.o:
	$(GC) $(CFLAGS) -I$(CXX_INCLUDE) ../crc.cpp

cobs.o:
	$(CC) $(CFLAGS) -I$(CXX_INCLUDE) ../cobs.c


test: all
	./BMMessageTest
	./CobsFrameTest

clean:
	rm -rf *.o BMMessageTest CobsFrameTest
	rm -rf *.cpp


