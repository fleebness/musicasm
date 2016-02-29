CC = g++
DEBUG = -g
CFLAGS = -Wall -C $(DEBUG)
LFLAGS = -Wall $(DEBUG) -lportaudio -lrt -lm -lasound -pthread

musicasm : musicasm.cpp
	$(CC) -std=c++11 musicasm.cpp $(LFLAGS) $(OBJS) -o musicasm

clean :
	rm musicasm

