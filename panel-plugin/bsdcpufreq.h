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

#if !defined(__BSDCPUFREQ_H__)
#define __BSDCPUFREQ_H__

#define MIB_MAX_SIZE 6

G_BEGIN_DECLS

typedef struct
{
	XfcePanelPlugin *plugin;

	GtkWidget *ebox;
	GtkWidget *hvbox;
	//GtkWidget *label;	//tooltip
	GtkWidget *status;	//progress bar indicating the status

	guint update_timeout;

	gint observed_cpu;
	gfloat observed_cpu_max_freq;
	int mib[MIB_MAX_SIZE];
	size_t miblen;
} BSDcpufreqPlugin;

void bsdcpufreq_init_cpu_data(BSDcpufreqPlugin *bsdcpufreq);
void bsdcpufreq_save(XfcePanelPlugin *plugin, BSDcpufreqPlugin *bsdcpufreq);

G_END_DECLS

#endif // __BSDCPUFREQ_H__

// vim:ts=8
