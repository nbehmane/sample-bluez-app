# tuxdrop
Tuxdrop is a wireless file transfer module that utilizes bluetooth.
## Dependencies
- Bluez-5.64-2
- dbus
- gio-2.0
- glib

## Development Dependencies
- Bluez-utils

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
