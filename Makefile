CC = gcc
CFLAGS = -O0 -Wall -g -lpthread

Client:chat_client.c list.c file_IO.c broadcast.c
	$(CC) $^ -o $@ $(CFLAGS)

debug:chat_client.c list.c file_IO.c broadcast.c
	$(CC) $^ -o $@ $(CFLAGS) -DDEBUG

clean:
	$(RM) .*.sw? test debug *.o

.PHONY:all clean