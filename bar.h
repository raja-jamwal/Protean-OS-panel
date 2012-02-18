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

#ifndef _H_PROTEAN_BAR_H
#define _H_PROTEAN_BAR_H

#define BAR_HEIGHT 		30
#define STRUT_TOP 		2
#define STRUT_TOP_START 	8
#define STRUT_TOP_END 		9
#define APPLET_TYPE_DATE	1
#define M_PI			3.14
#define PADDING			5

#define APPLET_TYPE_TRAY	1<<1
#define APPLET_TYPE_TASK	1<<2

#define VERSION			0.1

#define MAX_PATH		1024

#define WINDOW_NAME_LENGTH	20

// clock.c

GtkWidget * 
clock_init ();

// home.c

GtkWidget * 
home_init ();

void applet_background (cairo_t *cr, 
			unsigned int width, 
			unsigned int height);

// window_manager.c

GtkWidget *
window_manager_init ();

// common declarations

struct APPLETS{
GtkWidget * applet;
GtkWidget * (*init) (void);
int type;
};

// applet.c

void create_applets(void);

//bar.c

extern Atom net_wm_strut;
extern Atom net_wm_strut_partial;
extern Atom net_wm_window_type;

extern GtkWidget *window;
extern gint width_screen, height_screen;
extern gint window_width, window_height;

double RGB2C(double x);

// xutil.c

void
set_strut 	 (GdkWindow        *gdk_window,
                  GtkPositionType   position,
                  guint32           strut,
                  guint32           strut_start,
                  guint32           strut_end);

// panel.c
extern GtkWidget * taskmm;
extern GtkWidget * tray;
static  gboolean on_expose_event_panel (GtkWidget      *widget,
                       			GdkEventExpose *event,
                       			gpointer        data);

void init_window(void);

void setup_panel(void);

void finalize(void);

// icon_entry.c

GtkWidget * enum_icons(void);

#endif
