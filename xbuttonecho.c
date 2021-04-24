#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *dpy;
static Window root;
static int rflag = 0;

/* show usage */
static void
usage(void)
{
	(void)fprintf(stderr, "usage: xbuttonecho [-r] modifiers button string\n");
	exit(1);
}

/* parse modifiers */
static unsigned int
getmodifiers(char *s)
{
	unsigned int modifiers = 0;

	while (*s) {
		switch (*s) {
		case 'C':
			modifiers |= ControlMask;
			break;
		case 'L':
			modifiers |= LockMask;
			break;
		case 'S':
			modifiers |= ShiftMask;
			break;
		case '1':
		case 'A':
			modifiers |= Mod1Mask;
			break;
		case '2':
			modifiers |= Mod2Mask;
			break;
		case '3':
			modifiers |= Mod3Mask;
			break;
		case '4':
		case 'W':
			modifiers |= Mod4Mask;
			break;
		case '5':
			modifiers |= Mod5Mask;
			break;
		default:
			break;
		}
		s++;
	}
	return modifiers;
}

/* parse button */
static unsigned int
getbutton(char *s)
{
	unsigned int button = Button3;

	switch (*s) {
	case '1':
		button = Button1;
		break;
	case '2':
		button = Button2;
		break;
	case '3':
		button = Button3;
		break;
	default:
		errx(1, "%s: unknown button", s);
		return 0;
	}
	return button;
}

/* grab button and run event loop */
static void
xbuttonecho(unsigned int button, unsigned int modifiers, char *s)
{
	XEvent ev;

	XGrabButton(dpy, button, AnyModifier, root, False, ButtonPressMask, GrabModeSync, GrabModeSync, None, None);
	while (!XNextEvent(dpy, &ev)) {
		if (ev.type == ButtonPress && ev.xbutton.button == button &&
		    (((ev.xbutton.state & modifiers) == modifiers) ||
		     (rflag && ev.xbutton.subwindow == None))) {
			puts(s);
			fflush(stdout);
		}
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
	}
}

/* xbuttonecho: echo string on buttonpress */
int
main(int argc, char *argv[])
{
	unsigned int modifiers = 0;
	unsigned int button = Button3;
	int ch;

	while ((ch = getopt(argc, argv, "r")) != -1) {
		switch (ch) {
		case 'r':
			rflag = 1;
			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 3)
		usage();
	if ((dpy = XOpenDisplay(NULL)) == NULL)
		errx(1, "could not open display");
	root = DefaultRootWindow(dpy);
	modifiers = getmodifiers(argv[0]);
	button = getbutton(argv[1]);
	xbuttonecho(button, modifiers, argv[2]);
	return 0;
}
