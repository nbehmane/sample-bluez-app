# tuxdrop v.1
- Tuxdrop is a wireless file transfer module that utilizes bluetooth. This is a preview verison of the application.
- The full release will be at github.com/nbehmane/tuxdrop [private for now].
- This project is under heavy development, so program structure may change drastically later.
## Dependencies
- Bluez-5.64-2
- dbus
- gio-2.0
- glib-2.0

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
