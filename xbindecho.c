#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static Display *dpy;
static Window root;

struct Bind {
	struct Bind *next;
	KeyCode kcode;
	char *str;
	unsigned int mods;
};

/* show usage */
static void
usage(void)
{
	(void)fprintf(stderr, "usage: xbindecho modifiers\n");
	exit(1);
}

/* call malloc checking for error */
static void *
emalloc(size_t size)
{
	void *p;

	if ((p = malloc(size)) == NULL)
		err(1, "malloc");
	return p;
}

/* call strdup checking for error */
static char *
estrdup(const char *s)
{
	char *t;

	if ((t = strdup(s)) == NULL)
		err(1, "strdup");
	return t;
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

/* create bindings from stdin */
static struct Bind *
getbinds(unsigned int modifiers)
{
	struct Bind *binds = NULL;
	struct Bind *b;
	unsigned int mods;
	char buf[BUFSIZ];
	char *kstr, *str;

	while (fgets(buf, sizeof buf, stdin) != NULL) {
		if (*buf && *buf == '\n')
			continue;
		kstr = str = NULL;
		kstr = strtok(buf, "\t\n");
		str = strtok(NULL, "\n");
		b = emalloc(sizeof *b);
		mods = modifiers;
		b->str = (str == NULL ? "" : estrdup(str));
		b->kcode = getkeycode(kstr, &mods);
		b->mods = mods;
		b->next = binds;
		binds = b;
	}
	if (binds == NULL)
		errx(1, "empty input");
	return binds;
}

/* grab keys and run event loop */
static void
xbindecho(struct Bind *binds)
{
	XEvent ev;
	struct Bind *b;

	for (b = binds; b; b = b->next) {
		XGrabKey(dpy, b->kcode, b->mods, root, True, GrabModeAsync, GrabModeAsync);
	}
	while (!XNextEvent(dpy, &ev)) {
		if (ev.type == KeyPress) {
			for (b = binds; b; b = b->next) {
				if (ev.xkey.keycode == b->kcode &&
				    ((ev.xkey.state & b->mods) == b->mods)) {
					puts(b->str);
					fflush(stdout);
					break;
				}
			}
		}
	}
}

/* xbindecho: echo string on keypresses */
int
main(int argc, char *argv[])
{
	unsigned int modifiers = 0;
	struct Bind *binds;

	argc--;
	argv++;
	if (argc != 1)
		usage();
	if ((dpy = XOpenDisplay(NULL)) == NULL)
		errx(1, "could not open display");
	root = DefaultRootWindow(dpy);
	modifiers = getmodifiers(argv[0]);
	binds = getbinds(modifiers);
	xbindecho(binds);
	return 0;
}
