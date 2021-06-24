CC=gcc
CFLAGS=-I.

$(VERBOSE).SILENT:

.PHONY: install clean uninstall

all: install

install: tedi.c
	runner=`whoami` ; \
	if test $$runner != "root" ; \
	then \
		echo "You are not root" ; \
		echo "Installing Tedi in your current directory.." ; \
		$(CC) tedi.c -o tedi -Wall -Wextra -pedantic -std=c99 -lpthread ; \
	else \
		echo "You are root" ; \
		while [ -z "$$CONTINUE" ] ; do \
			read -r -p "Do you want to install Tedi in your system? [y/N]: " CONTINUE ; \
		done ; \
		[ $$CONTINUE = "y" ] || [ $$CONTINUE = "Y" ] || (echo "Installing Tedi in your current directory.."; $(CC) tedi.c -o tedi -Wall -Wextra -pedantic -std=c99 -lpthread;) ; \
		[ $$CONTINUE = "n" ] || [ $$CONTINUE = "N" ] || echo "Installing Tedi in your system.." ; $(CC) tedi.c -o "/bin/tedi" -Wall -Wextra -pedantic -std=c99 -lpthread; \
	fi

clean:
	@echo "Cleaning up..." ; \
	rm "tedi" ; \

uninstall:
	@echo "Uninstalling Tedi.." ; \
	rm "/bin/tedi" ; \

