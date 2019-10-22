# Sample Makefile to show how the plugin is built
# The FreeBSD ports can use this directly
SRCS=bsdcpufreq.c freq_funcs_freebsd.c dialog_about.c dialog_properties.c
PLUGINDIR=panel-plugin
CDEPS=`pkg-config --cflags gtk+-3.0 libxfce4panel-2.0 libxfce4ui-2`
LDEPS=`pkg-config --libs gtk+-3.0 libxfce4panel-2.0 libxfce4ui-2`
CC?=cc
CFLAGS+=-fPIC

all:
.for f in $(SRCS)
	$(CC) -Wall -c $(CFLAGS) $(CDEPS) $(PLUGINDIR)/$(f)
.endfor
	$(CC) -Wall $(CFLAGS) $(LDFLAGS) $(LDEPS) *.o -shared -o libbsdcpufreq.so

clean:
	rm -f *.o libbsdcpufreq.so
