TARGET = EvdisTPC
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)
CXXFLAGS = $(ROOTCFLAGS) -Wall
CXXLIBS = $(ROOTLIBS)
CC = g++


OBJS =  main.o

.SUFFIXES: .cc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -m64 $(CXXLIBS) $(OBJS) -o $@

.c.o:
	$(CC) $(CXXFLAGS) -Wall -c $<

clean:
	rm -f $(TARGET) $(OBJS)

