pcstats: main.o pcstats.o
	g++ -std=c++11 -o pcstats main.o pcstats.o
	rm *.o

pcstats.o: pcstats.cc
	g++ -std=c++11 -c pcstats.cc

main.o: main.cc
	g++ -std=c++11 -c main.cc

clean:
	rm pcstats
