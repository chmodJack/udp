
all:
	g++ -c -I.. ../udp.c
	g++ -c -I.. server.cpp
	g++ -c -I.. client.cpp
	g++ udp.o server.o -o server -lstdc++
	g++ udp.o client.o -o client -lstdc++
