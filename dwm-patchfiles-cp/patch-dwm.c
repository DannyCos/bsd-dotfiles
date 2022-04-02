--- dwm.c.orig	2022-01-07 11:42:18.000000000 +0000
+++ dwm.c	2022-03-20 18:05:33.475349000 +0000
@@ -57,12 +57,14 @@
 #define TAGMASK                 ((1 << LENGTH(tags)) - 1)
 #define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)
 
+#define OPAQUE                  0xffU
+
 /* enums */
 enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
 enum { SchemeNorm, SchemeSel }; /* color schemes */
 enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
        NetWMFullscreen, NetActiveWindow, NetWMWindowType,
-       NetWMWindowTypeDialog, NetClientList, NetLast }; /* EWMH atoms */
+       NetWMWindowTypeDialog, NetClientList, NetWMWindowsOpacity, NetLast }; /* EWMH atoms */
 enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
 enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
        ClkClientWin, ClkRootWin, ClkLast }; /* clicks */
@@ -95,6 +97,7 @@
 	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
 	Client *next;
 	Client *snext;
+	double opacity;
 	Monitor *mon;
 	Window win;
 };
@@ -138,6 +141,7 @@
 	const char *title;
 	unsigned int tags;
 	int isfloating;
+	double opacity;
 	int monitor;
 } Rule;
 
@@ -149,6 +153,8 @@
 static void attach(Client *c);
 static void attachstack(Client *c);
 static void buttonpress(XEvent *e);
+static void changeopacity(const Arg *arg);
+static void toggleopacity(const Arg *arg);
 static void checkotherwm(void);
 static void cleanup(void);
 static void cleanupmon(Monitor *mon);
@@ -163,12 +169,12 @@
 static Monitor *dirtomon(int dir);
 static void drawbar(Monitor *m);
 static void drawbars(void);
-static void enternotify(XEvent *e);
 static void expose(XEvent *e);
 static void focus(Client *c);
 static void focusin(XEvent *e);
 static void focusmon(const Arg *arg);
 static void focusstack(const Arg *arg);
+static void hideborder(const Arg *arg);
 static Atom getatomprop(Client *c, Atom prop);
 static int getrootptr(int *x, int *y);
 static long getstate(Window w);
@@ -182,9 +188,9 @@
 static void mappingnotify(XEvent *e);
 static void maprequest(XEvent *e);
 static void monocle(Monitor *m);
-static void motionnotify(XEvent *e);
 static void movemouse(const Arg *arg);
 static Client *nexttiled(Client *c);
+static void opacity(Client *c, double opacity);
 static void pop(Client *);
 static void propertynotify(XEvent *e);
 static void quit(const Arg *arg);
@@ -233,6 +239,7 @@
 static int xerror(Display *dpy, XErrorEvent *ee);
 static int xerrordummy(Display *dpy, XErrorEvent *ee);
 static int xerrorstart(Display *dpy, XErrorEvent *ee);
+static void xinitvisual();
 static void zoom(const Arg *arg);
 
 /* variables */
@@ -250,13 +257,11 @@
 	[ConfigureRequest] = configurerequest,
 	[ConfigureNotify] = configurenotify,
 	[DestroyNotify] = destroynotify,
-	[EnterNotify] = enternotify,
 	[Expose] = expose,
 	[FocusIn] = focusin,
 	[KeyPress] = keypress,
 	[MappingNotify] = mappingnotify,
 	[MapRequest] = maprequest,
-	[MotionNotify] = motionnotify,
 	[PropertyNotify] = propertynotify,
 	[UnmapNotify] = unmapnotify
 };
@@ -269,6 +274,11 @@
 static Monitor *mons, *selmon;
 static Window root, wmcheckwin;
 
+static int useargb = 0;
+static Visual *visual;
+static int depth;
+static Colormap cmap;
+
 /* configuration, allows nested code to access above variables */
 #include "config.h"
 
@@ -288,6 +298,7 @@
 	/* rule matching */
 	c->isfloating = 0;
 	c->tags = 0;
+	c->opacity = defaultopacity;
 	XGetClassHint(dpy, c->win, &ch);
 	class    = ch.res_class ? ch.res_class : broken;
 	instance = ch.res_name  ? ch.res_name  : broken;
