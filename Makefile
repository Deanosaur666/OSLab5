all: Dean_testFS Dean_testFS_debug

Dean_testFS : Dean_testFS.c Dean_libFS.c Dean_libFS.h
	@gcc Dean_testFS.c Dean_libFS.c -o Dean_testFS -Wall -lncurses

Dean_testFS_debug : Dean_testFS.c Dean_libFS.c Dean_libFS.h
	@gcc -g Dean_testFS.c Dean_libFS.c -o Dean_testFS_debug -Wall -lncurses

clean:
	@rm -f Dean_testFS