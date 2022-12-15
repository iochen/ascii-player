TARGET = asciiplayer
BUILDDIR = build
OBJDIR = obj
CC = clang
SUBMODULES = args channel log
OBJECTS = $(addprefix $(OBJDIR)/, main.o config.o display.o args/parse.o args/args.o av.o channel/channel.o log/log.o)
LDFLAGS = -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lz -lbz2 -lncurses -lportaudio -liconv
CCFLAGS = -Wall
FRAMEWORKFLAGS = $(addprefix -framework , CoreFoundation VideoDecodeAcceleration CoreVideo AudioToolbox VideoToolbox Security CoreMedia)
UNAME = $(shell uname)
OSFLAGS = 

# macOS
ifeq ($(UNAME), Darwin)
	OSFLAGS = $(FRAMEWORKFLAGS)
endif

# Linux
ifeq ($(UNAME), Linux)
	OSFLAGS = -I/usr/include/
endif

$(TARGET): PREPARE $(OBJECTS)
	$(CC) $(CCFLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(OSFLAGS) -I/usr/include/

$(OBJDIR)/%.o : %.c
	$(CC) -I/usr/include/ -c $< -o $@

PREPARE:
	mkdir -p $(addprefix $(OBJDIR)/, $(SUBMODULES)) $(BUILDDIR)
