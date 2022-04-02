--- win.h.orig	2022-01-07 11:41:35.000000000 +0000
+++ win.h	2022-03-20 02:02:16.149699000 +0000
@@ -25,7 +25,7 @@
 
 void xbell(void);
 void xclipcopy(void);
-void xdrawcursor(int, int, Glyph, int, int, Glyph);
+void xdrawcursor(int, int, Glyph, int, int, Glyph, Line, int);
 void xdrawline(Line, int, int, int);
 void xfinishdraw(void);
 void xloadcols(void);
