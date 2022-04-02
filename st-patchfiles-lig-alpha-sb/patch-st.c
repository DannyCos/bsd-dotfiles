--- st.c.orig	2022-01-07 11:41:35.000000000 +0000
+++ st.c	2022-03-20 12:25:10.657631000 +0000
@@ -2674,7 +2674,8 @@
 
 	drawregion(0, 0, term.col, term.row);
 	xdrawcursor(cx, term.c.y, term.line[term.c.y][cx],
-			term.ocx, term.ocy, term.line[term.ocy][term.ocx]);
+			term.ocx, term.ocy, term.line[term.ocy][term.ocx],
+			term.line[term.ocy], term.col);
 	term.ocx = cx;
 	term.ocy = term.c.y;
 	xfinishdraw();
