all:
	gcc -shared -fPIC -o libdbgapi.so dbg_api.c
	sudo cp ./libdbgapi.so /usr/lib/libdbgapi.so
	sudo cp ./dbg_api.h /usr/include/dbg_api.h
	gcc -o userfile userfile.c -L. -ldbgapi
	./userfile
build:
	gcc -shared -fPIC -o libdbgapi.so dbg_api.c
clean:
	sudo rm /usr/lib/libdbgapi.so
	sudo rm /usr/include/dbg_api.h
