/**
* @file bluez.c
* @author Nima Behmanesh
* @brief This is a helper library that wraps up some bluez stuff.
*/
#include "bluez.h"

#define DEBUG 0

/**
* @brief Scans for advertisements and returns object names found.
*
* @param conn Connection handle to dbus.
* @param devices_found An array of Devices.
* @param object_path_array_size The max amount of devices that can be found array.
*
* @returns The size of the Devices struct array found.
*/
gsize bluez_adapter_get_objects(
   GDBusConnection *conn, 
   Device *devices_found, 
   gsize object_path_array_size)
{
   GVariant *result = NULL;
   GVariant *array_of_objects = NULL; 
   GError *error = NULL;

   result = g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            "/",
            FREE_OBJECT_MANAGER,
            "GetManagedObjects",
            NULL,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
   dbus_check_error(error);

   array_of_objects = g_variant_get_child_value(result, 0);

   gboolean iter_bool = 0;
   GVariantIter iter;
   GVariant *object = NULL;
   GVariant *interface_array = NULL;
   int i = 0;

   g_variant_iter_init(&iter, array_of_objects);
   while ((iter_bool = 
            g_variant_iter_loop(&iter, 
            "{@o@a*}", 
            &object, 
            &interface_array))) 
   {

      // These strings will have to be freed externally.
      devices_found[i].obj_path = g_variant_print(object, FALSE);


      /**** INTERFACES START****/
      GVariantIter iter_interfaces;
      const gchar *interface_string = NULL;
      GVariant *property_array = NULL;
      gboolean iter_bool_interface = 0;

      int num_ifaces = 0;
      g_variant_iter_init(&iter_interfaces, interface_array);
      while ((iter_bool_interface = 
               g_variant_iter_loop(&iter_interfaces,
               "{&s@a*}",
               &interface_string,
               &property_array)))
      {
         // Must be freed.
         devices_found[i].ifaces[num_ifaces].iface = g_strdup(interface_string);

         /**** METHODS START ****/
         GVariantIter iter_properties;
         const gchar *property_string = NULL;
         GVariant *val = NULL;
         gboolean iter_bool_property = 0;
         g_variant_iter_init(&iter_properties, property_array);

         int num_properties = 0;
         while ((iter_bool_property = 
                  g_variant_iter_loop(&iter_properties,
                  "{&s@v}",
                  &property_string,
                  &val)))
         {
            // Must be freed
            devices_found[i].ifaces[num_ifaces].props[num_properties].prop = g_strdup(property_string);
            // Must be freed
            devices_found[i].ifaces[num_ifaces].props[num_properties].val = g_variant_get_variant(val);

            num_properties += 1;
            if (num_properties > MAX_PROPS - 1)
               break;
         }
         devices_found[i].ifaces[num_ifaces].num_properties = num_properties;
         num_ifaces += 1;
         if (num_ifaces > MAX_IFACES - 1)
            break;
      }

      devices_found[i].num_ifaces = num_ifaces;

      i += 1;
      if (i > object_path_array_size - 1)
         break;

   }

   g_variant_unref(array_of_objects);
   g_variant_unref(result);
   return i;
}


/**
* @brief Starts and stops device discovery. 
*
* @param conn A GDBusConnection handle. 
* @param power 1 to start discovery, 0 to stop.
*/
void bluez_adapter_discovery(GDBusConnection *conn, gboolean power) 
{
   GVariant *result = NULL;
   GError *error = NULL;
   gchar *method = power ? "StartDiscovery" : "StopDiscovery";

   result = g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            BLUEZ_ADAPTER_OBJECT,
            BLUEZ_ADAPTER_IFACE,
            method,
            NULL,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);

   dbus_check_error(error);
   g_variant_unref(result);
}

