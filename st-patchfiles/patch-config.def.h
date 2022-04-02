--- config.def.h.orig	2022-01-07 11:41:35.000000000 +0000
+++ config.def.h	2022-03-20 17:42:55.112633000 +0000
@@ -5,8 +5,8 @@
  *
  * font: see http://freedesktop.org/software/fontconfig/fontconfig-user.html
  */
-static char *font = "Liberation Mono:pixelsize=12:antialias=true:autohint=true";
-static int borderpx = 2;
+static char *font = "Hack:style=Regular:pixelsize=16:antialias=true:autohint=true";
+static int borderpx = 5;
 
 /*
  * What program is execed by st depends of these precedence rules:
@@ -93,35 +93,38 @@
  */
 unsigned int tabspaces = 8;
 
+/* bg opacity */
+float alpha = 0.85;
+
 /* Terminal colors (16 first used in escape sequence) */
 static const char *colorname[] = {
 	/* 8 normal colors */
-	"black",
-	"red3",
-	"green3",
-	"yellow3",
-	"blue2",
-	"magenta3",
-	"cyan3",
-	"gray90",
+	"#101010",
+	"#e84f4f",
+	"#b8d68c",
+	"#e1aa5d",
+	"#7dc1cf",
+	"#9b64fb",
+	"#6d878d",
+	"#dddddd",
 
 	/* 8 bright colors */
-	"gray50",
-	"red",
-	"green",
-	"yellow",
-	"#5c5cff",
-	"magenta",
-	"cyan",
-	"white",
+	"#404040",
+	"#d23d3d",
+	"#a0cf5d",
+	"#f39d21",
+	"#4e9fb1",
+	"#8542ff",
+	"#42717b",
+	"#dddddd",
 
 	[255] = 0,
 
 	/* more colors can be added after 255 to use with DefaultXX */
-	"#cccccc",
+	"#d7d0c7",
 	"#555555",
-	"gray90", /* default foreground colour */
-	"black", /* default background colour */
+	"#d7d0c7", /* default foreground colour */
+	"#070b17", /* default background colour */
 };
 
 
@@ -201,6 +204,8 @@
 	{ TERMMOD,              XK_Y,           selpaste,       {.i =  0} },
 	{ ShiftMask,            XK_Insert,      selpaste,       {.i =  0} },
 	{ TERMMOD,              XK_Num_Lock,    numlock,        {.i =  0} },
+	{ ShiftMask,            XK_Page_Up,     kscrollup,      {.i = -1} },
+	{ ShiftMask,            XK_Page_Down,   kscrolldown,    {.i = -1} },
 };
 
 /*
