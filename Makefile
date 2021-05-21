.PHONY: all help test clean install uninstall

help: 
	@echo "Lolita is a header-only libraray, needn't build."
	@echo "help:"
	@echo "\tmake install - install this libraray"
	@echo "\tmake uninstall - uninstall this libraray"
	@echo "\tmake test - run the unit test"
	@echo "\tmake clean - clean the test build files"

all: test

test:
	make -C ./test

clean:
	make -C ./test clean

install:
	install -m0644 ./src/lolita.hpp /usr/local/include

uninstall:
	rm -f /usr/local/include/lolita.hpp
