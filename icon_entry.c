// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"

// "path to icon", "command", "description", "GtkImage", "saturation"

#define ICON_SATURATION 5

struct TASKMM_ICONS{

	char path_to_icon[MAX_PATH];
	char command[100];
	char tooltip[256];
	GtkWidget * image_widget;
	int saturation;

};

struct TASKMM_ICONS icons[] = {
	
	{"Tango/scalable/apps/browser.svg", "firefox", "Browse the web", NULL, 0},
	{"Tango/scalable/apps/gnome-terminal.svg", "gnome-terminal", "Terminal emulator", NULL, 0},
	{"Tango/scalable/apps/update-manager.svg", "update", "Update", NULL, 0},
	{"Tango/scalable/apps/wallpaper.svg", "musescore", "Create sheet music", NULL, 0}
	
};

static gboolean
icon_press_callback 	(GtkWidget      *event_box,
                         GdkEventButton *event,
                         gpointer        data)
{
	g_spawn_command_line_async ((char*) data, NULL);
    	return TRUE;
}


static 
gboolean draw_icons 		(GtkWidget            *widget,
                       	   	 GdkEventExpose       *event,
                           	 struct TASKMM_ICONS  *data)
{
	// TODO: implement a nice way to render background of all items in panel

	cairo_t *cr;
  	cr = gdk_cairo_create(widget->window);

  	int width = widget->allocation.width, height = widget->allocation.height; 
  
  	applet_background (cr, width, height);

	// TODO: remove seeking disk for image on every expose event

	GdkPixbuf *pixbuf =	gdk_pixbuf_new_from_file_at_size((char*)data->path_to_icon, (4/3)*(BAR_HEIGHT-PADDING*2), BAR_HEIGHT-PADDING*2, NULL);

	if (pixbuf == NULL)
	{
		g_printerr((char*)data->path_to_icon);
		g_printerr("Failed to resize image\n");
		cairo_destroy(cr);
		return 1;
	}

	GdkPixbuf *dest_pixbuf = gdk_pixbuf_copy (pixbuf);

	gdk_pixbuf_saturate_and_pixelate (pixbuf, dest_pixbuf, ((data->saturation)>1.0?ICON_SATURATION:1.0), FALSE);
	
	gtk_image_set_from_pixbuf(GTK_IMAGE(widget), dest_pixbuf);

	cairo_destroy(cr);

	return FALSE;
}

static 
gboolean draw_hover_icons_enter  (GtkWidget           *widget,
                       	   	 GdkEventExpose      *event,
                           	 struct TASKMM_ICONS *data)
{data->saturation = 30;return FALSE;}

static 
gboolean draw_hover_icons_leave  (GtkWidget           *widget,
                       	   	 GdkEventExpose      *event,
                           	 struct TASKMM_ICONS *data)
{data->saturation = 1.0;return FALSE;}



GtkWidget * enum_icons(void){

 	// icons container

 	GtkWidget * hbox = gtk_hbox_new(FALSE, 3);

	/* install our icons */

 	int i;

 	for (i=0; i<G_N_ELEMENTS(icons); i++)
 	{
		struct TASKMM_ICONS temp = icons[i];

		GtkWidget *image;
    		GtkWidget *event_box;

    		image = gtk_image_new_from_file (temp.path_to_icon);

		icons[i].image_widget = image;
		icons[i].saturation = 1.0;

		//----g_signal_connect(image, "expose-event", G_CALLBACK(draw_icons), (gpointer)&icons[i]);
		
    		event_box = gtk_event_box_new ();

    		gtk_container_add (GTK_CONTAINER (event_box), image);

    		g_signal_connect (G_OBJECT (event_box),
                      	  	  "button_press_event",
                      	  	  G_CALLBACK (icon_press_callback),
                      	  	  (gpointer) icons[i].command);

		g_signal_connect (G_OBJECT (event_box),
                      	  	  "enter-notify-event",
                      	  	  G_CALLBACK (draw_hover_icons_enter),
                      	  	  (gpointer) &icons[i]);

		g_signal_connect (G_OBJECT (event_box),
                      	  	  "leave-notify-event",
                      	  	  G_CALLBACK (draw_hover_icons_leave),
                      	  	  (gpointer) &icons[i]);

		GtkTooltips *tooltips;
		tooltips = gtk_tooltips_new ();
		gtk_tooltips_set_tip (tooltips, event_box, icons[i].tooltip, NULL);

		gtk_box_pack_start (GTK_BOX(hbox), GTK_WIDGET(event_box), FALSE, FALSE, 1);

 	}

 	return hbox;
}
