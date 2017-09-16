CC=gcc
YACC=bison
LEX=flex

CSOURCES=src/json.struct.c src/minire.c src/output-*.c

BINDIR=/usr/local/bin

all: jsonc jsonc-doxygen
	

jsonc: src/jsonc.o
	cp $< $@

jsonc-doxygen: jsonc
	@echo "#!/bin/sh" > $@
	@echo './jsonc --doxygen $${1}' >> $@
	@chmod +x ./jsonc-doxygen

%.o: %.lex.c %.tab.c %.tab.h $(CSOURCES)
	$(CC) -o $@ $^ -lfl

%.lex.c: %.l
	$(LEX) -o $(patsubst %.l,%.lex.c,$<) $<

%.tab.c %.tab.h: %.y
	$(YACC) --defines=$(patsubst %.y,%.tab.h,$<) --output=$(patsubst %.y,%.tab.c,$<) $<

.PHONY: clean

clean:
	rm -f src/*.lex.c src/*.tab.c src/*.tab.h src/*.o jsonc jsonc-doxygen

install: jsonc jsonc-doxygen
	cp ./jsonc $(DESTDIR)$(BINDIR)/jsonc
	cp ./jsonc-doxygen $(DESTDIR)$(BINDIR)/jsonc-doxygen

uninstall:
	rm $(DESTDIR)$(BINDIR)/jsonc
	rm $(DESTDIR)$(BINDIR)/jsonc-doxygen

remove: uninstall
	
