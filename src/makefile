pcstats: main.o pcstats.o main_window.o
	g++ -std=c++11 -o pcstats main.o pcstats.o main_window.o -lncurses
	rm *.o

pcstats.o: pcstats.cc
	g++ -std=c++11 -c pcstats.cc
	
main_window.o: main_window.cc
	g++ -std=c++11 -c main_window.cc

main.o: main.cc
	g++ -std=c++11 -c main.cc

clean:
	rm pcstats
