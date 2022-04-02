--- drw.h.orig	2022-03-18 14:36:39.563938000 +0000
+++ drw.h	2022-03-18 14:41:34.696029000 +0000
@@ -20,6 +20,9 @@
 	Display *dpy;
 	int screen;
 	Window root;
+	Visual *visual;
+	unsigned int depth;
+	Colormap cmap;
 	Drawable drawable;
 	GC gc;
 	Clr *scheme;
@@ -27,7 +30,7 @@
 } Drw;
 
 /* Drawable abstraction */
-Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h);
+Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap);
 void drw_resize(Drw *drw, unsigned int w, unsigned int h);
 void drw_free(Drw *drw);
 
@@ -38,8 +41,8 @@
 void drw_font_getexts(Fnt *font, const char *text, unsigned int len, unsigned int *w, unsigned int *h);
 
 /* Colorscheme abstraction */
-void drw_clr_create(Drw *drw, Clr *dest, const char *clrname);
-Clr *drw_scm_create(Drw *drw, const char *clrnames[], size_t clrcount);
+void drw_clr_create(Drw *drw, Clr *dest, const char *clrname, unsigned int alpha);
+Clr *drw_scm_create(Drw *drw, const char *clrnames[], const unsigned int alphas[], size_t clrcount);
 
 /* Cursor abstraction */
 Cur *drw_cur_create(Drw *drw, int shape);
