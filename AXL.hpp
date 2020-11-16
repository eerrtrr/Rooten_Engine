#ifndef AXL_H
#define AXL_H


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>


namespace Rooten{

	//......................CLASS DEFINITIONS......................//
	//WINAXL
	class Winaxl{
	private:
		Display* display;
		Window window;
		int screen;
		GC gc;
		unsigned int x, y, border_width, width, height;
		unsigned long border;
	  	unsigned long background;
		Colormap cmap;
		Atom close;
	  	bool exist;

	public:
		//CONSTRUCTOR - DECONSTRUCTOR
		Winaxl(unsigned int x, unsigned int y, unsigned int border_width, unsigned int width, unsigned int height, std::string name);
		~Winaxl();


		//GETTER
		Window getWindow();
		Display* getDisplay();
		int getScreen();
		GC getGC();
		unsigned int getX();
		unsigned int getY();
		unsigned int getW();
		unsigned int getH();
		bool getStatus();
		Atom getSignal();

		//SETTER
		void setX(unsigned int x);
		void setY(unsigned int y);
		void setW(unsigned int w);
		void setH(unsigned int h);
		void setStatus(bool status);
		void setSignal(Atom signal);


		//TOOLS
		void setFullScreen();
		void pollEvents();
		void setColor(unsigned long color);
		unsigned long addColor(int r, int g, int b);
	};



	//POINTS
	class Points{
	private:
		XPoint* points;
		unsigned int nbr;

	public:
		Points();
		~Points();

		XPoint* getPoints();
		unsigned int getNbrPoints();

		void drawPoint(Winaxl* win, unsigned int x, unsigned int y);
		void drawPoints(Winaxl* win, Points* pts);
		Points* createPointList(unsigned int nbr, short coords[][2]);
	};



	//LINES
	class Lines{
	private:
		XPoint* lines;
		unsigned int nbr;

	public:
		Lines();
		~Lines();
		void drawLine(Winaxl* win, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
		void drawLines(Winaxl* win, Points* pts);
	};




	//......................FUNCTIONS PROTOTYPES......................//
	void drawSqare(Winaxl* win, unsigned int x, unsigned int y, unsigned int length, bool fill);
	void drawRectangle(Winaxl* win, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool fill);
	void drawCircle(Winaxl* win, unsigned int x, unsigned int y, unsigned int radius, bool fill);
	void drawElipse(Winaxl* win, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int angle1, unsigned int angle2, bool fill);
}

#endif
