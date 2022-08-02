#include "app.h"
#define DEV_ARRAY_INIT_SIZE 40
#define CLI 1

static GDBusConnection *conn = NULL;
static GMainLoop *main_loop = NULL;
static GSource *timeout_source = NULL;
static Device *devices = NULL;
static gsize num_devices = 0;
static guint prop_changed;
#if CLI
static struct option long_options[] = 
{
   {"help", no_argument, 0, 'h'},
   {"scan", required_argument, 0, 's'},
   {"debug", no_argument, 0, 'e'},
   {"list", no_argument, 0, 'l'},
   {"pair", no_argument, 0, 'p'},
   {"connect", no_argument, 0, 'c'},
   {"remove", no_argument, 0, 'r'},
   {"quit", no_argument, 0, 'q'},
   {0, 0, 0, 0}
};
#endif

/***** MAIN SIGNAL HANDLERS *****/
void handle_properties_changed(GDBusConnection *sig,
				const gchar *sender_name,
				const gchar *object_path,
				const gchar *interface,
				const gchar *signal_name,
				GVariant *parameters,
				gpointer user_data)
{
	(void)sig;
	(void)sender_name;
	(void)object_path;
	(void)interface;
	(void)signal_name;
   (void)parameters;
   (void)user_data;
   g_print("Sender Name: %s\n", sender_name);
   g_print("Object Path: %s\n", object_path);
   g_print("Interface  : %s\n", interface);
   g_print("Signal Name: %s\n", signal_name);
   g_print(g_variant_print(parameters, FALSE));
   g_print("\n");
}

int app_run()
{
   g_main_loop_run(main_loop);
   return 0;
}

int app_quit()
{
   g_print("Shutting down\n");
   g_main_loop_unref(main_loop);
   g_source_unref(timeout_source);
	g_dbus_connection_signal_unsubscribe(conn, prop_changed);

   bluez_adapter_set_property(conn, "Pairable", g_variant_new_boolean(0));
   // Turn off the adapter.
   bluez_adapter_set_property(conn, "Powered", g_variant_new_boolean(0));

   /****** CLOSE CONNECTION START ******/
   dbus_close_bus(conn);
   /****** CLOSE CONNECTION END ******/

   /****** CLEANUP START ******/
   bluez_devices_free(devices, num_devices);
   /****** CLEANUP END ******/
   return 0;
}

int app_init()
{
   conn = dbus_connect_bus(); // Establish a connection with dbus

   //! Main loop settings.
   main_loop = g_main_loop_new(NULL, FALSE);
   timeout_source = g_timeout_source_new_seconds(45);
   g_source_set_callback(timeout_source,
                        (GSourceFunc)timeout_cb,
                        main_loop,
                        NULL);
   g_source_attach(timeout_source, NULL);
   g_idle_add((GSourceFunc)idle_function, NULL);
   


   //! Adapter properties and agent setup.
   bluez_adapter_set_property(conn, "Powered", g_variant_new_boolean(1));
   bluez_adapter_set_property(conn, "Pairable", g_variant_new_boolean(1));
   int rc = bluez_register_autopair_agent(conn);
   if (rc) 
      fprintf(stderr, "CRITICAL ERROR\n");

   //! Initial device array.
   devices = (Device *)malloc(sizeof(Device) * DEV_ARRAY_INIT_SIZE);
   num_devices = bluez_adapter_get_objects(conn, devices, DEV_ARRAY_INIT_SIZE);
   devices = realloc(devices, sizeof(Device) * (num_devices));
   return 0;
}

/**** SIGNAL HANDLERS ****/
void sig_handler(int signo)
{
   #if CLI
   if (signo == SIGINT)
   {
      app_quit();
      exit(EXIT_SUCCESS);
   }
   #else
   if (signo == SIGINT)
      g_main_loop_quit(main_loop);
   #endif
}
/**** END SIGNAL HANDLERS ****/

/**** GMAINLOOP CALLBACKS ****/
gboolean timeout_cb(gpointer arg)
{
   return TRUE;
}

gboolean idle_function(gpointer arg)
{
   int ret = -1;
   #if CLI
   ret = app_main(0, NULL);
   #else
   ret = app_main();
   #endif

   if (ret == -1)
      g_main_loop_quit(main_loop);

   if (signal(SIGINT, sig_handler) == SIG_ERR)
   {
      fprintf(stderr, "Couldn't catch signal.\n");
      g_main_loop_quit(main_loop);
   }
   return TRUE;
}

