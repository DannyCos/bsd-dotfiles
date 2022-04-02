--- st.h.orig	2022-03-20 13:41:13.343357000 +0000
+++ st.h	2022-03-20 13:38:35.579684000 +0000
@@ -11,7 +11,8 @@
 #define DIVCEIL(n, d)		(((n) + ((d) - 1)) / (d))
 #define DEFAULT(a, b)		(a) = (a) ? (a) : (b)
 #define LIMIT(x, a, b)		(x) = (x) < (a) ? (a) : (x) > (b) ? (b) : (x)
-#define ATTRCMP(a, b)		((a).mode != (b).mode || (a).fg != (b).fg || \
+#define ATTRCMP(a, b)		(((a).mode & (~ATTR_WRAP) & (~ATTR_LIGA)) != ((b).mode & (~ATTR_WRAP) & (~ATTR_LIGA)) || \
+				(a).fg != (b).fg || \
 				(a).bg != (b).bg)
 #define TIMEDIFF(t1, t2)	((t1.tv_sec-t2.tv_sec)*1000 + \
 				(t1.tv_nsec-t2.tv_nsec)/1E6)
@@ -33,6 +34,7 @@
 	ATTR_WRAP       = 1 << 8,
 	ATTR_WIDE       = 1 << 9,
 	ATTR_WDUMMY     = 1 << 10,
+	ATTR_LIGA       = 1 << 11,
 	ATTR_BOLD_FAINT = ATTR_BOLD | ATTR_FAINT,
 };
 
@@ -81,6 +83,8 @@
 void redraw(void);
 void draw(void);
 
+void kscrolldown(const Arg *);
+void kscrollup(const Arg *);
 void printscreen(const Arg *);
 void printsel(const Arg *);
 void sendbreak(const Arg *);
@@ -126,3 +130,4 @@
 extern unsigned int defaultfg;
 extern unsigned int defaultbg;
 extern unsigned int defaultcs;
+extern float alpha;
