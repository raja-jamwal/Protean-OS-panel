// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"

//TODO: on 64-bit system check the back data format of xserver conversation explicitally [DONE]

#define WIN_STATE_STICKY          (1<<0)	/* everyone knows sticky */
#define WIN_STATE_MINIMIZED       (1<<1)	/* ??? */
#define WIN_STATE_MAXIMIZED_VERT  (1<<2)	/* window in maximized V state */
#define WIN_STATE_MAXIMIZED_HORIZ (1<<3)	/* window in maximized H state */
#define WIN_STATE_HIDDEN          (1<<4)	/* not on taskbar but window visible */
#define WIN_STATE_SHADED          (1<<5)	/* shaded (NeXT style) */
#define WIN_STATE_HID_WORKSPACE   (1<<6)	/* not on current desktop */
#define WIN_STATE_HID_TRANSIENT   (1<<7)	/* owner of transient is hidden */
#define WIN_STATE_FIXED_POSITION  (1<<8)	/* window is fixed in position even */
#define WIN_STATE_ARRANGE_IGNORE  (1<<9)	/* ignore for auto arranging */

#define WIN_HINTS_SKIP_FOCUS      (1<<0)	/* "alt-tab" skips this win */
#define WIN_HINTS_SKIP_WINLIST    (1<<1)	/* not in win list */
#define WIN_HINTS_SKIP_TASKBAR    (1<<2)	/* not on taskbar */
#define WIN_HINTS_GROUP_TRANSIENT (1<<3)	/* ??????? */
#define WIN_HINTS_FOCUS_ON_CLICK  (1<<4)	/* app only accepts focus when clicked */
#define WIN_HINTS_DO_NOT_COVER    (1<<5)	/* attempt to not cover this window */

GtkWidget *hbox = NULL;

