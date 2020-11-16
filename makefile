CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -L/usr/X11/lib

Rooten: main.o Logger.o AXL.o Engine.o Platform.o VulkanRenderer.o
	g++ $(CFLAGS) -o exe main.o Logger.o AXL.o Engine.o Platform.o VulkanRenderer.o $(LDFLAGS)
	make rm

main.o: main.cpp Engine.hpp
	g++ $(CFLAGS) -c -Wall main.cpp

Logger.o: Logger.cpp Logger.hpp
	g++ $(CFLAGS) -c -Wall Logger.cpp

Engine.o: Engine.cpp Engine.hpp Platform.hpp Logger.hpp VulkanRenderer.hpp
	g++ $(CFLAGS) -c -Wall Engine.cpp

Platform.o: Platform.cpp Platform.hpp Engine.hpp AXL.hpp VulkanUtils.hpp
	g++ $(CFLAGS) -c -Wall Platform.cpp

VulkanRenderer.o: VulkanRenderer.cpp VulkanRenderer.hpp VulkanUtils.hpp Platform.hpp Logger.hpp
	g++ $(CFLAGS) -c -Wall VulkanRenderer.cpp

AXL.o : AXL.cpp AXL.hpp
	g++ $(CFLAGS) -c -Wall AXL.cpp

rm :
	rm *.o
