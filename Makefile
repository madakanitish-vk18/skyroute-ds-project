CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g -Imodel -Iview -Icontroller
SRC=main.c model/hashtable.c model/heap.c model/graph.c model/routing.c view/view.c controller/controller.c

skyroute: $(SRC)
	$(CC) $(CFLAGS) -o skyroute $(SRC)

run: skyroute
	./skyroute

clean:
	rm -f skyroute
