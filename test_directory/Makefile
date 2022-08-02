FLAGS := -Wall -Werror -g `pkg-config --cflags --libs glib-2.0 gtk4 gio-2.0`

all: build 

build:
	gcc $(FLAGS) -lcurses *.c -o test

clean: 
	rm test
