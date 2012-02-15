// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"

static 
gboolean on_expose_event_panel (GtkWidget      *widget,
                       GdkEventExpose *event,
                       gpointer        data)
{
  cairo_t *cr;

  cr = gdk_cairo_create(widget->window);

  int width = widget->allocation.width, height = widget->allocation.height; 
  
  // create a gradient, paint the background
  cairo_pattern_t *linpat = cairo_pattern_create_linear (0, 0, 0, height);
  cairo_pattern_add_color_stop_rgb (linpat, 0, RGB2C(20), RGB2C(20), RGB2C(20));
  cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0, 0);
  cairo_set_source(cr, linpat);
  cairo_rectangle(cr, 0, 0 , width, height);
  cairo_fill(cr);

  cairo_destroy(cr);

  return FALSE;
}


void
init_window(void)
{

 window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

 gtk_window_stick (GTK_WINDOW(window));
 gtk_window_set_decorated (GTK_WINDOW(window), FALSE);

 GdkScreen * screen = gtk_widget_get_screen (GTK_WIDGET(window));
 width_screen = gdk_screen_get_width (screen);
 height_screen = gdk_screen_get_height (screen);

 gtk_widget_set_size_request (GTK_WIDGET(window), width_screen, BAR_HEIGHT);
 gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
 gtk_window_move (GTK_WINDOW(window), 0, 0);

 g_signal_connect_swapped (G_OBJECT(window), "destroy",
      			  G_CALLBACK(gtk_main_quit), NULL);

 /* expose event for background */
 //gtk_widget_set_double_buffered( GTK_WIDGET(window), FALSE );
 /* we'll draw the background of the main window */

 gtk_widget_set_app_paintable(window, TRUE);

 /* needed for some instance, this create the DC for painting explicitly*/
 //gtk_widget_realize (GTK_WIDGET(window));

 g_signal_connect(G_OBJECT(window), "expose-event", G_CALLBACK (on_expose_event_panel), NULL);

}

void
setup_panel(void)
{

 // add the alignment container

 GtkWidget * container = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);

 // main container to add everything in panel
 GtkWidget * hbox = gtk_hbox_new(FALSE, 1);
 
 // logo
 GtkWidget * label = gtk_label_new (NULL);
 gtk_label_set_markup (GTK_LABEL (label), "<span font=\"arial\" font_weight=\"bold\" font_size=\"18500\" foreground=\"white\">ProteanOS</span>");

 // task manager

 taskmm = gtk_hbox_new(FALSE, 1);
 //gtk_widget_set_size_request (taskmm, (0.8)*width_screen, taskmm->height);

 // tray gtk_hbox_new (w, 1) w-> homogenous
 tray = gtk_hbox_new (FALSE, 1);

 // gtk_box_pack_start (q, w, e, r, y) e- expand,r-fill,y-padding
 gtk_box_pack_start (GTK_BOX(hbox), label, FALSE, FALSE, 5);
 gtk_box_pack_start (GTK_BOX(hbox), taskmm, TRUE, TRUE, 0);
 gtk_box_pack_start (GTK_BOX(hbox), tray, FALSE, FALSE, 0);

  // test check
 //GtkWidget * text = gtk_label_new (NULL);
 //gtk_label_set_markup (GTK_LABEL (text), "<span font=\"arial\" font_weight=\"normal\" font_size=\"18500\" foreground=\"white\">tray</span>");
 // GtkWidget * tex = gtk_label_new (NULL);
 //gtk_label_set_markup (GTK_LABEL (tex), "<span font=\"arial\" font_weight=\"bold\" font_size=\"18500\" foreground=\"white\">More</span>");
	
 //gtk_box_pack_start (GTK_BOX(taskmm), text, FALSE, FALSE, 1);
 //gtk_box_pack_start (GTK_BOX(taskmm), tex, FALSE, FALSE, 1);

 // add applets to tray icon

 create_applets();

 gtk_container_add (GTK_CONTAINER (container), hbox);
 gtk_container_add (GTK_CONTAINER(window), container);

}

void 
finalize(void)
{

 // window should be realized, mapped before sending strut

 GdkWindow *win = gtk_widget_get_window (GTK_WIDGET (window));
 gint root_x, root_y;

 gtk_window_get_position (GTK_WINDOW (window), &root_x, &root_y);

 gtk_window_get_size (GTK_WINDOW (window), &window_width, &window_height);

 g_print("we are at %ix %iy size %i:%i\n", root_x, root_y, window_width, window_height);

 set_strut (win, GTK_POS_TOP, root_y+BAR_HEIGHT, root_x, root_x+window_width);


}

