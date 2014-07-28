SRCS=bsdcpufreq.c freq_funcs_freebsd.c dialog_about.c dialog_properties.c
PLUGINDIR=panel-plugin
CDEPS=`pkg-config --cflags gtk+-2.0 libxfce4panel-1.0 libxfce4ui-1`
LDEPS=`pkg-config --libs gtk+-2.0 libxfce4panel-1.0 libxfce4ui-1`
CC?=cc
CFLAGS+=-fPIC

all:
.for f in $(SRCS)
	$(CC) -Wall -c $(CFLAGS) $(CDEPS) $(PLUGINDIR)/$(f)
.endfor
	$(CC) -Wall $(CFLAGS) $(LDFLAGS) $(LDEPS) -shared -o libbsdcpufreq.so

clean:
	rm -f *.o libbsdcpufreq.so
