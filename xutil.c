// (c) 2012 Raja Jamwal <linux1@zoho.com>
#include "bar.h"

void
set_strut 	 (GdkWindow        *gdk_window,
                  GtkPositionType   position,
                  guint32           strut,
                  guint32           strut_start,
                  guint32           strut_end)
{
  Atom atom_type[1], atom_window_type;
  
  Display *display;
  Window   window;
  gulong   struts [12] = { 0, };

  g_return_if_fail (GDK_IS_WINDOW (gdk_window));

  display = GDK_WINDOW_XDISPLAY (gdk_window);
  window  = GDK_WINDOW_XWINDOW (gdk_window);

  if (net_wm_strut == None)
    net_wm_strut = XInternAtom (display, "_NET_WM_STRUT", False);
  if (net_wm_strut_partial == None)
    net_wm_strut_partial = XInternAtom (display, "_NET_WM_STRUT_PARTIAL", False);

  struts [STRUT_TOP] = strut;
  struts [STRUT_TOP_START] = strut_start;
  struts [STRUT_TOP_END] = strut_end;

  gdk_error_trap_push ();

  atom_window_type = gdk_x11_get_xatom_by_name ("_NET_WM_WINDOW_TYPE");
  atom_type[0] = gdk_x11_get_xatom_by_name ("_NET_WM_WINDOW_TYPE_DOCK");

  XChangeProperty (display,
                   window,
                   atom_window_type,
                   XA_ATOM, 32, PropModeReplace,
                   (guchar *) &atom_type, 1);

  XChangeProperty (display, window, net_wm_strut,
                   XA_CARDINAL, 32, PropModeReplace,
                   (guchar *) &struts, 4);
  XChangeProperty (display, window, net_wm_strut_partial,
                   XA_CARDINAL, 32, PropModeReplace,
                   (guchar *) &struts, 12);

  XChangeProperty(display, window,
    		  XInternAtom(display, "_NET_WM_WINDOW_TYPE", False),
    		  XA_ATOM, 32, PropModeReplace,
    		  (unsigned char *) &net_wm_window_type, 1);

  
  gdk_error_trap_pop ();
}

