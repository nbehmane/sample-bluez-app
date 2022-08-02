#ifndef DISCOVERY_H
#define DISCOVERY_H
#include <unistd.h>

#include <glib.h>
#include <gio/gio.h>

#include "dbus.h"
#include "bluez.h"

/** Funcs **/
/**
 * @brief Returns a pointer to a Device array. See bluez.h:Device for struct info.
 *
 * @param conn Connection handle to dbus.
 * @param init_size Size to initialize the size of Device array. Must be freed.
 * @param num_devices A pointer to an integer to hold the number of devices found.  
 * @param scan_time The amount of time in seconds to scan.
 */
Device *discovery_get_remote_devices(
   GDBusConnection *conn, 
   gsize init_size, 
   gsize *num_devices,
   int scan_time);

#endif // DISCOVERY_H
