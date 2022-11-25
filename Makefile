TARGET = asciiplayer
BUILDDIR = build
OBJDIR = obj
CC = clang
SUBMODULES = args
OBJECTS = $(addprefix $(OBJDIR)/, main.o config.o args/parse.o args/args.o av.o)
LDFLAGS = -lavcodec -lavformat -lavfilter -lavdevice -lavresample -lswscale -lavutil -lz -lbz2 -lncurses
FRAMEWORKFLAGS = $(addprefix -framework , CoreFoundation VideoDecodeAcceleration CoreVideo)

$(TARGET): PREPARE $(OBJECTS)
	$(CC) -o $(BUILDDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(FRAMEWORKFLAGS)

$(OBJDIR)/%.o : %.c
	$(CC) -c $< -o $@

PREPARE:
	mkdir -p $(addprefix $(OBJDIR)/, $(SUBMODULES)) $(BUILDDIR)
