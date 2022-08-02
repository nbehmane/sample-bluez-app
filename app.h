#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include <glib.h>
#include <gio/gio.h>

#include "curses.h"
#include "dbus.h"
#include "bluez.h"
#include "discovery.h"

#define CLI 1



void sig_handler(int signo);
gboolean timeout_cb(gpointer arg);
gboolean idle_function(gpointer arg);

void handle_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data);


#if CLI
int app_main(int argc, char **argv);
#else
int app_main();
#endif

int app_discovery(int scan_time);

int app_debug_list_devices();

int app_list_devices();

int app_pair();

int app_connect();

int app_disconnect();

int app_remove();

int app_init();
int app_run();
int app_quit();


#endif
