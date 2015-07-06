
NOW := $(shell date "+%Y%m%d%H%M%S")
HASH = $(shell git log --pretty=format:'%h' -n 1)

CC         = cc
#CFLAGS     = -g -Weverything
CFLAGS     = -g 
OUTFLAG    = -o 
DFLAGS    =            

LIBS       = -L /usr/local/lib  -lm -lusb-1.0
TARGET     = cf-test

INCLUDEDIR = -I/usr/local/include/libusb-1.0

CF         = libcf2.o

all:  $(TARGET) 

.c.o : 
	$(CC) $(CFLAGS) -c $< $(INCLUDEDIR) 


clean:
	rm -f *.o *~ TAGS core $(TARGET) 

tags:
	etags *.[ch] *.[ch] 

push:
	git push --all

git_commit: clean
	@-git add *
	@-git commit -am"interim commit ${NOW}"


cf-test: libcf2.o  
	${CC} $(CFLAGS) -o $@ test.c $(CF) $(INCLUDEDIR) $(LIBS) 