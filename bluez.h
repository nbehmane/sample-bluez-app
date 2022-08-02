/** @file bluez.h 
*  @author Nima Behmanesh.
**/
#ifndef BLUEZ_H_HELPER
#define BLUEZ_H_HELPER

/** Defines **/
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>

#include "dbus.h"

/** Macros **/
#define BLUEZ_ORG "org.bluez" /** Bluez Org **/
#define BLUEZ_ADAPTER_IFACE "org.bluez.Adapter1"
#define BLUEZ_ADAPTER_OBJECT "/org/bluez/hci0" 
#define FREE_PROPERTIES "org.freedesktop.DBus.Properties"
#define FREE_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"
#define AGENT_PATH "/org/bluez/AutoPinAgent"
#define MAX_IFACES 10
#define MAX_PROPS  20


/** @brief GLib Prop struct */
typedef struct _Prop
{
   char *prop;
   GVariant *val;
   
} Prop;

/** @brief GLib Iface struct */
typedef struct _Iface
{
   char *iface;
   int num_properties;
   Prop props[MAX_PROPS];
} Iface;

/** @brief A preliminary struct to hold device information. */
typedef struct _Device
{
   char *obj_path; 
   int num_ifaces;
   Iface ifaces[MAX_IFACES];
} Device;

/**
* @brief Prints the properties of hci0 controller. 
*
* @param conn A GDBusConnection handle.
*/
void bluez_adapter_print_properties(GDBusConnection *conn);

/**
* @brief Sets a property.
*
* @param conn Connection handle to dbus.
* @param prop desc.
* @param val desc.
*/
void bluez_adapter_set_property(
   GDBusConnection *conn, 
   const gchar *prop,
   GVariant *val);

/**
* @brief Starts and stops device discovery. 
*
* @param conn A GDBusConnection handle. 
* @param power 1 to start discovery, 0 to stop.
*/
void bluez_adapter_discovery(GDBusConnection *conn, gboolean power);

/**
* @brief Scans for advertisements and returns object names found.
*
* @brief TODO: This information will all need to be stored in structs.
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
   gsize object_path_array_size);

/**
 * @brief Prints out every device found after scanning. Debug function.
 *
 * @param devices Pointer to Device array.
 * @param num_devices Size of Device array.
 */
void bluez_print_devices(Device *devices, gsize num_devices);

/**
 * @brief Frees all Device in the devices array.
 *
 * @param devices Pointer to Device array.
 * @param num_devices Size of Device array.
 */
void bluez_devices_free(Device *devices, gsize num_devices);

/**
* @brief Connects a device. 
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_connect(Device dev, GDBusConnection *conn);

/**
* @brief Pairs a device. 
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_pair(Device dev, GDBusConnection *conn);

/**
* @brief A way to select devices. 
*
* @param devices A Device struct array.
* @param num_devices The number of devices in the array.
*/
Device bluez_choose_device(Device *devices, int num_devices);

/**
 * @brief Calls a given method with parameters to the agent manager.
 *
 * @param method The method to be called.
 * @param param The parameters to be passed to the method.
 * @param conn Connection handle to dbus.
 */
int bluez_agent_call_method(const gchar *method, GVariant *param, GDBusConnection *conn);

/**
 * @brief Registers the autopair agent.
 *
 * @param conn Connection handle to dbus.
 */
int bluez_register_autopair_agent(GDBusConnection *conn);

/**
* @brief Disconnects a device. Removes all pairing information.
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*/
void bluez_device_disconnect(Device dev, GDBusConnection *conn);

/**
* @brief Removes a device. Removes all pairing information.
*
* @param dev A Device struct.
* @param conn Connection handle to dbus.
*
*/
void bluez_adapter_remove_device(Device dev, GDBusConnection *conn);

/**
 * @brief Internal function to convert paths. This will be moved. It's not safe...
 */
void obj_to_str(char *obj_path, char *path);

#endif // BLUEZ_H_HELPER