@@ -300,6 +311,7 @@
 		{
 			c->isfloating = r->isfloating;
 			c->tags |= r->tags;
+			c->opacity = r->opacity;
 			for (m = mons; m && m->num != r->monitor; m = m->next);
 			if (m)
 				c->mon = m;
@@ -425,16 +437,23 @@
 
 	click = ClkRootWin;
 	/* focus monitor if necessary */
-	if ((m = wintomon(ev->window)) && m != selmon) {
+	if ((m = wintomon(ev->window)) && m != selmon
+	    && (focusonwheel || (ev->button != Button4 && ev->button != Button5))) {
 		unfocus(selmon->sel, 1);
 		selmon = m;
 		focus(NULL);
 	}
 	if (ev->window == selmon->barwin) {
 		i = x = 0;
-		do
+		unsigned int occ = 0;
+		for(c = m->clients; c; c=c->next)
+                        occ |= c->tags;
+                do {
+                        /* Do not reserve space for vacant tags */
+                        if (!(occ & 1 << i || m->tagset[m->seltags] & 1 << i))
+                                continue;
 			x += TEXTW(tags[i]);
-		while (ev->x >= x && ++i < LENGTH(tags));
+		} while (ev->x >= x && ++i < LENGTH(tags));
 		if (i < LENGTH(tags)) {
 			click = ClkTagBar;
 			arg.ui = 1 << i;
@@ -445,8 +464,8 @@
 		else
 			click = ClkWinTitle;
 	} else if ((c = wintoclient(ev->window))) {
-		focus(c);
-		restack(selmon);
+		if (focusonwheel || (ev->button != Button4 && ev->button != Button5))
+			focus(c);
 		XAllowEvents(dpy, ReplayPointer, CurrentTime);
 		click = ClkClientWin;
 	}
@@ -457,6 +476,35 @@
 }
 
 void
+changeopacity(const Arg *arg)
+{
+	if (!selmon->sel)
+		return;
+	selmon->sel->opacity+=arg->f;
+	if(selmon->sel->opacity > 1.0)
+		selmon->sel->opacity = 1.0;
+
+	if(selmon->sel->opacity < minopacity)
+		selmon->sel->opacity = minopacity;
+
+	opacity(selmon->sel, selmon->sel->opacity);
+}
+
+void
+toggleopacity(const Arg *arg)
+{
+	if (!selmon->sel)
+		return;
+
+	if(selmon->sel->opacity < 1.0)
+		selmon->sel->opacity = 1.0;
+	else
+		selmon->sel->opacity = defaultopacity;
+
+	opacity(selmon->sel, selmon->sel->opacity);
+}
+
+void
 checkotherwm(void)
 {
 	xerrorxlib = XSetErrorHandler(xerrorstart);
@@ -697,6 +745,7 @@
 drawbar(Monitor *m)
 {
 	int x, w, tw = 0;
+	int tlpad;
 	int boxs = drw->fonts->h / 9;
 	int boxw = drw->fonts->h / 6 + 2;
 	unsigned int i, occ = 0, urg = 0;
@@ -719,13 +768,14 @@
 	}
 	x = 0;
 	for (i = 0; i < LENGTH(tags); i++) {
+		/* Do not draw vacant tags */
+                if(!(occ & 1 << i || m->tagset[m->seltags] & 1 << i))
+                        continue;
 		w = TEXTW(tags[i]);
 		drw_setscheme(drw, scheme[m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeNorm]);
 		drw_text(drw, x, 0, w, bh, lrpad / 2, tags[i], urg & 1 << i);
-		if (occ & 1 << i)
-			drw_rect(drw, x + boxs, boxs, boxw, boxw,
-				m == selmon && selmon->sel && selmon->sel->tags & 1 << i,
-				urg & 1 << i);
+		if (ulineall || m->tagset[m->seltags] & 1 << i) /* if there are conflicts, just move these lines directly underneath both 'drw_setscheme' and 'drw_text' :) */
+			drw_rect(drw, x + ulinepad, bh - ulinestroke - ulinevoffset, w - (ulinepad * 2), ulinestroke, 1, 0);
 		x += w;
 	}
 	w = blw = TEXTW(m->ltsymbol);
@@ -735,9 +785,11 @@
 	if ((w = m->ww - tw - x) > bh) {
 		if (m->sel) {
 			drw_setscheme(drw, scheme[m == selmon ? SchemeSel : SchemeNorm]);
-			drw_text(drw, x, 0, w, bh, lrpad / 2, m->sel->name, 0);
+			tlpad = MAX((m->ww - ((int)TEXTW(m->sel->name) - lrpad)) / 2 - x, lrpad / 2);
+			drw_text(drw, x, 0, w, bh, tlpad, m->sel->name, 0);
 			if (m->sel->isfloating)
-				drw_rect(drw, x + boxs, boxs, boxw, boxw, m->sel->isfixed, 0);
+				drw_rect(drw, x + boxs + tlpad - lrpad / 2, boxs, 
+						boxw, boxw, m->sel->isfixed, 0);
 		} else {
 			drw_setscheme(drw, scheme[SchemeNorm]);
 			drw_rect(drw, x, 0, w, bh, 1, 1);
@@ -756,25 +808,6 @@
 }
 
 void
-enternotify(XEvent *e)
-{
-	Client *c;
-	Monitor *m;
-	XCrossingEvent *ev = &e->xcrossing;
-
-	if ((ev->mode != NotifyNormal || ev->detail == NotifyInferior) && ev->window != root)
-		return;
-	c = wintoclient(ev->window);
-	m = c ? c->mon : wintomon(ev->window);
-	if (m != selmon) {
-		unfocus(selmon->sel, 1);
-		selmon = m;
-	} else if (!c || c == selmon->sel)
-		return;
-	focus(c);
-}
-
-void
 expose(XEvent *e)
 {
 	Monitor *m;
@@ -859,6 +892,12 @@
 	}
 }
 
+void
+hideborder(const Arg *arg)
+{
+	XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColBg].pixel);
+}
+
 Atom
 getatomprop(Client *c, Atom prop)
 {
@@ -1042,6 +1081,7 @@
 		c->mon = selmon;
 		applyrules(c);
 	}
+	opacity(c, c->opacity);
 
 	if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
 		c->x = c->mon->mx + c->mon->mw - WIDTH(c);
@@ -1120,23 +1160,6 @@
 }
 
 void
