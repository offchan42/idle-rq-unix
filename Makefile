all:
	gcc server.c -o server
	gcc client.c -o client

clean:
	$(RM) server client
