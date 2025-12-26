EXE = testCan

SOURCES = \
	main.c \
	coMgt/CanOpenMgt.c \
	coMgt/CanOpenBus.c \
	coMgt/CanOpenAction.c \
	Stub.c

OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -g -I.
LDFLAGS = -pthreads

all: $(EXE)

$(EXE): $(OBJECTS)
	gcc $(CFLAGS) $(SOURCES) -o $(EXE)

clean:
	rm $(EXE) $(OBJECTS)

.PHONY: all clean
