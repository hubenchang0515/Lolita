.PHONY: help install uninstall test

help: 
	@echo "Lolita is a header-only libraray, needn't build."
	@echo "help:"
	@echo "  make install - install this libraray"
	@echo "  make uninstall - uninstall this libraray"
	@echo "  make test - run the unit test"
	@echo "  make clean - clean the test build files"

install:
	install -m0644 ./src/lolita.hpp /usr/local/include

uninstall:
	rm -f /usr/local/include/lolita.hpp

test:
	$(MAKE) -C ./test test

%:
	$(MAKE) -C ./test $@