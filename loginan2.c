#include <X11/Xlib.h>
#include <stdio.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <sys/time.h>
#include <string.h>
long utime();
void draw(cairo_t*);
void main() {
	Display *disp = XOpenDisplay(NULL);
	cairo_t *g,*bg;
	cairo_surface_t *ximg,*img;
	img = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,640,480);
	bg = cairo_create(img);
	int s;
	if(disp == NULL) {
		printf("Display open failed.\n");
		return;
	}
	Atom WM_DELETE_WINDOW = XInternAtom(disp,"WM_DELETE_WINDOW",False);
	s = DefaultScreen(disp);
	int w = XCreateSimpleWindow(disp,RootWindow(disp,s),0,0,640,480,1,0,0);
	XMapWindow(disp,w);
	XSetWMProtocols(disp,w,&WM_DELETE_WINDOW,1);
	ximg = cairo_xlib_surface_create(disp,w,DefaultVisual(disp,s),640,480);
	cairo_xlib_surface_set_size(ximg,640,480);
	g = cairo_create(ximg);
	cairo_set_source_surface(g,img,0,0);
	long t1 = utime();
	while(1) {
		if(XPending(disp)) {
			XEvent e;
			XNextEvent(disp,&e);
			if(e.type == ClientMessage && e.xclient.data.l[0] == WM_DELETE_WINDOW) {break;}
		} else {
			if(t1 + 300 > utime()) {
				t1 = utime();
				draw(bg);
				cairo_paint(g);
			}
		}
	}
	cairo_destroy(g);
	cairo_surface_destroy(ximg);
	cairo_destroy(bg);
	cairo_surface_destroy(img);
	XDestroyWindow(disp,w);
	XCloseDisplay(disp);
}

long utime() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}


void draw(cairo_t *g) {
	char buffer[64];
	snprintf(buffer,64,"%ld",utime());
	cairo_set_source_rgb(g,0,0,0);
	cairo_paint(g);
	cairo_set_source_rgb(g,1,1,1);
	cairo_select_font_face(g,"Ubuntu Mono",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(g,24);
	cairo_move_to(g,0,30);
	cairo_show_text(g,buffer);
}
