--- hb.h.orig	2022-03-20 02:02:16.149023000 +0000
+++ hb.h	2022-03-20 02:02:16.149085000 +0000
@@ -0,0 +1,6 @@
+#include <X11/Xft/Xft.h>
+#include <hb.h>
+#include <hb-ft.h>
+
+void hbunloadfonts();
+void hbtransform(XftGlyphFontSpec *, const Glyph *, size_t, int, int);
