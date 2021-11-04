FLAGS=-g -D_REENTRANT
HEADERS=network.h server.h requests.h resource.h meetup.h
OBJECTS=network.o myserver.o resource.o meetup.o
LIBS=-lpthread -lrt
CC=gcc

myserver: $(OBJECTS)
	$(CC) -o myserver $(OBJECTS) $(LIBS) $(FLAGS)

example: example.o resource.o
	$(CC) -o example example.o resource.o

myserver.o: myserver.c $(HEADERS)
	$(CC) $(FLAGS) -c myserver.c

network.o: network.c $(HEADERS)
	$(CC) $(FLAGS) -c network.c 

meetup.o: meetup.c $(HEADERS)
	$(CC) $(FLAGS) -c meetup.c 

resource.o: resource.c $(HEADERS)
	$(CC) $(FLAGS) -c resource.c 

example.o: example.c resource.h
	$(CC) -c example.c 

all: myserver 

clean:
	-rm $(OBJECTS) myserver
