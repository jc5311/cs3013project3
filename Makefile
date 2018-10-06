
all: addem.o life.o
	g++ -pthread addem.o -o addem
	g++ -pthread life.o -o life

addem.o: addem.cpp
	g++ -c addem.cpp

life.o: life.cpp
	g++ -c life.cpp

clean:
	rm *.o addem life