// (c) 2012 Raja Jamwal <linux1@zoho.com>
/*
	Protean OS - BAR (C) Raja Jamwal 2011
*/

#include "bar.h"

int 
main (int argc, char **argv){

 gtk_init (&argc, &argv);

 init_window();

 setup_panel();

 gtk_widget_show_all (window); /* realize the windows */

 finalize();

 gtk_main ();

 return 0;
}
