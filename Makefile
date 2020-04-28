CC = gcc
# CFLAG = -D SHOWMSG -Wall -std=c99
CFLAG = -Wall -std=c99

TARGET = scheduler
OBJS = main.o util.o process.o schedule.o

all: $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -o $(TARGET) 

main.o: main.c util.h process.h schedule.h
	$(CC) $(CFLAG) -c $< -o $@

util.o: util.c util.h
	$(CC) $(CFLAG) -c $< -o $@

process.o: process.c process.h util.h
	$(CC) $(CFLAG) -c $< -o $@

schedule.o: schedule.c process.h schedule.h
	$(CC) $(CFLAG) -c $< -o $@


clear: 
	rm *.o scheduler