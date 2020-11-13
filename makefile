Rooten:main.o Logger.o AXL.o
	g++ -o exe main.o Logger.o AXL.o -L/usr/X11/lib -lX11

main.o: main.cpp Logger.hpp AXL.hpp
	g++ -c -Wall main.cpp

Logger.o: Logger.cpp Logger.hpp
	g++ -c -Wall Logger.cpp

AXL.o : AXL.cpp AXL.hpp
	g++ -c -Wall AXL.cpp

rm :
	rm *.o
