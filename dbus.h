/**
* @file dbus.h
* @author Nima Behmanesh.
*/
#ifndef DBUS_H
#define DBUS_H
#include <glib.h>
#include <gio/gio.h>

/** Functions */
/**
* @brief Creates a new dbus connection.
*
* @return conn GDBusConnection handle.
*/
GDBusConnection *dbus_connect_bus();

/**
* @brief Prints out the type value of a GVariant.
*
* @param val GVariant value to print out.
*/
void dbus_print_type(GVariant *val);

/**
* @brief Checks the error value, and if set, prints out the error message.
*
* @param error GError object.
*/
void dbus_check_error(GError *error);

/**
* @brief Prints the value of a GVariant.
* 
* @param val GVariant value to print out.
*/
void dbus_print_value(GVariant *val);

/**
* @brief This will print dicts of a{sa*}
*  Really this is only for debugging and figuring out info.
*
* @param dict GVariant of type a{sa*}.
*/
void dbus_print_string_dict(GVariant *dict);

/**
* @brief Prints out key and value in a dictionary.
*
* @param dict GVariant of type a{sa*}.
*/
void dbus_print_dict_val(GVariant *dict);

/*
 * @brief Closes a DBus connection.
 *
 * @param conn A connection to dbus.
 */
void dbus_close_bus(GDBusConnection *conn);

#endif // DBUS_H
