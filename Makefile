GCC = gcc
SCRS = *.c
LIBS = -lX11
LIBS += `pkg-config --libs gtk+-2.0`
#LIBS += `pkg-config --libs libbonoboui-2.0`
INC = `pkg-config --cflags gtk+-2.0`
#INC += `pkg-config --cflags libbonoboui-2.0`
PROG = bar
all:
	$(GCC) $(SCRS) $(LIBS) $(INC) -o $(PROG)