typedef struct WINDOW_META{

	struct win_meta *next;
	Window win;
	Pixmap icon;
	GtkImage * icon_image;
	Pixmap mask;
	char *name;
	char * icon_name;
	//int pos_x;
	//int width;
	unsigned int focused:1;
	unsigned int iconified:1;
	unsigned int icon_copied:1;
	unsigned int icon_shown:1;
	unsigned int found_again:1;
	GtkWidget * button;
	int button_handler_id;

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


int
is_hidden (Window win)
{
	unsigned long *data;
	int ret = 0;

	data = get_prop_data (win, "_WIN_HINTS", XA_CARDINAL, 0);
	if (data)
	{
		if ((*data) & WIN_HINTS_SKIP_TASKBAR)
			ret = 1;
		XFree (data);
	}

	return ret;
}


int
is_focused (Window win)
{
	// Find the window which has the current keyboard focus.
   	Window winFocus;
   	int    revert;
   	XGetInputFocus(gdk_x11_get_default_xdisplay(), &winFocus, &revert);
	
	if (win != winFocus) return FALSE;
	
	return TRUE;
}


int
is_iconified (Window win)
{
	unsigned long *data;
	int ret = FALSE;

	Atom xa_wm_state = XInternAtom(gdk_x11_get_default_xdisplay(), "WM_STATE" , FALSE);

	data = get_prop_data (win, xa_wm_state, xa_wm_state, 0);
	if (data)
	{
		if (data[0] == IconicState)
			ret = TRUE;
		XFree (data);
	}
	return ret;
}

win_meta *
found_window_again (container *tb, Window win)
{
	win_meta *list = (win_meta*)(tb->win_list);

	while (list)
	{
		if (list->win == win){
			list->found_again=TRUE;
			return list;
		}
		list = list->next;
	}
	
	// new window, populate with new data
	if( list == NULL){
	
		win_meta * ld = calloc(1, sizeof(win_meta));
		ld->win = win;
		ld->next = NULL;
		ld->found_again = TRUE;
		ld->iconified = is_iconified (win);
		ld->focused = is_focused (win);

		if (ld->name)
			XFree (ld->name);

		ld->name = get_prop_data (ld->win, XA_WM_NAME, XA_STRING, 0);
		//ld->icon_name = get_prop_data (ld->win, XA_WM_NAME, XA_STRING, 0);

		//g_print ("%s", ld->icon_name);

		//gtk_window_get_icon (gtk_window_foreign_new (ld->win));
		
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

// BUG: we never get click events on task button, once xsendevent is issued, either from us or from any other applciation
void
window_activate (Display * dd, Window wid) 
{
  int ret;
  XEvent xev;
  XWindowAttributes wattr;

  memset(&xev, 0, sizeof(xev));
  xev.type = ClientMessage;
  xev.xclient.display = dd;
  xev.xclient.window = wid;
  xev.xclient.message_type = XInternAtom(dd, "_NET_ACTIVE_WINDOW", False);
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 2L; /* 2 == Message from a window pager */
  xev.xclient.data.l[1] = CurrentTime;

  XGetWindowAttributes(dd, wid, &wattr);
  ret = XSendEvent(dd, wattr.screen->root, False,
                   SubstructureNotifyMask | SubstructureRedirectMask,
                   &xev);

  XSync (dd, False);
  XFlush(dd);
}

void
window_focus(Display * dd, Window * window)
{

  int ret = 0;
  int unused_revert_ret;

  ret = XGetInputFocus(dd, window, &unused_revert_ret);

}

void
window_map(Display * dd, Window wid)
{
  int ret = 0;
  ret = XMapWindow(dd, wid);
  XFlush(dd);
}

void
window_minimize(Display * dd, Window window) 
{
  int ret;
  int screen;

  /* Get screen number */
  XWindowAttributes attr;
  XGetWindowAttributes(dd, window, &attr);
  screen = XScreenNumberOfScreen(attr.screen);

  /* Minimize it */
  ret = XIconifyWindow(dd, window, screen);
}

void 
task_click	(GtkWidget *widget,
                 gpointer   data )
{
	win_meta * list = data;

	Display * dd = gdk_x11_get_default_xdisplay();
		
	g_print("called");

	if (list->iconified == TRUE)
	{
			list->iconified = FALSE;
			list->focused = TRUE;
			window_map (dd, list->win);
			//XMapWindow (dd, list->win);
			window_focus (dd, &(list->win));
				
			g_print("was iconified");
	} else
		{
			if (list->focused == TRUE)
			{
			list->iconified = TRUE;
			list->focused = FALSE;
			window_minimize (dd, (list->win));
			g_print("Was focused");
			}else
			{
			
			list->iconified = FALSE;
			list->focused = TRUE;
			//XMapWindow (dd, list->win);
			window_focus (dd, &(list->win));
			
			//XRaiseWindow (dd, list->win); // not doing
			//XMapWindow (dd, list->win);
			g_print("raising");
			//XSetInputFocus (dd, list->win, RevertToParent, CurrentTime);
			//XFlush(dd);
			//XSync (dd, False);
			}
	}
	XFlush(dd);
	XSync (dd, False);

	//g_print ("Hello again - %s was pressed\n", (char *) list->name);
}

void add_task_button(container * tb, GtkWidget * hbox, int nwins)
{
	win_meta *list = tb->win_list;

	// make sure to free widgets memory from previous calls

	while (list)
	{
		if(list->name){

			if (list->name && list->button)
				gtk_container_remove (hbox, list->button);
			
			list->button = NULL;
			list->icon = 0;
			list->mask =0;
			list->icon_image = NULL;		
			//list->button = gtk_button_new_with_label(list->name);

			/*if (list->button_handler_id)
				g_signal_handler_disconnect (list->button, list->button_handler_id);*/
			
			list->button = gtk_button_new();

			GtkWidget * bbox = gtk_hbox_new (FALSE, 0);

			gtk_box_pack_start (GTK_BOX(bbox), gtk_image_new_from_file("Tango/22x22/actions/window_new.png"), FALSE, FALSE, 0);

			gtk_box_pack_start (GTK_BOX(bbox), gtk_label_new(list->name), TRUE, TRUE, 0);

			gtk_container_add (GTK_CONTAINER(list->button),
					   bbox);

			list->button_handler_id = gtk_signal_connect (GTK_OBJECT (list->button), "clicked",
                        		    GTK_SIGNAL_FUNC (task_click), (gpointer) list);
		
			//if (!list->icon_shown && list->icon_copied){

			/*if (list->icon == 0 && list->icon_image ==NULL && list->win !=NULL)
			{

				XWMHints *hin;

				list->icon = None;
				list->mask = None;

				hin = (XWMHints *) get_prop_data (list->win, XA_WM_HINTS, XA_WM_HINTS, 0);
				if (hin)
				{
				if ((hin->flags & IconPixmapHint))
				{
					if ((hin->flags & IconMaskHint))
					{
					list->mask = hin->icon_mask;
					}

				list->icon = hin->icon_pixmap;
				//list->icon_copied = TRUE;

				list->icon_image = gtk_image_new_from_pixmap (gdk_pixmap_foreign_new (list->icon),
								    	      NULL);
			
				//scale_icon (tk);
				}
				XFree (hin);
			}
			}
			
			if (list->icon_image && list->button !=NULL && list->win !=NULL){
				
			
				gtk_container_add (GTK_CONTAINER(list->button),
						   GTK_WIDGET(list->icon_image));
			
				list->icon_shown = TRUE;
			}							
			//}*/
		
			gtk_widget_set_size_request (list->button, 
						    (hbox->allocation.width/nwins)
						    , list->button->allocation.height);

			if (list->button)
				gtk_box_pack_start (GTK_BOX(hbox), list->button, FALSE, TRUE, 0);
		}		
		list = list->next;
	}

	gtk_widget_show_all(hbox);

}

void * remove_old_window (container *tb)
{
	win_meta *list = tb->win_list;

	while (list)
	{
		win_meta *even_next = (win_meta*) list->next;
	
		if (even_next){
			if (even_next->found_again == FALSE){
			// delete the button
			if (even_next->name && even_next->button)
				gtk_container_remove (hbox, even_next->button);
			
			even_next->button = NULL;

			list->next = (win_meta*) even_next->next;
			return;
			}
		}
	
		list = list->next;
	}

	return 0;
}

int total_window (container *tb)
{
	
	win_meta *list = tb->win_list;

	int nwins = 0;

	while (list)
	{
		nwins++;
	
		list = list->next;
	}

	return nwins;

}

void handle_property_notify (Window win, Atom at, GtkWidget * datai)
{

	win_meta *list = all_wins->win_list;

	while (list)
	{
		list->found_again = FALSE;
		
		list = list->next;
	}

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
                guint32 *array = (guint32*) data;

		int k;
                for ( k = 0; k < numItems; k++)
                {
                        // get window Id:
                        Window w = (Window) array[k];

                        found_window_again (all_wins, w);
		
                }
                XFree(data);
        }
	
	remove_old_window (all_wins);

	add_task_button (all_wins, datai, total_window(all_wins));

	
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
				break;
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

	hbox = gtk_hbox_new(FALSE, 0);

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