/**
* @brief Sets a property.
*
* @param conn Connection handle to dbus.
* @param prop Property to be changed.
* @param val Must be used with g_variant_new object, will be freed. 
*/
void bluez_adapter_set_property(GDBusConnection *conn, 
   const gchar *prop,
   GVariant *val)
{
   GVariant *result = NULL;
   GError *error = NULL;

   result = g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            BLUEZ_ADAPTER_OBJECT,
            FREE_PROPERTIES,
            "Set",
            g_variant_new("(ssv)", BLUEZ_ADAPTER_IFACE, prop, val),
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);

   dbus_check_error(error);
   g_variant_unref(result);
}

/**
* @brief Prints the properties of hci0 controller. 
*
* @param conn A GDBusConnection handle.
*/
void bluez_adapter_print_properties(GDBusConnection *conn)
{

   GError *error = NULL;
   GVariant *response = NULL;
   GVariant *result = NULL;
   const char *property_string;
   GVariant *val = NULL;
   gboolean iter_bool = 0;
   GVariantIter iter;

   response =  g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            BLUEZ_ADAPTER_OBJECT,
            "org.freedesktop.DBus.Properties",
            "GetAll",
            g_variant_new("(s)", BLUEZ_ADAPTER_IFACE),
            G_VARIANT_TYPE("(a{sv})"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);

   dbus_check_error(error);
   result = g_variant_get_child_value(response, 0);

   g_variant_iter_init(&iter, result);
   while ((iter_bool = g_variant_iter_loop(&iter, "{&sv}", &property_string, &val))) {
      g_print("Property: %s\n", property_string);
      dbus_print_value(val);
   }
   g_print("---------------------------------------\n");
   
   /******** CLEANUP ********/
   g_variant_unref(response);
   g_variant_unref(result);
}

/**
 * @brief Prints out every device found after scanning. Debug function.
 *
 * @param devices Pointer to Device array.
 * @param num_devices Size of Device array.
 */
void bluez_print_devices(Device *devices, gsize num_devices)
{
   if (devices == NULL)
   {
      fprintf(stderr, "tuxdrop: There are no devices.\n");
      exit(EXIT_FAILURE);
   }
   for (int i = 0; i < num_devices; i++)
   {
      Device dev = devices[i];
      g_print("obj: %s\n", dev.obj_path);
      g_print("num_ifaces: %d\n", dev.num_ifaces);
      for (int j = 0; j < dev.num_ifaces; j++)
      {
         Iface iface = dev.ifaces[j];
         g_print(" | iface: %s\n", iface.iface);
         for (int k = 0; k < iface.num_properties; k++)
         {
            Prop prop = iface.props[k];
            g_print("  | prop: %s\n", prop.prop);
            gchar *val = g_variant_print(prop.val, FALSE);
            g_print("   | val: %s\n", val);
            free(val);
         }
      }
   }
}

/**
 * @brief Frees all Device in the devices array.
 *
 * @param devices Pointer to Device array.
 * @param num_devices Size of Device array.
 */
void bluez_devices_free(Device *devices, gsize num_devices)
{
   if (devices == NULL)
      return;
   for (int i = 0; i < num_devices; i++)
   {

      Device dev = devices[i];
      free(dev.obj_path);
      for (int j = 0; j < dev.num_ifaces; j++)
      {
         Iface iface = dev.ifaces[j];
         free(iface.iface);
         for (int k = 0; k < iface.num_properties; k++)
         {
            Prop prop = iface.props[k];
            free(prop.prop);
            g_variant_unref(prop.val);
         }
      }
   }
   free(devices);
}

/**
* @brief Connects a device. 
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_connect(Device dev, GDBusConnection *conn)
{
   GError *error = NULL;
   char path[40];
   obj_to_str(dev.obj_path, path);

   g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            path,
            "org.bluez.Device1",
            "Connect",
            NULL,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
   dbus_check_error(error);
   g_print("Connected to %s\n", dev.obj_path);
}

/**
* @brief A way to select devices. 
*
* @param devices A Device struct array.
* @param num_devices The number of devices in the array.
*/
Device bluez_choose_device(Device *devices, int num_devices)
{
   for (int i = 0; i < num_devices; i++)
      g_print("%d | %s\n", i, devices[i].obj_path);  
   
   g_print("Input Dev Index: ");
   // ADD ERROR CHECK HERE THIS IS TOTALLY NOT SAFE LOL
   int index = 0;
   scanf("%d", &index);

   
   return devices[index];
}

