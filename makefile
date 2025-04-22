CC=gcc
CFLAGS=-g -Wall -std=c99
CCLINK=$(CC)
RM= rm -rf *.o libgrades.so

all: libgrades.so prog.exe

libgrades.so: grades.o
	$(CCLINK) -shared grades.o -o libgrades.so liblinked-list.so

grades.o: grades.c grades.h linked-list.h
	$(CCLINK) $(CFLAGS) -c -fpic grades.c
	
clean:
	$(RM)
