
output: main.o
	g++ -pthread main.o -o addem

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o addem