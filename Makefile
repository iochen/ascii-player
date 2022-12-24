TARGET = asciiplayer
BUILDDIR = build
OBJDIR = obj
CC = clang
SUBMODULES = args channel log
OBJECTS = $(addprefix $(OBJDIR)/, main.o config.o display.o av.o apcache.o args/parse.o args/args.o channel/channel.o log/log.o)
LDFLAGS = -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lz -lbz2 -lncurses -lportaudio -lpthread
CCFLAGS = -Wall
FRAMEWORKFLAGS = $(addprefix -framework , CoreFoundation VideoDecodeAcceleration CoreVideo AudioToolbox VideoToolbox Security CoreMedia)
UNAME = $(shell uname)
OSFLAGS = 

# macOS
ifeq ($(UNAME), Darwin)
	OSFLAGS = -liconv $(FRAMEWORKFLAGS) 
endif

# Linux
ifeq ($(UNAME), Linux)
	OSFLAGS = -I/usr/include/
endif

$(TARGET): clean build 

$(OBJDIR)/%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

PREPARE:
	mkdir -p $(addprefix $(OBJDIR)/, $(SUBMODULES)) $(BUILDDIR)

build: PREPARE $(OBJECTS)
	$(CC) $(CCFLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(OSFLAGS)

clean:
	rm -rf $(OBJDIR) $(BUILDDIR)/$(TARGET)
