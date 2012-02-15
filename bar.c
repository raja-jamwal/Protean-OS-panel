// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cairo.h>

Atom net_wm_strut;
Atom net_wm_strut_partial;
Atom net_wm_window_type;

GtkWidget *window;
gint width_screen, height_screen;
gint window_width, window_height;

GtkWidget * taskmm;
GtkWidget * tray;

double RGB2C(double x){return (double) (x/255);}


