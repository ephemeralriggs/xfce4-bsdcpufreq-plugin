/* Copyright (c) 2014-2020, Thomas Zander <thomas.e.zander@googlemail.com>
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

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include "freq_funcs.h"
#include "bsdcpufreq.h"
#include "dialog_about.h"
#include "dialog_properties.h"

#define DEFAULT_OBSERVED_CPU 0
#define TOOLTIP_STRING_EMPTY "CPU -: -.-- GHz"
#define TOOLTIP_STRING_FORMAT "CPU %d: %1.02f GHz"
#define UPDATE_INTERVAL 500	//miliseconds

#define CSS_DATA "\
		progressbar.horizontal trough { min-height: 8px; }\
		progressbar.horizontal progress { min-height: 8px; }\
		progressbar.vertical trough { min-width: 8px; }\
		progressbar.vertical progress { min-width: 8px; }\
		progressbar progress { background-color: %s; background-image: none; }"

const gchar *DEFAULT_GRAPH_COLOR = "#346ae9";

static gint bsdcpufreq_update(BSDcpufreqPlugin *bsdcpufreq)
{
	if (bsdcpufreq->miblen == 0)
	{
		gtk_widget_set_tooltip_text(bsdcpufreq->status, TOOLTIP_STRING_EMPTY);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bsdcpufreq->status), 0);
		return TRUE;	//Avoid querying if we do not have a valid mib
	}

	int error;
	gint freq = get_int_sysctl_by_mib(bsdcpufreq->mib, bsdcpufreq->miblen, &error);
	if (error != 0)
	{
		gtk_widget_set_tooltip_text(bsdcpufreq->status, TOOLTIP_STRING_EMPTY);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bsdcpufreq->status), 0);
	}
	else
	{
		gchar freqstr[20];
		gfloat freq_fraction = freq / bsdcpufreq->observed_cpu_max_freq;
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(bsdcpufreq->status), freq_fraction);
		g_snprintf(freqstr, sizeof(freqstr), TOOLTIP_STRING_FORMAT, bsdcpufreq->observed_cpu, (gfloat)freq/1000.0f);
		gtk_widget_set_tooltip_text(bsdcpufreq->status, freqstr);
	}

	return TRUE;
}

void bsdcpufreq_init_cpu_data(BSDcpufreqPlugin *bsdcpufreq)
{
	int error;
	bsdcpufreq->observed_cpu_max_freq = get_max_cpufreq_level(bsdcpufreq->observed_cpu);
	bsdcpufreq->miblen = get_mib_for_cpu_freq(bsdcpufreq->mib, sizeof(bsdcpufreq->mib)/sizeof(bsdcpufreq->mib[0]), bsdcpufreq->observed_cpu, &error);
	if (error != 0)
		bsdcpufreq->miblen = 0;
}

void bsdcpufreq_set_status_color(BSDcpufreqPlugin *bsdcpufreq)
{
#if !GTK_CHECK_VERSION (3, 20, 0)
#error "This panel plugin requires gtk+ >= 3.20"
#endif
	gchar *color = gdk_rgba_to_string(&bsdcpufreq->status_color);
	gchar *css = g_strdup_printf(CSS_DATA, color);
	gtk_css_provider_load_from_data(g_object_get_data(G_OBJECT(bsdcpufreq->status), "css_provider"), css, strlen(css), NULL);
	g_free(color);
	g_free(css);
}

void bsdcpufreq_save(XfcePanelPlugin *plugin, BSDcpufreqPlugin *bsdcpufreq)
{
	XfceRc *rc;

	gchar *config_file = xfce_panel_plugin_save_location(plugin, TRUE);

	if (G_UNLIKELY(config_file == NULL))
	{
		DBG ("Error accessing the config file");
		return;
	}

	rc = xfce_rc_simple_open(config_file, FALSE);
	g_free(config_file);

	if (G_LIKELY(rc != NULL))
	{
		xfce_rc_write_int_entry(rc, "observed_cpu", bsdcpufreq->observed_cpu);

		gchar *graph_color = gdk_rgba_to_string(&bsdcpufreq->status_color);
		xfce_rc_write_entry(rc, "graph_color", graph_color);
		g_free(graph_color);

		xfce_rc_close(rc);
		DBG("Settings saved");
	}
}

static void bsdcpufreq_read(BSDcpufreqPlugin *bsdcpufreq)
{
	XfceRc *rc;
	gchar *config_file = xfce_panel_plugin_save_location(bsdcpufreq->plugin, TRUE);

	if (G_LIKELY(config_file != NULL))
	{
		rc = xfce_rc_simple_open(config_file, TRUE);
		g_free(config_file);

		if (G_LIKELY(rc != NULL))
		{
			bsdcpufreq->observed_cpu = xfce_rc_read_int_entry(rc, "observed_cpu", DEFAULT_OBSERVED_CPU);

			const gchar *graph_color = xfce_rc_read_entry(rc, "graph_color", DEFAULT_GRAPH_COLOR);
			gdk_rgba_parse(&bsdcpufreq->status_color, graph_color);

			xfce_rc_close(rc);
			return;
		}
	}

	DBG("Could not read settings, applying default settings");

	bsdcpufreq->observed_cpu = DEFAULT_OBSERVED_CPU;
	gdk_rgba_parse(&bsdcpufreq->status_color, DEFAULT_GRAPH_COLOR);
}

static BSDcpufreqPlugin *bsdcpufreq_new(XfcePanelPlugin *plugin)
{
	BSDcpufreqPlugin *bsdcpufreq;
	GtkOrientation orientation;

	bsdcpufreq = g_slice_new0(BSDcpufreqPlugin);
	bsdcpufreq->plugin = plugin;
	bsdcpufreq_read(bsdcpufreq);
	orientation = xfce_panel_plugin_get_orientation(plugin);

	bsdcpufreq->ebox = gtk_event_box_new();
	gtk_widget_show(bsdcpufreq->ebox);

	bsdcpufreq->hvbox = gtk_box_new(orientation, 2);
	gtk_widget_show(bsdcpufreq->hvbox);
	gtk_container_add(GTK_CONTAINER(bsdcpufreq->ebox), bsdcpufreq->hvbox);
	// Ensure mouse events are passed to the event handlers
	gtk_event_box_set_above_child(GTK_EVENT_BOX(bsdcpufreq->ebox), TRUE);

	bsdcpufreq->status = GTK_WIDGET(gtk_progress_bar_new());
	gtk_widget_set_tooltip_text(GTK_WIDGET(bsdcpufreq->status),(_(TOOLTIP_STRING_EMPTY)));
	gtk_widget_show(bsdcpufreq->status);
	gtk_box_pack_start(GTK_BOX(bsdcpufreq->hvbox), bsdcpufreq->status, FALSE, FALSE, 0);

	GtkCssProvider *css_provider = gtk_css_provider_new();
	gtk_style_context_add_provider(
		GTK_STYLE_CONTEXT(gtk_widget_get_style_context(GTK_WIDGET(bsdcpufreq->status))),
		GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_set_data(G_OBJECT(bsdcpufreq->status), "css_provider", css_provider);
	bsdcpufreq_set_status_color(bsdcpufreq);

	return bsdcpufreq;
}

static void bsdcpufreq_free(XfcePanelPlugin *plugin, BSDcpufreqPlugin *bsdcpufreq)
{
	GtkWidget *dialog;
	//Check if the dialog is still open. If so, destroy it
	dialog = g_object_get_data(G_OBJECT(plugin), "dialog");
	if (G_UNLIKELY(dialog != NULL))
		gtk_widget_destroy(dialog);

	gtk_widget_destroy(bsdcpufreq->hvbox);

	g_slice_free(BSDcpufreqPlugin, bsdcpufreq);
}

static gboolean bsdcpufreq_size_changed(XfcePanelPlugin *plugin, gint size, BSDcpufreqPlugin *bsdcpufreq)
{
	//Constants are based on empirical values found in other plugin code and some guesswork
	const gint threshold_size = 26;
	gtk_container_set_border_width(GTK_CONTAINER(bsdcpufreq->ebox), (size > threshold_size ? 2 : 1));
	return TRUE;
}

static void bsdcpufreq_orientation_changed(XfcePanelPlugin *plugin, GtkOrientation orientation, BSDcpufreqPlugin *bsdcpufreq)
{
	gtk_orientable_set_orientation(GTK_ORIENTABLE(bsdcpufreq->hvbox), orientation);
	if (orientation == GTK_ORIENTATION_HORIZONTAL)
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(bsdcpufreq->status), GTK_ORIENTATION_VERTICAL);
		gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(bsdcpufreq->status), TRUE);
	}
	else
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(bsdcpufreq->status), GTK_ORIENTATION_HORIZONTAL);
		gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(bsdcpufreq->status), FALSE);
	}

	bsdcpufreq_size_changed(plugin, xfce_panel_plugin_get_size(plugin), bsdcpufreq);
}

static void bsdcpufreq_construct(XfcePanelPlugin *plugin)
{
	BSDcpufreqPlugin *bsdcpufreq;

	//No translations at the moment, so no gettext
	//xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

	bsdcpufreq = bsdcpufreq_new(plugin);
	bsdcpufreq_init_cpu_data(bsdcpufreq);
	bsdcpufreq->update_timeout = g_timeout_add(UPDATE_INTERVAL, (GSourceFunc)bsdcpufreq_update, bsdcpufreq);

	gtk_container_add(GTK_CONTAINER(plugin), bsdcpufreq->ebox);

	//Required for initial orientation
	bsdcpufreq_orientation_changed(plugin, xfce_panel_plugin_get_orientation(plugin), bsdcpufreq);

	xfce_panel_plugin_add_action_widget(plugin, bsdcpufreq->ebox);

	g_signal_connect(G_OBJECT(plugin), "free-data", G_CALLBACK(bsdcpufreq_free), bsdcpufreq);
	g_signal_connect(G_OBJECT(plugin), "save", G_CALLBACK(bsdcpufreq_save), bsdcpufreq);
	g_signal_connect(G_OBJECT(plugin), "size-changed", G_CALLBACK(bsdcpufreq_size_changed), bsdcpufreq);
	g_signal_connect(G_OBJECT(plugin), "orientation-changed", G_CALLBACK(bsdcpufreq_orientation_changed), bsdcpufreq);

	xfce_panel_plugin_menu_show_configure(plugin);
	g_signal_connect(G_OBJECT(plugin), "configure-plugin", G_CALLBACK(bsdcpufreq_configure), bsdcpufreq);

	xfce_panel_plugin_menu_show_about(plugin);
	g_signal_connect(G_OBJECT(plugin),"about", G_CALLBACK(bsdcpufreq_about), NULL);
}

XFCE_PANEL_PLUGIN_REGISTER(bsdcpufreq_construct);

// vim:ts=8
