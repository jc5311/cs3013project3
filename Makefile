
output: main.o
	g++ -pthread main.o -o main

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o main