-motionnotify(XEvent *e)
-{
-	static Monitor *mon = NULL;
-	Monitor *m;
-	XMotionEvent *ev = &e->xmotion;
-
-	if (ev->window != root)
-		return;
-	if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
-		unfocus(selmon->sel, 1);
-		selmon = m;
-		focus(NULL);
-	}
-	mon = m;
-}
-
-void
 movemouse(const Arg *arg)
 {
 	int x, y, ocx, ocy, nx, ny;
@@ -1204,6 +1227,18 @@
 }
 
 void
+opacity(Client *c, double opacity)
+{
+	if(opacity >= 0 && opacity <= 1) {
+		unsigned long real_opacity[] = { opacity * 0xffffffff };
+		XChangeProperty(dpy, c->win, netatom[NetWMWindowsOpacity], XA_CARDINAL,
+				32, PropModeReplace, (unsigned char *)real_opacity,
+				1);
+	} else
+		XDeleteProperty(dpy, c->win, netatom[NetWMWindowsOpacity]);
+}
+
+void
 pop(Client *c)
 {
 	detach(c);
@@ -1545,7 +1580,8 @@
 	sw = DisplayWidth(dpy, screen);
 	sh = DisplayHeight(dpy, screen);
 	root = RootWindow(dpy, screen);
-	drw = drw_create(dpy, screen, root, sw, sh);
+	xinitvisual();
+	drw = drw_create(dpy, screen, root, sw, sh, visual, depth, cmap);
 	if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
 		die("no fonts could be loaded.");
 	lrpad = drw->fonts->h;
@@ -1566,6 +1602,7 @@
 	netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
 	netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
 	netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
+	netatom[NetWMWindowsOpacity] = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
 	/* init cursors */
 	cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
 	cursor[CurResize] = drw_cur_create(drw, XC_sizing);
@@ -1573,7 +1610,7 @@
 	/* init appearance */
 	scheme = ecalloc(LENGTH(colors), sizeof(Clr *));
 	for (i = 0; i < LENGTH(colors); i++)
-		scheme[i] = drw_scm_create(drw, colors[i], 3);
+		scheme[i] = drw_scm_create(drw, colors[i], alphas[i], 3);
 	/* init bars */
 	updatebars();
 	updatestatus();
@@ -1677,28 +1714,31 @@
 void
 tile(Monitor *m)
 {
-	unsigned int i, n, h, mw, my, ty;
+	unsigned int i, n, h, mw, my, ty, ns;
 	Client *c;
 
 	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
 	if (n == 0)
 		return;
 
-	if (n > m->nmaster)
+	if (n > m->nmaster) {
 		mw = m->nmaster ? m->ww * m->mfact : 0;
-	else
+		ns = m->nmaster > 0 ? 2 : 1;
+	} else {
 		mw = m->ww;
-	for (i = my = ty = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
+		ns = 1;
+	}
+	for(i = 0, my = ty = gappx, c = nexttiled(m->clients); c; c = nexttiled(c->next), i++)
 		if (i < m->nmaster) {
-			h = (m->wh - my) / (MIN(n, m->nmaster) - i);
-			resize(c, m->wx, m->wy + my, mw - (2*c->bw), h - (2*c->bw), 0);
-			if (my + HEIGHT(c) < m->wh)
-				my += HEIGHT(c);
+			h = (m->wh - my) / (MIN(n, m->nmaster) - i) - gappx;
+                        resize(c, m->wx + gappx, m->wy + my, mw - (2*c->bw) - gappx*(5-ns)/2, h - (2*c->bw), False);
+			if (my + HEIGHT(c) + gappx < m->wh)
+				my += HEIGHT(c) + gappx;
 		} else {
-			h = (m->wh - ty) / (n - i);
-			resize(c, m->wx + mw, m->wy + ty, m->ww - mw - (2*c->bw), h - (2*c->bw), 0);
-			if (ty + HEIGHT(c) < m->wh)
-				ty += HEIGHT(c);
+			h = (m->wh - ty) / (n - i) - gappx;
+                        resize(c, m->wx + mw + gappx/ns, m->wy + ty, m->ww - mw - (2*c->bw) - gappx*(5-ns)/2, h - (    2*c->bw), False);
+                        if (ty + HEIGHT(c) + gappx < m->wh)
+				ty += HEIGHT(c) + gappx;
 		}
 }
 
@@ -1810,16 +1850,18 @@
 	Monitor *m;
 	XSetWindowAttributes wa = {
 		.override_redirect = True,
-		.background_pixmap = ParentRelative,
+		.background_pixel = 0,
+		.border_pixel = 0,
+		.colormap = cmap,
 		.event_mask = ButtonPressMask|ExposureMask
 	};
 	XClassHint ch = {"dwm", "dwm"};
 	for (m = mons; m; m = m->next) {
 		if (m->barwin)
 			continue;
-		m->barwin = XCreateWindow(dpy, root, m->wx, m->by, m->ww, bh, 0, DefaultDepth(dpy, screen),
-				CopyFromParent, DefaultVisual(dpy, screen),
-				CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
+		m->barwin = XCreateWindow(dpy, root, m->wx, m->by, m->ww, bh, 0, depth,
+		                          InputOutput, visual,
+		                          CWOverrideRedirect|CWBackPixel|CWBorderPixel|CWColormap|CWEventMask, &wa);
 		XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
 		XMapRaised(dpy, m->barwin);
 		XSetClassHint(dpy, m->barwin, &ch);
@@ -2114,6 +2156,43 @@
 {
 	die("dwm: another window manager is already running");
 	return -1;
+}
+
+void
+xinitvisual()
+{
+	XVisualInfo *infos;
+	XRenderPictFormat *fmt;
+	int nitems;
+	int i;
+
+	XVisualInfo tpl = {
+		.screen = screen,
+		.depth = 32,
+		.class = TrueColor
+	};
+	long masks = VisualScreenMask | VisualDepthMask | VisualClassMask;
+
+	infos = XGetVisualInfo(dpy, masks, &tpl, &nitems);
+	visual = NULL;
+	for(i = 0; i < nitems; i ++) {
+		fmt = XRenderFindVisualFormat(dpy, infos[i].visual);
+		if (fmt->type == PictTypeDirect && fmt->direct.alphaMask) {
+			visual = infos[i].visual;
+			depth = infos[i].depth;
+			cmap = XCreateColormap(dpy, root, visual, AllocNone);
+			useargb = 1;
+			break;
+		}
+	}
+
+	XFree(infos);
+
+	if (! visual) {
+		visual = DefaultVisual(dpy, screen);
+		depth = DefaultDepth(dpy, screen);
+		cmap = DefaultColormap(dpy, screen);
+	}
 }
 
 void
