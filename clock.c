// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"
#include <time.h>

time_t rawtime;
struct tm * timeinfo;
char buffer [80];

static 
gboolean on_expose_clock_applet (GtkWidget      *widget,
                       	   	 GdkEventExpose *event,
                           	 gpointer        data)
{
  cairo_t *cr;
  cr = gdk_cairo_create(widget->window);

  int width = widget->allocation.width, height = widget->allocation.height; 
  
  applet_background (cr, width, height);

  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_select_font_face(cr, "Arial",
      			 CAIRO_FONT_SLANT_NORMAL,
      			 CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 13);

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strftime (buffer,80,"%I:%M%p",timeinfo);

  cairo_text_extents_t extents;
  cairo_text_extents(cr, buffer, &extents);

  cairo_move_to(cr, 0, (height/2));
  cairo_show_text(cr, buffer); 


  strftime (buffer,80,"%b %d, %Y",timeinfo);

  cairo_move_to(cr, 0, (height/2) + PADDING + extents.height);
  cairo_show_text(cr, buffer); 

 
  cairo_destroy(cr);

  return FALSE;
}

gboolean repaint (gpointer data) {

 gtk_widget_queue_draw(data);
 return TRUE;
}


GtkWidget * 
clock_init (void)
{
g_print("initializing clock applet\n");
GtkWidget *app = gtk_drawing_area_new ();
gtk_drawing_area_size (GTK_DRAWING_AREA(app), 0.08*width_screen, BAR_HEIGHT);
gtk_signal_connect (GTK_OBJECT (app), "expose_event",
                   (GtkSignalFunc) on_expose_clock_applet, NULL);

 gint m_timer = g_timeout_add(1000, repaint, app);

return app;
}