/**
* @brief Pairs a device. 
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_pair(Device dev, GDBusConnection *conn)
{
   GError *error = NULL;
   char path[40];
   obj_to_str(dev.obj_path, path);

   g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            path,
            "org.bluez.Device1",
            "Pair",
            NULL,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
   dbus_check_error(error);
}

/**
* @brief Disconnects a device. Removes all pairing information.
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_disconnect(Device dev, GDBusConnection *conn)
{
   GError *error = NULL;
   char path[40];
   obj_to_str(dev.obj_path, path);

   g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            path,
            "org.bluez.Device1",
            "Disconnect",
            NULL,
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
   dbus_check_error(error);
}

/**
 * @brief Calls a given method with parameters to the agent manager.
 *
 * @param method The method to be called.
 * @param param The parameters to be passed to the method.
 * @param conn Connection handle to dbus.
 */
int bluez_agent_call_method(const gchar *method, GVariant *param, GDBusConnection *conn)
{
   GVariant *result;
   GError *error = NULL;

   result = g_dbus_connection_call_sync(conn,
                                     "org.bluez",
                                     "/org/bluez",
                                     "org.bluez.AgentManager1",
                                     method,
                                     param,
                                     NULL,
                                     G_DBUS_CALL_FLAGS_NONE,
                                     -1,
                                     NULL,
                                     &error);
   if(error != NULL) 
   {
      g_print("Register %s: %s\n", AGENT_PATH, error->message);
      return 1;
   }

   g_variant_unref(result);
   return 0;
}

/**
 * @brief Registers the autopair agent.
 *
 * @param conn Connection handle to dbus.
 */
int bluez_register_autopair_agent(GDBusConnection *conn)
{
	int rc;

	rc = bluez_agent_call_method("RegisterAgent", g_variant_new("(os)", AGENT_PATH, "NoInputNoOutput"), conn);
	if(rc)
		return 1;

	rc = bluez_agent_call_method("RequestDefaultAgent", g_variant_new("(o)", AGENT_PATH), conn);
	if(rc) {
		bluez_agent_call_method("UnregisterAgent", g_variant_new("(o)", AGENT_PATH), conn);
		return 1;
	}

	return 0;
}
/**
* @brief Connects to a device using the bt address. 
*
* @param bt_addr Bluetooth address
* @param conn Connection handle to dbus.
*/
void bluez_adapter_connect_addr(char *bt_addr, char *path, GDBusConnection *conn)
{
   GError *error = NULL;
   g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            "/org/bluez/hci0",
            "org.bluez.Adapter1",
            "ConnectDevice",
            g_variant_new("(o)", path),
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);

   dbus_check_error(error);
   g_print("%s removed.\n", path);
}


/**
* @brief Removes a device. Removes all pairing information.
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_adapter_remove_device(Device dev, GDBusConnection *conn)
{
   GError *error = NULL;
   char path[40];
   obj_to_str(dev.obj_path, path);
   g_dbus_connection_call_sync(conn,
            BLUEZ_ORG,
            "/org/bluez/hci0",
            "org.bluez.Adapter1",
            "RemoveDevice",
            g_variant_new("(o)", path),
            NULL,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);

   dbus_check_error(error);
   g_print("%s removed.\n", dev.obj_path);
}

/**
 * @brief Internal function to convert paths. This will be moved. It's not safe...
 */
void obj_to_str(char *obj_path, char *path)
{
   // This will have to go in a different place
   for (int i = 1; i < 38; i++)
      path[i - 1] = obj_path[i];
   path[37] = '\0';
}
