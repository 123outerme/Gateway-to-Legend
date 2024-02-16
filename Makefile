CFLAGS = -O3
LDFLAGS =
appname = GatewayToLegend.exe

all: $(appname)
clean:
	rm -f $(appname) *.o
.PHONY: all clean

sdl_cflags := $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_image SDL2_mixer)
sdl_libs := $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer)
override CFLAGS += $(sdl_cflags)
override LIBS += $(sdl_libs)

$(appname): main.c mainToolchain.c outermeSDL.o SDLGateway.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -lm
	mv $(appname) ../execute
