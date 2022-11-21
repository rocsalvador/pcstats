flags=-std=c++17 -O2 -Wall

pcstats: main.o pcstats.o mainwindow.o
	g++ $(flags) -o pcstats main.o pcstats.o mainwindow.o -lncurses
	rm *.o

pcstats.o: pcstats.cc
	g++ $(flags) -c pcstats.cc
	
mainwindow.o: mainwindow.cc
	g++ $(flags) -c mainwindow.cc

main.o: main.cc
	g++ $(flags) -c main.cc

install: pcstats
	sudo cp pcstats /usr/bin

uninstall:
	sudo rm /usr/bin/pcstats

clean:
	rm pcstats
