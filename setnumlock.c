/* Install libxtst-dev, compile: gcc -L/usr/X11R6/lib -o setnumlock setnumlock.c -lXtst */
#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>

int main(void) {
	Display *display;
	if(!(display = XOpenDisplay(getenv("DISPLAY")))) {
		return fprintf(stderr, "unable to open display\n"), 11;
	}

	XTestFakeKeyEvent(display,77,1,0);

	XFlush(display);
	XCloseDisplay(display);

	return 0;
}

