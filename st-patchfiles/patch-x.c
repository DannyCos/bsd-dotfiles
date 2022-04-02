--- x.c.orig	2022-03-20 13:41:23.922492000 +0000
+++ x.c	2022-03-20 13:38:35.580091000 +0000
@@ -19,6 +19,7 @@
 #include "arg.h"
 #include "st.h"
 #include "win.h"
+#include "hb.h"
 
 /* types used in config.h */
 typedef struct {
@@ -105,6 +106,7 @@
 	XSetWindowAttributes attrs;
 	int scr;
 	int isfixed; /* is fixed geometry? */
+	int depth; /* bit depth */
 	int l, t; /* left and top offset */
 	int gm; /* geometry mask */
 } XWindow;
@@ -243,6 +245,7 @@
 static double usedfontsize = 0;
 static double defaultfontsize = 0;
 
+static char *opt_alpha = NULL;
 static char *opt_class = NULL;
 static char **opt_cmd  = NULL;
 static char *opt_embed = NULL;
@@ -736,7 +739,7 @@
 
 	XFreePixmap(xw.dpy, xw.buf);
 	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h,
-			DefaultDepth(xw.dpy, xw.scr));
+			xw.depth);
 	XftDrawChange(xw.draw, xw.buf);
 	xclear(0, 0, win.w, win.h);
 
@@ -796,6 +799,13 @@
 			else
 				die("could not allocate color %d\n", i);
 		}
+
+	/* set alpha value of bg color */
+	if (opt_alpha)
+		alpha = strtof(opt_alpha, NULL);
+	dc.col[defaultbg].color.alpha = (unsigned short)(0xffff * alpha);
+	dc.col[defaultbg].pixel &= 0x00FFFFFF;
+	dc.col[defaultbg].pixel |= (unsigned char)(0xff * alpha) << 24;
 	loaded = 1;
 }
 
@@ -1046,6 +1056,9 @@
 void
 xunloadfonts(void)
 {
+	/* Clear Harfbuzz font cache. */
+	hbunloadfonts();
+
 	/* Free the loaded fonts in the font cache.  */
 	while (frclen > 0)
 		XftFontClose(xw.dpy, frc[--frclen].font);
@@ -1118,12 +1131,24 @@
 	Window parent;
 	pid_t thispid = getpid();
 	XColor xmousefg, xmousebg;
+	XWindowAttributes attr;
+	XVisualInfo vis;
 
 	if (!(xw.dpy = XOpenDisplay(NULL)))
 		die("can't open display\n");
 	xw.scr = XDefaultScreen(xw.dpy);
-	xw.vis = XDefaultVisual(xw.dpy, xw.scr);
 
+	if (!(opt_embed && (parent = strtol(opt_embed, NULL, 0)))) {
+		parent = XRootWindow(xw.dpy, xw.scr);
+		xw.depth = 32;
+	} else {
+		XGetWindowAttributes(xw.dpy, parent, &attr);
+		xw.depth = attr.depth;
+	}
+
+	XMatchVisualInfo(xw.dpy, xw.scr, xw.depth, TrueColor, &vis);
+	xw.vis = vis.visual;
+
 	/* font */
 	if (!FcInit())
 		die("could not init fontconfig.\n");
@@ -1132,7 +1157,7 @@
 	xloadfonts(usedfont, 0);
 
 	/* colors */
-	xw.cmap = XDefaultColormap(xw.dpy, xw.scr);
+	xw.cmap = XCreateColormap(xw.dpy, parent, xw.vis, None);
 	xloadcols();
 
 	/* adjust fixed window geometry */
@@ -1152,19 +1177,15 @@
 		| ButtonMotionMask | ButtonPressMask | ButtonReleaseMask;
 	xw.attrs.colormap = xw.cmap;
 
-	if (!(opt_embed && (parent = strtol(opt_embed, NULL, 0))))
-		parent = XRootWindow(xw.dpy, xw.scr);
 	xw.win = XCreateWindow(xw.dpy, parent, xw.l, xw.t,
-			win.w, win.h, 0, XDefaultDepth(xw.dpy, xw.scr), InputOutput,
+			win.w, win.h, 0, xw.depth, InputOutput,
 			xw.vis, CWBackPixel | CWBorderPixel | CWBitGravity
 			| CWEventMask | CWColormap, &xw.attrs);
 
 	memset(&gcvalues, 0, sizeof(gcvalues));
 	gcvalues.graphics_exposures = False;
-	dc.gc = XCreateGC(xw.dpy, parent, GCGraphicsExposures,
-			&gcvalues);
-	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h,
-			DefaultDepth(xw.dpy, xw.scr));
+	xw.buf = XCreatePixmap(xw.dpy, xw.win, win.w, win.h, xw.depth);
+	dc.gc = XCreateGC(xw.dpy, xw.buf, GCGraphicsExposures, &gcvalues);
 	XSetForeground(xw.dpy, dc.gc, dc.col[defaultbg].pixel);
 	XFillRectangle(xw.dpy, xw.buf, dc.gc, 0, 0, win.w, win.h);
 
@@ -1245,7 +1266,7 @@
 		mode = glyphs[i].mode;
 
 		/* Skip dummy wide-character spacing. */
-		if (mode == ATTR_WDUMMY)
+		if (mode & ATTR_WDUMMY)
 			continue;
 
 		/* Determine font for glyph if different from previous glyph. */
@@ -1352,6 +1373,9 @@
 		numspecs++;
 	}
 
+	/* Harfbuzz transformation for ligatures. */
+	hbtransform(specs, glyphs, len, x, y);
+
 	return numspecs;
 }
 
@@ -1501,15 +1525,18 @@
 }
 
 void
-xdrawcursor(int cx, int cy, Glyph g, int ox, int oy, Glyph og)
+xdrawcursor(int cx, int cy, Glyph g, int ox, int oy, Glyph og, Line line, int len)
 {
 	Color drawcol;
 
 	/* remove the old cursor */
 	if (selected(ox, oy))
 		og.mode ^= ATTR_REVERSE;
-	xdrawglyph(og, ox, oy);
 
+	/* Redraw the line where cursor was previously.
+	 * It will restore the ligatures broken by the cursor. */
+	xdrawline(line, 0, oy, len);
+
 	if (IS_SET(MODE_HIDE))
 		return;
 
@@ -2018,6 +2045,9 @@
 	ARGBEGIN {
 	case 'a':
 		allowaltscreen = 0;
+		break;
+	case 'A':
+		opt_alpha = EARGF(usage());
 		break;
 	case 'c':
 		opt_class = EARGF(usage());
