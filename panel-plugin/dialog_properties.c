/* Copyright (c) 2014-2019, Thomas Zander <thomas.e.zander@googlemail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <limits.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

#include "bsdcpufreq.h"
#include "dialog_properties.h"

static void observed_cpu_changed_cb(GtkSpinButton *spinbutton, gpointer data)
{
	BSDcpufreqPlugin *bsdcpufreq = (BSDcpufreqPlugin*)data;
	bsdcpufreq->observed_cpu = gtk_spin_button_get_value_as_int(spinbutton);
	bsdcpufreq_init_cpu_data(bsdcpufreq);
}

static void status_color_changed_cb(GtkColorButton *colorbutton, gpointer data)
{
	BSDcpufreqPlugin *bsdcpufreq = (BSDcpufreqPlugin*)data;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbutton), &bsdcpufreq->status_color);
	bsdcpufreq_set_status_color(bsdcpufreq);
}

static void bsdcpufreq_configure_response(GtkWidget *dialog, gint response, BSDcpufreqPlugin *bsdcpufreq)
{
	if (response != GTK_RESPONSE_HELP)
	{
		g_object_set_data(G_OBJECT(bsdcpufreq->plugin), "dialog", NULL);
		xfce_panel_plugin_unblock_menu(bsdcpufreq->plugin);
		bsdcpufreq_save(bsdcpufreq->plugin, bsdcpufreq);
		gtk_widget_destroy(dialog);
	}
	// else do nada
}

void bsdcpufreq_configure(XfcePanelPlugin *plugin, BSDcpufreqPlugin *bsdcpufreq)
{
	xfce_panel_plugin_block_menu(plugin);

	GtkWidget *dialog = xfce_titled_dialog_new_with_buttons(_("CPU frequency monitor"),
			GTK_WINDOW (gtk_widget_get_toplevel(GTK_WIDGET(plugin))),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			"gtk-close", GTK_RESPONSE_OK,
			NULL);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_icon_name(GTK_WINDOW(dialog), "xfce4-settings");

	g_object_set_data(G_OBJECT(plugin), "dialog", dialog);
	g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(bsdcpufreq_configure_response), bsdcpufreq);

	GtkBox *content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));

	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
	gtk_box_pack_start(content, grid, TRUE, TRUE, 0);

	GtkWidget *label = gtk_label_new (NULL);
	gtk_label_set_markup(GTK_LABEL (label), _("<b>General</b>"));
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

	//Observed CPU setting and associated color
	label = gtk_label_new(_("Observed CPU:"));
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	GtkWidget *spinbutton = gtk_spin_button_new_with_range(0,99,1);
	gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spinbutton), 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton), bsdcpufreq->observed_cpu);
	gtk_widget_set_halign(spinbutton, GTK_ALIGN_START);
	g_signal_connect(G_OBJECT(spinbutton), "value-changed", G_CALLBACK(observed_cpu_changed_cb), bsdcpufreq);
	gtk_grid_attach(GTK_GRID(grid), spinbutton, 1, 1, 1, 1);
	GtkWidget *colorbutton = gtk_color_button_new_with_rgba(&bsdcpufreq->status_color);
	gtk_grid_attach(GTK_GRID(grid), colorbutton, 2, 1, 1, 1);
	g_signal_connect(G_OBJECT(colorbutton), "color-set", G_CALLBACK(status_color_changed_cb), bsdcpufreq);

	gtk_widget_show_all(dialog);
}

// vim:ts=8
