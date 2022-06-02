 .PHONY: lib src

src: lib
		cd src && make

lib:
		cd lib && make