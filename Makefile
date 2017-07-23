
NOW := $(shell date "+%Y%m%d%H%M%S")
HASH = $(shell git log --pretty=format:'%h' -n 1)

CC         = cc
#CFLAGS     = -g -Weverything
CFLAGS     = -g -std=c99
OUTFLAG    = -o 
DFLAGS    =            

LIBS       = -lm -lusb-1.0
TARGET     = cf-test

CF         = libcf2.o

all:  $(TARGET) 

.c.o : 
	$(CC) $(CFLAGS) -c $< 


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
