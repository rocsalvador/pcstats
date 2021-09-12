flags=-std=c++17 -O2 -Wall

pcstats: main.o pcstats.o processes.o mainwindow.o
	g++ $(flags) -o pcstats main.o pcstats.o processes.o mainwindow.o -lncurses
	rm *.o

pcstats.o: pcstats.cc
	g++ $(flags) -c pcstats.cc
	
processes.o:
	g++ $(flags) -c processes.cc

mainwindow.o: mainwindow.cc
	g++ $(flags) -c mainwindow.cc

main.o: main.cc
	g++ $(flags) -c main.cc

install: pcstats
	sudo cp pcstats /usr/bin
	sudo cp ../packages/pcstats.desktop /usr/share/applications

uninstall:
	sudo rm /usr/bin/pcstats
	sudo rm /usr/share/applications/pcstats.desktop

clean:
	rm pcstats