void usage()
{
   fprintf(stderr, "\t-s x Scan for x seconds.\n");
   fprintf(stderr, "\t-l List current devices.\n");
   fprintf(stderr, "\t-e List debug info.\n");
   fprintf(stderr, "\t-c Connect to a device.\n");
   fprintf(stderr, "\t-d Disconnect from a device.\n");
   fprintf(stderr, "\t-r Remove a device.\n");
   fprintf(stderr, "\t-p Pair a device.\n");
}

int app_discovery(int scan_time)
{
   bluez_devices_free(devices, num_devices);
   printf("Scanning...\n");
   devices = discovery_get_remote_devices(conn, DEV_ARRAY_INIT_SIZE, &num_devices, scan_time);
   printf("Done scanning\n");
   return 0;
}

int app_debug_list_devices()
{
   bluez_print_devices(devices, num_devices);
   return 0;
}

int app_list_devices()
{
   bluez_devices_free(devices, num_devices);

   devices = (Device *)malloc(sizeof(Device) * DEV_ARRAY_INIT_SIZE);
   num_devices = bluez_adapter_get_objects(conn, devices, DEV_ARRAY_INIT_SIZE);
   devices = realloc(devices, sizeof(Device) * (num_devices));
   
   for (int i = 0; i < num_devices; i++)
      g_print("%d | %s\n", i, devices[i].obj_path);  
   return 0;
}

int app_pair()
{
   Device temp_dev = bluez_choose_device(devices, num_devices);
   char path[40];
   bluez_device_pair(temp_dev, conn); 
   obj_to_str(temp_dev.obj_path, path);
   g_print(path);
   prop_changed = g_dbus_connection_signal_subscribe(conn,
		"org.bluez",
		"org.freedesktop.DBus.Properties",
		"PropertiesChanged",
		path,
		"org.bluez.Device1",
		G_DBUS_SIGNAL_FLAGS_NONE,
		handle_properties_changed,
		NULL,
		NULL);
   
   return 0;
}

int app_connect()
{
   Device temp_dev = bluez_choose_device(devices, num_devices);
   char path[40];
   bluez_device_connect(temp_dev, conn);
   obj_to_str(temp_dev.obj_path, path);
   prop_changed = g_dbus_connection_signal_subscribe(conn,
		"org.bluez",
		"org.freedesktop.DBus.Properties",
		"PropertiesChanged",
		path,
		"org.bluez.Device1",
		G_DBUS_SIGNAL_FLAGS_NONE,
		handle_properties_changed,
		NULL,
		NULL);
   return 0;
}

int app_disconnect()
{
   Device temp_dev = bluez_choose_device(devices, num_devices);
   bluez_device_disconnect(temp_dev, conn);
   return 0;
}


int app_remove()
{
   Device temp_dev = bluez_choose_device(devices, num_devices);
   bluez_adapter_remove_device(temp_dev, conn);
   return 0;
}

#if CLI
int app_main(int argc, char **argv)
#else
int app_main()
#endif
{
   #if CLI 
   int c = 0;
   signal(SIGINT, sig_handler);
   while (1)
   {
      int option_index = 0;
      c = getopt_long(argc, argv, "hs:elpcdrq", long_options, &option_index);
      if (c == -1)
         break;
      switch (c)
      {
         case 'h':
            usage();
            break;
         case 's':
            int time_seconds = atoi(optarg);
            app_discovery(time_seconds);
            break;
         case 'e': // List devices
            app_debug_list_devices();
            break;
         case 'l': // List devices
            app_list_devices();
            break;
         case 'p': // Pairing
            app_pair();
            break;
         case 'c': // Connecting
            app_connect();
            break;
         case 'd': // Disconnect
            app_disconnect();
            break;
         case 'r': // Remove device
            app_remove();
            break;
         case 'q':
            return -1;
         default:
            break;
      }
   }
   #else
   char c = 0;
   scanf("%c", &c);
   if (c != -1)
   {
      switch(c)
      {
         case 's': // Discovery
            app_discovery(5);
            break;
         case 'e': // List devices
            app_debug_list_devices();
            break;
         case 'l': // List devices
            app_list_devices();
            break;
         case 'p': // Pairing
            app_pair();
            break;
         case 'c': // Connecting
            app_connect();
            break;
         case 'd': // Disconnect
            app_disconnect();
            break;
         case 'r':
            app_remove();
            break;
         case 'q':
            return -1;
         default:
            break;
      }
   }
   #endif
   return 0;
}
