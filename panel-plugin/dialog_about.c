/* Copyright (c) 2014, Thomas Zander <thomas.e.zander@googlemail.com>
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

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "bsdcpufreq.h"
#include "dialog_about.h"

void bsdcpufreq_about(XfcePanelPlugin *plugin, gpointer data)
{
	GdkPixbuf *icon = xfce_panel_pixbuf_from_source("utilities-system-monitor", NULL, 32);
	const gchar *authors[] = {"Thomas Zander <thomas.e.zander@googlemail.com>", NULL};
	gtk_show_about_dialog(NULL, "logo", icon, "license", xfce_get_license_text (XFCE_LICENSE_TEXT_BSD),
		"version", "0.2", "program-name", "xfce4-bsdcpufreq-plugin",
		"authors", authors,
		"comments", _("Monitor CPU frequency scaling"),
		"website", "https://github.com/riggs-/xfce4-bsdcpufreq-plugin",
		"copyright", _("Copyright (c) 2014"), NULL);

	if(icon)
		g_object_unref(G_OBJECT(icon));
}

// vim:ts=8
