CXX= g++ -std=c++11 -fPIC -O3 -W -Wall

none : 
	@echo "Please do 'make {linux|mingw}'"

install : liblolita.a
	mkdir -p /usr/local/include/lolita
	cp liblolita.a /usr/local/lib/liblolita.a
	cp bmp.h /usr/local/include/lolita/bmp.h
	cp mat.hpp /usr/local/include/lolita/mat.hpp 
	cp pixel.h /usr/local/include/lolita/pixel.h 
	cp tools.h /usr/local/include/lolita/tools.h 
	cp lolita.h /usr/local/include/lolita/lolita.h

linux : liblolita.a liblolita.so 
	mkdir -p ./build/linux/include
	mkdir -p ./build/linux/lib
	cp liblolita.a ./build/linux/lib/liblolita.a
	cp liblolita.so ./build/linux/lib/liblolita.so
	cp bmp.h ./build/linux/include/bmp.h
	cp mat.hpp ./build/linux/include/mat.hpp 
	cp pixel.h ./build/linux/include/pixel.h 
	cp tools.h ./build/linux/include/tools.h 
	cp lolita.h ./build/linux/include/lolita.h

mingw : liblolita.a liblolita.dll 
	mkdir -p ./build/mingw/include
	mkdir -p ./build/mingw/lib
	cp liblolita.a ./build/mingw/lib/liblolita.a
	cp liblolita.dll ./build/mingw/lib/liblolita.dll
	cp bmp.h ./build/mingw/include/bmp.h
	cp mat.hpp ./build/mingw/include/mat.hpp
	cp pixel.h ./build/mingw/include/pixel.h 
	cp tools.h ./build/mingw/include/tools.h 
	cp lolita.h ./build/mingw/include/lolita.h
	
liblolita.so : pixel.o bmp.o tools.o
	$(CXX) -shared -o liblolita.so bmp.o pixel.o tools.o
	
liblolita.dll : pixel.o bmp.o tools.o
	$(CXX) -shared -o liblolita.dll bmp.o pixel.o tools.o
	
liblolita.a : pixel.o bmp.o tools.o
	ar rc liblolita.a bmp.o pixel.o tools.o
	
pixel.o : pixel.cpp pixel.h

bmp.o : bmp.cpp bmp.h mat.hpp pixel.h

tools.o : tools.cpp tools.h mat.hpp pixel.h

clean : 
	rm pixel.o bmp.o tools.o
