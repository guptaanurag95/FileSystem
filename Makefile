CC = g++
CFLAGS  = -g

default: test

test:  disk.o file.o test.o
	$(CC) $(CFLAGS) -o test disk.o file.o test.o

test.o:  test.cpp disk.h file.h
	$(CC) $(CFLAGS) -c test.cpp

disk.o:  disk.cpp disk.h
	$(CC) $(CFLAGS) -c disk.cpp

file.o:  file.cpp file.h
	$(CC) $(CFLAGS) -c file.cpp

clean: 
	$(RM) test *.o *~