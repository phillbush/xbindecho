#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *dpy;
static Window root;

/* show usage */
static void
usage(void)
{
	(void)fprintf(stderr, "usage: xkeyecho modifiers key string\n");
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

/* get keycode from s; update *modifiers if keysym is uppercase */
static KeyCode
getkeycode(char *s, unsigned int *modifiers)
{
	KeySym ksym = NoSymbol;
	KeySym klower = NoSymbol;
	KeySym kupper = NoSymbol;
	KeyCode kcode = 0;

	if ((ksym = XStringToKeysym(s)) == NoSymbol)
		goto error;
	XConvertCase(ksym, &klower, &kupper);
	if (ksym != klower && ksym == kupper)
		*modifiers |= ShiftMask;
	if ((kcode = XKeysymToKeycode(dpy, ksym)) == 0)
		goto error;
	return kcode;
error:
	errx(1, "%s: unknown key", s);
	return 0;
}

/* grab key and run event loop */
static void
xkeyecho(KeyCode kcode, unsigned int modifiers, char *s)
{
	XEvent ev;

	XGrabKey(dpy, kcode, modifiers, root, True, GrabModeAsync, GrabModeAsync);
	while (!XNextEvent(dpy, &ev)) {
		if (ev.type == KeyPress) {
			puts(s);
			fflush(stdout);
		}
	}
}

/* xkeyecho: echo string on keypress */
int
main(int argc, char *argv[])
{
	unsigned int modifiers = 0;
	KeyCode kcode;

	argc--;
	argv++;
	if (argc != 3)
		usage();
	if ((dpy = XOpenDisplay(NULL)) == NULL)
		errx(1, "could not open display");
	root = DefaultRootWindow(dpy);
	modifiers = getmodifiers(argv[0]);
	kcode = getkeycode(argv[1], &modifiers);
	xkeyecho(kcode, modifiers, argv[2]);
	return 0;
}
