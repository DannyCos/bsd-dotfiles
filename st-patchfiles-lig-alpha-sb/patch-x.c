--- x.c.orig	2022-03-20 02:00:34.870848000 +0000
+++ x.c	2022-03-20 02:02:16.150002000 +0000
@@ -19,6 +19,7 @@
 #include "arg.h"
 #include "st.h"
 #include "win.h"
+#include "hb.h"
 
 /* types used in config.h */
 typedef struct {
@@ -1055,6 +1056,9 @@
 void
 xunloadfonts(void)
 {
+	/* Clear Harfbuzz font cache. */
+	hbunloadfonts();
+
 	/* Free the loaded fonts in the font cache.  */
 	while (frclen > 0)
 		XftFontClose(xw.dpy, frc[--frclen].font);
@@ -1262,7 +1266,7 @@
 		mode = glyphs[i].mode;
 
 		/* Skip dummy wide-character spacing. */
-		if (mode == ATTR_WDUMMY)
+		if (mode & ATTR_WDUMMY)
 			continue;
 
 		/* Determine font for glyph if different from previous glyph. */
@@ -1369,6 +1373,9 @@
 		numspecs++;
 	}
 
+	/* Harfbuzz transformation for ligatures. */
+	hbtransform(specs, glyphs, len, x, y);
+
 	return numspecs;
 }
 
@@ -1518,14 +1525,17 @@
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
+
+	/* Redraw the line where cursor was previously.
+	 * It will restore the ligatures broken by the cursor. */
+	xdrawline(line, 0, oy, len);
 
 	if (IS_SET(MODE_HIDE))
 		return;
