all: Dean_testFS

Dean_testFS : Dean_testFS.c Dean_libFS.c Dean_libFS.h
	@gcc Dean_testFS.c Dean_libFS.c -o Dean_testFS -Wall -lncurses

clean:
	@rm -f Dean_testFS