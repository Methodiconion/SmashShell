PRIMARYOBJ = smash.o
LIBOBJS = history.o commands.o
LIBS = smashLib.a
CFLAGS = -Wall -std=c99
DFLAGS = -MM
EXE = smash
CC = gcc

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)	

$(LIBS): $(LIBOBJS)
	ar r $@ $?
	
$(EXE): $(PRIMARYOBJ) $(LIBS)
	$(CC) -o $@ $^
	
clean:
	rm -f $(PRIMARYOBJ) $(LIBOBJS) $(EXE) $(LIBS)
