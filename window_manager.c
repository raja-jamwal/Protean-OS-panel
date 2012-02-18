#include "bar.h"

//TODO: on 64-bit system check the back data format of xserver conversation explicitally

typedef struct WINDOW_META{

	struct win_meta *next;
	Window win;
	Pixmap icon;
	Pixmap mask;
	char *name;
	int pos_x;
	int width;
	unsigned int focused:1;
	unsigned int iconified:1;
	unsigned int icon_copied:1;
	GtkWidget * button;

}win_meta;

typedef struct ABC{

	struct win_meta * win_list;

}container;


container * all_wins;

void *
get_prop_data (Window win, Atom prop, Atom type, int *items)
{
	Atom type_ret;
	int format_ret;
	unsigned long items_ret;
	unsigned long after_ret;
	unsigned char *prop_data;

	prop_data = 0;

	XGetWindowProperty (gdk_x11_get_default_xdisplay(), win, prop, 0, 0x7fffffff, False,
							  type, &type_ret, &format_ret, &items_ret,
							  &after_ret, &prop_data);
	if (items)
		*items = items_ret;

	return prop_data;
}

// Contains garbage and logic from trials
// TODO: clean un-used logic

win_meta *
find_win (container *tb, Window win)
{
	win_meta *list = (win_meta*)(tb->win_list);

	while (list)
	{
		if (list->win == win)
			return list;
		list = list->next;
	}
	
	// new window, populate with new data
	if( list == NULL){
	
		win_meta * ld = calloc(1, sizeof(win_meta));
		ld->win = win;
		ld->next = NULL;

		if (ld->name)
			XFree (ld->name);

		ld->name = (char*) malloc(WINDOW_NAME_LENGTH);
		char * title = get_prop_data (ld->win, XA_WM_NAME, XA_STRING, 0);
		memcpy (ld->name, title, WINDOW_NAME_LENGTH-4);
		
		//list = ld;

		list = tb->win_list;
		if (!list)
		{
		tb->win_list = ld;
		return tb->win_list;
		}

		while (1)
		{
			if (!list->next)
			{
			list->next = ld;
			return list->next;
			}
			list = list->next;
		}
	}
	
	if ( list != NULL)
		return list;
	
	return 0;
}

void add_task_button(container * tb, GtkWidget * hbox)
{
	win_meta *list = tb->win_list;

	//gtk_box_pack_start (GTK_BOX(hbox), label, FALSE, FALSE, 5);

	while (list)
	{
		if(list->name){

			if (list->name && list->button)
				gtk_container_remove (hbox, list->button);
			
			list->button = NULL;		
			list->button = gtk_button_new_with_label(list->name);

			if (list->button)
				gtk_box_pack_start (GTK_BOX(hbox), list->button, FALSE, FALSE, 1);
		}		
		list = list->next;
	}

	gtk_widget_show_all(hbox);

}


void handle_property_notify (Window win, Atom at, GtkWidget * datai)
{

	Window root_win = gdk_x11_get_default_root_xwindow ();

	Display *display = gdk_x11_get_default_xdisplay();

	Atom atom = XInternAtom(display, "_NET_CLIENT_LIST_STACKING" , TRUE);
        Atom actualType;
        int format;
        unsigned long numItems, bytesAfter;
        unsigned char *data =0;
        int status = XGetWindowProperty				(display,
                                                                root_win,
                                                                atom,
                                                                0L,
                                                                (~0L),
                                                                FALSE,
                                                                AnyPropertyType,
                                                                &actualType,
                                                                &format,
                                                                &numItems,
                                                                &bytesAfter,
                                                                &data);

        if (status >= Success && numItems)
        {
                // success - we have data: Format should always be 32:
                // cast to proper format, and iterate through values:
                int *array = (int*) data;

		int k;
                for ( k = 0; k < numItems; k++)
                {
                        // get window Id:
                        Window w = (Window) array[k];

                        find_win (all_wins, w);
		
                }
                XFree(data);
        }

	add_task_button (all_wins, datai);

	
}

GdkFilterReturn
event_filter_func  (GdkXEvent *xevent,
                    GdkEvent *event,
                    gpointer data)
{
	
	XEvent * ev = (XEvent*) xevent;

	switch (ev->type)
			{
			case DestroyNotify:
				//del_task (tb, ev.xdestroywindow.window);
				/* fall through */
			case Expose:
				//gui_draw_taskbar (tb);
				break;
			case PropertyNotify:
				handle_property_notify (ev->xproperty.window,
						        ev->xproperty.atom, data);		
				break;
			case FocusIn:
				//handle_focusin (tb, ev.xfocus.window);
				break;
			/*default:
				   printf ("unknown evt type: %d\n", ev.type);*/
			}

	return GDK_FILTER_CONTINUE;
}

GtkWidget * 
window_manager_init (void)
{

	g_print("initializing window_manager applet\n");

	all_wins = calloc(1, sizeof(container));

	GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

	Window root_win = gdk_x11_get_default_root_xwindow ();

	Display *display = gdk_x11_get_default_xdisplay();

	GdkWindow *root = gdk_window_foreign_new_for_display (gdk_display_get_default(),
							      gdk_x11_get_default_root_xwindow ());
	gdk_window_add_filter (NULL,
			       event_filter_func,
			       hbox);

	Window parent_ret;
	Window *children;
	int nchildren;

	XQueryTree (display, gdk_x11_get_default_root_xwindow (),
	&root_win, &parent_ret, &children, &nchildren);
	
	int i;
 	for (i = 0; i < nchildren; i++)
 	{
 		GdkWindow *toplevel = gdk_window_foreign_new_for_display (gdk_display_get_default(),
 					 				  children[i]);

 		/* Need property notify on all windows */

 		gdk_window_set_events (toplevel,
 					gdk_window_get_events (toplevel) |
 					GDK_PROPERTY_CHANGE_MASK);
 	}

 	/* Need MapNotify on new windows */
 	gdk_window_set_events (root, gdk_window_get_events (root) |
 			       GDK_STRUCTURE_MASK |
 			       GDK_PROPERTY_CHANGE_MASK |
 			       GDK_VISIBILITY_NOTIFY_MASK |
 			       GDK_SUBSTRUCTURE_MASK);


	//GtkWidget * label = gtk_label_new("check");

	//gtk_box_pack_start (GTK_BOX(hbox), label, FALSE, FALSE, 5);

	//gtk_drawing_area_size (GTK_DRAWING_AREA(app), 0.08*width_screen, BAR_HEIGHT);
	//gtk_signal_connect (GTK_OBJECT (app), "expose_event",
	//                   (GtkSignalFunc) on_expose_clock_applet, NULL);

	//gint m_timer = g_timeout_add(100, manage_x, hbox);

	return hbox;
}
