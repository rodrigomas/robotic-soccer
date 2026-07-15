UNAME_S := $(shell uname -s)
PKG_CONFIG ?= pkg-config

CXX ?= g++
CXXFLAGS ?= -g -Wall
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?=
USE_SOUND ?= 1
USE_MUSIC ?= 1
LEVEL ?= -O2 -pipe
ARCH ?=

TARGET_DIR = ../output/$(shell uname -s | tr A-Z a-z)
TARGET = $(TARGET_DIR)/soccer

INCPATH = -I.
LIBPATH =
LIBS = -lm -lpthread

ifeq ($(UNAME_S),Darwin)
BREW_PREFIX ?= /opt/homebrew
INCPATH += -Iplatform/darwin
INCPATH += $(shell $(PKG_CONFIG) --cflags glew luajit libxml-2.0 libjpeg vorbisfile ogg 2>/dev/null)
INCPATH += -I$(BREW_PREFIX)/include -I$(BREW_PREFIX)/opt/openal-soft/include
LIBPATH += -L$(BREW_PREFIX)/lib -L$(BREW_PREFIX)/opt/openal-soft/lib
LIBS += $(shell $(PKG_CONFIG) --libs glew luajit libxml-2.0 libjpeg vorbisfile ogg 2>/dev/null)
LIBS += -framework OpenGL -framework GLUT -lopenal
else
INCPATH += -I/usr/X11R6/include/ -I/usr/include -I/usr/include/libxml2 -I/usr/local/include
INCPATH += -Iinclude
LIBPATH += -L/usr/lib -L/usr/X11R6/lib/ -L/usr/local/lib -Llibs/linux
LIBS += -llua5.1 -lglut -lGLU -lGL -lGLEW -ljpeg -lxml2 -lstdc++ -lopenal -logg -lvorbis -lvorbisfile
ARCH ?= -march=pentium4
LEVEL ?= -O3 -msse -msse2 -msse3 -mmmx -pipe
endif

ifeq ($(USE_SOUND),1)
WSOUND = -DUSE_SOUND
endif

ifeq ($(USE_MUSIC),1)
WMUSIC = -DUSE_MUSIC
endif

FILES = defs.o vector3d.o camera.o tga.o wav.o ogg.o keyhelper.o lights.o color.o rigidbody.o info.o scene.o jpeg.o gamedata.o sound.o libsoccer.o button.o fade.o player.o physics.o scene_01.o scene_02.o scene_03.o scene_04.o scene_05.o scene_06.o sky.o placar.o soccerfield.o flagman.o ball.o rules.o

$(TARGET): main.o
	mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(ARCH) $(LEVEL) -o $(TARGET) main.o $(FILES) $(LIBPATH) $(LDFLAGS) $(LIBS) $(LDLIBS)

physics.o: physics.cpp physics.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c physics.cpp -Wall $(LEVEL) $(ARCH)

defs.o: defs.cpp defs.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c defs.cpp -Wall $(LEVEL) $(ARCH)

libsoccer.o: libsoccer.cpp libsoccer.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c libsoccer.cpp -Wall $(LEVEL) $(ARCH)

fade.o: fade.cpp fade.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c fade.cpp -Wall $(LEVEL) $(ARCH)

scene_01.o: scene_01.cpp scene_01.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_01.cpp -Wall $(LEVEL) $(ARCH)

scene_02.o: scene_02.cpp scene_02.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_02.cpp -Wall $(LEVEL) $(ARCH)

scene_03.o: scene_03.cpp scene_03.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_03.cpp -Wall $(LEVEL) $(ARCH)

scene_04.o: scene_04.cpp scene_04.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_04.cpp -Wall $(LEVEL) $(ARCH)

scene_05.o: scene_05.cpp scene_05.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_05.cpp -Wall $(LEVEL) $(ARCH)

scene_06.o: scene_06.cpp scene_06.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_06.cpp -Wall $(LEVEL) $(ARCH)

button.o: button.cpp button.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c button.cpp -Wall $(LEVEL) $(ARCH)

rules.o: rules.cpp rules.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c rules.cpp -Wall $(LEVEL) $(ARCH)

sky.o: sky.cpp sky.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c sky.cpp -Wall $(LEVEL) $(ARCH)

placar.o: placar.cpp placar.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c placar.cpp -Wall $(LEVEL) $(ARCH)

soccerfield.o: soccerfield.cpp soccerfield.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c soccerfield.cpp -Wall $(LEVEL) $(ARCH)

flagman.o: flagman.cpp flagman.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c flagman.cpp -Wall $(LEVEL) $(ARCH)

ball.o: ball.cpp ball.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c ball.cpp -Wall $(LEVEL) $(ARCH)

player.o: player.cpp  player.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c player.cpp -Wall $(LEVEL) $(ARCH)

sound.o: sound.cpp sound.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c sound.cpp -Wall $(LEVEL) $(ARCH)

color.o: color.cpp color.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c color.cpp -Wall $(LEVEL) $(ARCH)

wav.o: wav.cpp wav.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c wav.cpp -Wall $(LEVEL) $(ARCH)

scene.o: scene.cpp scene.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene.cpp -Wall $(LEVEL) $(ARCH)

info.o: info.cpp info.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c info.cpp -Wall $(LEVEL) $(ARCH)

vector3d.o: vector3d.cpp vector3d.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c vector3d.cpp -Wall $(LEVEL) $(ARCH)

jpeg.o: jpeg.cpp jpeg.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c jpeg.cpp -Wall $(LEVEL) $(ARCH)

tga.o: tga.cpp tga.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c tga.cpp -Wall $(LEVEL) $(ARCH)

ogg.o: ogg.cpp ogg.h 
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c ogg.cpp -Wall $(LEVEL) $(ARCH)

keyhelper.o: keyhelper.cpp keyhelper.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c keyhelper.cpp -Wall $(LEVEL) $(ARCH)

camera.o: camera.cpp camera.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c camera.cpp -Wall $(LEVEL) $(ARCH)

gamedata.o: gamedata.cpp gamedata.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c gamedata.cpp -Wall $(LEVEL) $(ARCH)

lights.o: lights.cpp lights.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c lights.cpp -Wall $(LEVEL) $(ARCH)

rigidbody.o: rigidbody.cpp rigidbody.h
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c rigidbody.cpp -Wall $(LEVEL) $(ARCH)

main.o: common.h main.cpp $(FILES)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c main.cpp -Wall $(LEVEL) $(ARCH)
all:
	make	
test:
	make clean
	make
	./$(TARGET)
clean:
	rm -rf *.o *~ $(TARGET)
