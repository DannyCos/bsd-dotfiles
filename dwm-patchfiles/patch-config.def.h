--- config.def.h.orig	2022-01-07 11:42:18.000000000 +0000
+++ config.def.h	2022-03-20 23:39:35.727033000 +0000
@@ -2,39 +2,63 @@
 
 /* appearance */
 static const unsigned int borderpx  = 1;        /* border pixel of windows */
+static const unsigned int gappx     = 15;       /* gap pixel between windows */
 static const unsigned int snap      = 32;       /* snap pixel */
 static const int showbar            = 1;        /* 0 means no bar */
 static const int topbar             = 1;        /* 0 means bottom bar */
+static const int horizpadbar        = 5;        /* horizontal padding for statusbar */
+static const int vertpadbar         = 0;        /* vertical padding for statusbar */
+static const int focusonwheel       = 0;        
+static const double defaultopacity  = 0.90;
+static const double minopacity      = 0.50;
 static const char *fonts[]          = { "monospace:size=10" };
 static const char dmenufont[]       = "monospace:size=10";
 static const char col_gray1[]       = "#222222";
 static const char col_gray2[]       = "#444444";
 static const char col_gray3[]       = "#bbbbbb";
 static const char col_gray4[]       = "#eeeeee";
+static const char col_fg[]          = "#f4f3f0";
+static const char col_bg[]          = "#151515";
+static const char col_white[]       = "#dddddd";
 static const char col_cyan[]        = "#005577";
+static const char col_darkblue[]    = "#194ba0";
+static const unsigned int baralpha  = 0x50;
+static const unsigned int borderalpha = OPAQUE;
 static const char *colors[][3]      = {
 	/*               fg         bg         border   */
-	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
-	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
+	[SchemeNorm] = { col_fg,    col_bg,    col_bg },
+	[SchemeSel]  = { col_fg,    col_gray2, col_darkblue },
+	[SchemeTitle]= { col_fg,    col_bg,    col_bg },
 };
+static const unsigned int alphas[][3]      = {
+	/*               fg      bg        border     */
+	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
+	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
+};
 
 /* tagging */
 static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
 
+static const unsigned int ulinepad     = 5; /* horizontal padding between the underland and the tag */
+static const unsigned int ulinestroke  = 1; /* thickness / height of the underline */
+static const unsigned int ulinevoffset = 1; /* how far above the bottom of the bar the line should appear */
+static const int ulineall              = 0; /* 1 to show underline on all tags, 0 for just the active one */
+
 static const Rule rules[] = {
 	/* xprop(1):
 	 *	WM_CLASS(STRING) = instance, class
 	 *	WM_NAME(STRING) = title
 	 */
-	/* class      instance    title       tags mask     isfloating   monitor */
-	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
-	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
+	/* class      instance    title       tags mask     isfloating   opacity         monitor */
+	{ "Gimp",     NULL,       NULL,       0,            1,           1.0,            -1 },
+	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           1.0,            -1 },
+	{ "st",       NULL,       NULL,       0,            0,           1.0,            -1 },
 };
 
 /* layout(s) */
 static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
 static const int nmaster     = 1;    /* number of clients in master area */
-static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
+static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
 static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
 
 static const Layout layouts[] = {
@@ -45,7 +69,7 @@
 };
 
 /* key definitions */
-#define MODKEY Mod1Mask
+#define MODKEY Mod4Mask
 #define TAGKEYS(KEY,TAG) \
 	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
 	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
@@ -65,6 +89,7 @@
 	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
 	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
 	{ MODKEY,                       XK_b,      togglebar,      {0} },
+	{ MODKEY|ShiftMask,             XK_b,      hideborder,     {0} },
 	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
 	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
 	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
@@ -81,10 +106,13 @@
 	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
 	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
 	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
-	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
-	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
-	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
-	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
+	{ MODKEY,                       XK_comma,  focusmon,       {.i = +1 } },
+	{ MODKEY,                       XK_period, focusmon,       {.i = -1 } },
+	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = +1 } },
+	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = -1 } },
+	{ MODKEY|ShiftMask,             XK_KP_Add, changeopacity,  {.f = +0.01 } },
+	{ MODKEY|ShiftMask,             XK_KP_Subtract, changeopacity,  {.f = -0.01 } },
+	{ MODKEY|ShiftMask,             XK_o,      toggleopacity,  {0} },
 	TAGKEYS(                        XK_1,                      0)
 	TAGKEYS(                        XK_2,                      1)
 	TAGKEYS(                        XK_3,                      2)
