FLAGS := -Wall -Werror -g `pkg-config --cflags --libs glib-2.0 gtk4 gio-2.0`

all: build 
build:
	gcc $(FLAGS) -lcurses *.c *.h -o tuxdrop

test:
	gcc $(FLAGS) -lcurses test.c -o test

blue:
	modprobe btusb
	systemctl start bluetooth

docs:
	echo "Making docs..."
	doxygen doxyconfig

clean:
	rm tuxdrop 
	rm -r docs
