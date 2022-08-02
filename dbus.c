/**
* @file dbus.c
* @author Nima Behmanesh.
*
* @brief A helper library for GDBus.
*/
#include "dbus.h"

#define DEBUG 0

/**
* @brief Checks the error value, and if set, prints out the error message.
*
* @param error GError object.
*/
void dbus_check_error(GError *error)
{
   if (error != NULL)
      g_error(error->message);
}

/**
* @brief Creates a new dbus connection.
*
* @return conn GDBusConnection handle.
*/
GDBusConnection *dbus_connect_bus()
{
   GDBusConnection *conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
   if (conn == NULL) {
      g_error("Could not connect to system bus.");
      exit(EXIT_FAILURE);
   }
   return conn;
}

/**
* @brief Prints out the type value of a GVariant.
*
* @param val The GVariant value to figure out type.
*/
void dbus_print_type(GVariant *val)
{
   const gchar *type_info = NULL;
   type_info = g_variant_get_type_string(val);
   g_print("Type: %s\n", type_info);
}

/**
* @brief Prints the value of a GVariant.
* 
* @param val GVariant value to print out.
*/
void dbus_print_value(GVariant *val)
{
   const gchar *type_info = g_variant_get_type_string(val);
   switch (*type_info) {
      case 'v':
         GVariant *result = g_variant_get_child_value(val, 0);
         const gchar *temp = g_variant_get_type_string(result);
         g_print("TYPE OF V: %s\n", temp);
         dbus_print_value(result);
         g_variant_unref(result);
         break;
      case 'o':
         break;
      case 's':
         g_print("%s\n", g_variant_get_string(val, NULL));
         break; 
      case 'u':
         g_print("%u\n", g_variant_get_uint32(val));
         break;
      case 'b':
         gboolean temp_bool = g_variant_get_boolean(val);  
         gchar *temp_str = temp_bool ? "True" : "False";
         g_print("%s\n", temp_str);
         break;
      case 'a':
         /** TODO: Add more subtype capabilites **/
         if (*(type_info + 1) == 's') {
            gboolean iter_bool = 0;
            GVariantIter iter;
            const gchar *temp_str = NULL;

            g_variant_iter_init(&iter, val);
            while ((iter_bool = g_variant_iter_loop(&iter, "&s", &temp_str)))
               g_print("%s\n", temp_str); 
         } else {
            g_print("Unknown Type: %s\n", type_info);
         }
         break;
      default:
         g_print("Unknown Type: %s\n", type_info);
         break;
   }
}

/**
* @brief This will print dicts of a{sa*}
*  Really this is only for debugging and figuring out info.
*
* @param dict GVariant of type a{sa*}.
*/
void dbus_print_string_dict(GVariant *dict)
{
   GVariantIter iter_keys;
   const gchar *key_string = NULL;
   GVariant *value_array = NULL;
   gboolean iter_bool_key = 0;

   g_variant_iter_init(&iter_keys, dict);
   while ((iter_bool_key = 
            g_variant_iter_loop(&iter_keys,
            "{&s@a*}",
            &key_string,
            &value_array)))
   {
      g_print("Key: %s\n", key_string);
   }
   // DOUBLE CHECK THIS!
   // The iterator doesn't have to be freed, since it'll be gone once 
   // the dict is freed.
}

/**
* @brief Prints out key and value in a dictionary.
*
* @param dict GVariant of type a{sa*}.
*/
void dbus_print_dict_val(GVariant *dict)
{
   GVariantIter iter_keys;
   const gchar *key_string = NULL;
   GVariant *value = NULL;
   gboolean iter_bool_key = 0;

   g_variant_iter_init(&iter_keys, dict);
   while ((iter_bool_key = 
            g_variant_iter_loop(&iter_keys,
            "{&s@v}",
            &key_string,
            &value)))
   {
      g_print("Key: %s\n", key_string);
      dbus_print_value(value);
   }
}

/*
 * @brief Closes a DBus connection.
 *
 * @param conn A connection to dbus.
 */
void dbus_close_bus(GDBusConnection *conn)
{
   gboolean close_connection_result = g_dbus_connection_close_sync(conn, NULL, NULL);
   const gchar *closed = close_connection_result ? "True" : "False";
   #if DEBUG
   g_print("Connection Closed Result: %s\n", closed);
   #endif
   g_object_unref(conn); // Cleanup the connection | NOT WORKING RIGHT NOW
   // GCC errors
   (void)closed;
}
