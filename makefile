all: UdodenkoN_proj2

run:
	./UdodenkoN_proj2.exe

UdodenkoN_proj2: UdodenkoN_proj2.c
	gcc -o UdodenkoN_proj2.exe UdodenkoN_proj2.c -lpthread -lrt

clean:
	rm -rf *.exe *.txt