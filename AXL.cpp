#include "AXL.hpp"


namespace Rooten{

	//......................DISPLAY STUFF......................//
	//CONSTRCTOR
	Winaxl::Winaxl(unsigned int x, unsigned int y, unsigned int border_width, unsigned int width, unsigned int height, std::string name){
		this->display = XOpenDisplay(NULL);
		if(this->display == NULL){
			fprintf(stderr, "Ca not create display\n");
			exit(1);
		}

	 	this->screen = DefaultScreen(this->display);
	 	this->x = x;		this->y = y;		this->border_width = border_width;
	 	this->width = width;		this->height = height;
	 	this->border = BlackPixel(this->display, this->screen);
	 	this->background = WhitePixel(this->display, this->screen);

	 	this->window = XCreateSimpleWindow(this->display, DefaultRootWindow(this->display), x, y, width, height, border_width, this->border, this->background);
		this->gc = XCreateGC(this->display, this->window, 0, NULL);
		this->cmap = XDefaultColormap(this->display, this->screen);
		XSetForeground(this->display, this->gc, BlackPixel(this->display, this->screen));
		XSelectInput(this->display, this->window, StructureNotifyMask | ExposureMask | KeyPressMask);

		XStoreName(this->display, this->window, name.c_str());

		this->exist = true;
		this->close = XInternAtom(this->display, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(this->display, this->window, &this->close, 1);

		XMapWindow(this->display, this->window);
		for(;;){
		    XEvent e;
		    XNextEvent(this->display, &e);
		    if (e.type == MapNotify)
			  break;
	    }
	}

	//DECONSTRUCTOR
	Winaxl::~Winaxl(){
		XDestroyWindow(this->display, this->window);
	  	XCloseDisplay(this->display);
		free((void*)(this->display));
	}


	//......................GETTER & SETTER......................//
	//GETTER
	Window Winaxl::getWindow(){
		return this->window;
	}

	Display* Winaxl::getDisplay(){
		return this->display;
	}

	GC Winaxl::getGC(){
		return this->gc;
	}

	int Winaxl::getScreen(){
		return this->screen;
	}

	unsigned int Winaxl::getX(){
		return this->x;
	}

	unsigned int Winaxl::getY(){
		return this->y;
	}

	unsigned int Winaxl::getW(){
		return this->width;
	}

	unsigned int Winaxl::getH(){
		return this->height;
	}

	bool Winaxl::getStatus(){
		return this->exist;
	}

	Atom Winaxl::getSignal(){
		return this->close;
	}


	//SETTER
	void Winaxl::setX(unsigned int x){
		this->x = x;
	}

	void Winaxl::setY(unsigned int y){
		this->y = y;
	}

	void Winaxl::setW(unsigned int w){
		this->width = w;
	}

	void Winaxl::setH(unsigned int h){
		this->height = h;
	}

	void Winaxl::setStatus(bool status){
		this->exist = status;
	}

	void Winaxl::setSignal(Atom signal){
		this->close = signal;
	}


	//......................TOOLS......................//
	//FULL SCREEN
	void Winaxl::setFullScreen(){
		Screen* temp = ScreenOfDisplay(this->getDisplay(), 0);
		XMoveResizeWindow(this->getDisplay(), this->getWindow(), 0, 0, temp->width, temp->height);
	}


	//Events management
	void Winaxl::pollEvents(){
		XEvent e;
	    XNextEvent(this->display, &e);

		switch (e.type){
			case Expose:

				break;

			case ButtonPress:

				break;

			case KeyPress:
                if(XLookupKeysym(&e.xkey, 0) == XK_q)
                    this->exist = false;
				break;

			case ClientMessage:
				if(e.xclient.data.l[0] == (long int)this->close)
					this->exist = false;
				break;

			case ConfigureNotify:
				this->x = e.xconfigure.x;		this->y = e.xconfigure.y;		this->width = e.xconfigure.width;		this->height = e.xconfigure.height;
			break;
		}
	}


	//COLORS
	unsigned long Winaxl::addColor(int r, int g, int b){
		XColor color;
		color.red = (r+1)*256-1;    color.green = (g+1)*256-1;    color.blue = (b+1)*256-1;
		color.flags = DoRed | DoGreen | DoBlue;
		if(XAllocColor(this->display, this->cmap, &color))
			return(color.pixel);
	    else{
		    printf("Warning: can't allocate requested colour\n");
		    return(BlackPixel(this->display, this->cmap));
	    }
	}

	void Winaxl::setColor(unsigned long color){
		XSetForeground(this->display, this->gc, color);
	}



	//----------------------------------------------------------------------------------------------------------------//



	//......................POINTS......................//
	//CONSTRCTOR
	Points::Points(){
	}

	//DECONSTRUCTOR
	Points::~Points(){
	}


	//......................GETTER & SETTER......................//
	//GETTER
	XPoint* Points::getPoints(){
		return this->points;
	}

	unsigned int Points::getNbrPoints(){
		return this->nbr;
	}


	//......................TOOLS......................//
	//Create list of points
	Points* Points::createPointList(unsigned int nbr, short coords[][2]){
		Points* pts = new Points;
		pts->nbr = nbr;
		XPoint* xpts = new XPoint[nbr];

		for(unsigned int i=0; i<nbr; i++){
			xpts[i].x = coords[i][0];
			xpts[i].y = coords[i][1];
		}

		pts->points = xpts;

		return pts;
	}

	void Points::drawPoint(Winaxl* win, unsigned int x, unsigned int y){
		XDrawPoint(win->getDisplay(), win->getWindow(), win->getGC(), x, y);
	}

	void Points::drawPoints(Winaxl* win, Points* pts){
		XDrawPoints(win->getDisplay(), win->getWindow(), win->getGC(), pts->points, pts->nbr, CoordModeOrigin);
	}



	//----------------------------------------------------------------------------------------------------------------//



	//......................LINES......................//
	//CONSTRCTOR
	Lines::Lines(){
	}

	//DECONSTRUCTOR
	Lines::~Lines(){
	}


	//......................GETTER & SETTER......................//


	//......................TOOLS......................//
	//Create list of points
	void Lines::drawLine(Winaxl* win, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2){
		XDrawLine(win->getDisplay(), win->getWindow(), win->getGC(), x1, y1, x2, y2);
	}

	void Lines::drawLines(Winaxl* win, Points* pts){
		XDrawLines(win->getDisplay(), win->getWindow(), win->getGC(), pts->getPoints(), pts->getNbrPoints(), CoordModeOrigin);
	}



	//----------------------------------------------------------------------------------------------------------------//



	//......................DRAWING FUNCTIONS......................//
	//SQUARE
	void drawSqare(Winaxl* win, unsigned int x, unsigned int y, unsigned int length, bool fill){
		if(fill)
			XFillRectangle(win->getDisplay(), win->getWindow(), win->getGC(), x, y, length, length);
		else
			XDrawRectangle(win->getDisplay(), win->getWindow(), win->getGC(), x, y, length, length);
	}

	//RECTANGLE
	void drawRectangle(Winaxl* win, unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool fill){
		if(fill)
			XFillRectangle(win->getDisplay(), win->getWindow(), win->getGC(), x, y, width, height);
		else
			XDrawRectangle(win->getDisplay(), win->getWindow(), win->getGC(), x, y, width, height);
	}

	//CIRCLE
	void drawCircle(Winaxl* win, unsigned int x, unsigned int y, unsigned int radius, bool fill){
		if(fill)
			XFillArc(win->getDisplay(), win->getWindow(), win->getGC(), x, y, radius, radius, 0, 23040);
		else
			XDrawArc(win->getDisplay(), win->getWindow(), win->getGC(), x, y, radius, radius, 0, 23040);
	}

	//ELIPSE
	void drawElipse(Winaxl* win, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int angle1, unsigned int angle2, bool fill){
		if(fill)
			XFillArc(win->getDisplay(), win->getWindow(), win->getGC(), x, y, width, height, angle1, angle2);
		else
			XDrawArc(win->getDisplay(), win->getWindow(), win->getGC(), x, y, width, height, angle1, angle2);
	}

	//Clean the window
	void cleanWindow(Winaxl* win){
		XClearWindow(win->getDisplay(), win->getWindow());
	}
}
