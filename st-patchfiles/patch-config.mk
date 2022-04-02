--- config.mk.orig	2022-03-20 13:40:40.300042000 +0000
+++ config.mk	2022-03-20 13:38:35.578819000 +0000
@@ -15,10 +15,12 @@
 # includes and libs
 INCS = -I$(X11INC) \
        `$(PKG_CONFIG) --cflags fontconfig` \
-       `$(PKG_CONFIG) --cflags freetype2`
-LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft \
+       `$(PKG_CONFIG) --cflags freetype2` \
+       `$(PKG_CONFIG) --cflags harfbuzz`
+LIBS = -L$(X11LIB) -lm -lrt -lX11 -lutil -lXft -lXrender\
        `$(PKG_CONFIG) --libs fontconfig` \
-       `$(PKG_CONFIG) --libs freetype2`
+       `$(PKG_CONFIG) --libs freetype2` \
+       `$(PKG_CONFIG) --libs harfbuzz`
 
 # flags
 STCPPFLAGS = -DVERSION=\"$(VERSION)\" -D_XOPEN_SOURCE=600
