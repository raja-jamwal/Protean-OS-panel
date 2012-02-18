// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"

struct APPLETS apps[] = {
{NULL, clock_init, APPLET_TYPE_TRAY}
//,{NULL, home_init, APPLET_TYPE_TASK} // dummy home app to show APPLET_TYPE_TASK applet type
,{NULL, window_manager_init, APPLET_TYPE_TASK}
};

void 
applet_background (cairo_t *cr, 
		   unsigned int width, 
		   unsigned int height)
{

 // create a gradient, paint the background
  cairo_pattern_t *linpat = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_t *button_normal = cairo_pattern_create_linear (0, 0, 0, height);

  cairo_pattern_add_color_stop_rgb (linpat, 0, RGB2C(20), RGB2C(20), RGB2C(20));
  cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0, 0);

  cairo_pattern_add_color_stop_rgba (button_normal, 0, RGB2C(194), RGB2C(194), RGB2C(194), 0.5);
  cairo_pattern_add_color_stop_rgba (button_normal, 1, 0, 0, 0, 0);
  
  cairo_set_source(cr, linpat);
  cairo_rectangle(cr, 0, 0 , width, height);
  cairo_fill(cr);

/*
  
  double x         	= 0,       
  	 y         	=  7,
       	 aspect        	= 1.0,     
       	 corner_radius 	= height / 20;   

  double radius = corner_radius / aspect;
  double degrees = M_PI / 180.0;

  cairo_new_sub_path (cr);
  cairo_arc (cr, x + width - radius -x, y + radius, radius, -90 * degrees, 0 * degrees);
  cairo_arc (cr, x + width - radius -x, y + height - radius, radius, 0 * degrees, 90 * degrees);
  cairo_arc (cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path (cr);

  cairo_set_source (cr, button_normal);
  cairo_fill_preserve (cr);
  //cairo_stroke (cr);
*/
}

void 
enum_applet (void)
{
 /* intialize all applets */

 gtk_box_pack_start (GTK_BOX(taskmm), enum_icons(), FALSE, FALSE, 1);

 int i;
 for (i=0; i<G_N_ELEMENTS(apps); i++)
 {
	struct APPLETS temp = apps[i];
	apps[i].applet = (temp.init)();

	// applet container to add single applet in panel
 	GtkWidget * hbox = gtk_hbox_new(FALSE, 1);

	gtk_box_pack_start (GTK_BOX(hbox), GTK_WIDGET(apps[i].applet), FALSE, FALSE, 1);

	if (temp.type == APPLET_TYPE_TRAY)
		gtk_box_pack_start (GTK_BOX(tray), hbox, FALSE, FALSE, 1);
	else
		gtk_box_pack_start (GTK_BOX(taskmm), hbox, FALSE, FALSE, 1);


 }

 
}


void 
create_applets(void)
{
 enum_applet();
}


