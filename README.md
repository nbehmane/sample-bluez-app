# Sample BlueZ Application
- An example project for using BlueZ to create Bluetooth applications using GLib's DBus bindings.
- This is a snapshot of development for a different project.

## Dependencies
- Bluez-5.64-2
- dbus
- gio-2.0
- glib-2.0

## Development Dependencies
- Bluez-utils

## Features
- BLE Scanning and Connecting

## Features Under Development
- Curses GUI.
- Asynchronous event driven framework.

## Generate docs.
- make docs

## How to use.
- modprobe btusb
- systemctl start bluetooth
- make
- ./tuxdrop 

## Sources
- https://dbus.freedesktop.org/doc/dbus-tutorial.html
- https://stackoverflow.com/questions/67989440/glib-for-bluetooth-client-server
- https://www.linumiz.com/author/parthiban/
