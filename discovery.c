#include "discovery.h"

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
   int scan_time)
{
   bluez_adapter_discovery(conn, 1); // Start discovery.

   sleep(scan_time); // scan_time 

   bluez_adapter_discovery(conn, 0); // Stop discovery.
   
   Device *devices_found = malloc(sizeof(Device) * init_size);

   *num_devices = bluez_adapter_get_objects(conn, devices_found, init_size);

   devices_found = realloc(devices_found, sizeof(Device) * (*num_devices));

   return (Device *)devices_found;
}

