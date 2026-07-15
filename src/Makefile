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
BUILD_DIR = ../build/$(shell uname -s | tr A-Z a-z)
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = $(BUILD_DIR)/tests
LUA ?= luajit
XMLLINT ?= xmllint

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

OBJECTS = $(addprefix $(OBJ_DIR)/, defs.o vector3d.o camera.o tga.o wav.o ogg.o keyhelper.o lights.o color.o rigidbody.o info.o scene.o jpeg.o gamedata.o sound.o libsoccer.o button.o fade.o player.o physics.o scene_01.o scene_02.o scene_03.o scene_04.o scene_05.o scene_06.o sky.o placar.o soccerfield.o flagman.o ball.o rules.o fixed_timestep.o lua_strategy_profile.o match_heatmap.o match_metrics.o match_telemetry.o tactical_advisor.o)

$(TARGET): $(OBJ_DIR)/main.o
	mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(ARCH) $(LEVEL) -o $(TARGET) $(OBJ_DIR)/main.o $(OBJECTS) $(LIBPATH) $(LDFLAGS) $(LIBS) $(LDLIBS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

$(OBJ_DIR)/physics.o: physics.cpp physics.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c physics.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/defs.o: defs.cpp defs.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c defs.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/libsoccer.o: libsoccer.cpp libsoccer.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c libsoccer.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/fade.o: fade.cpp fade.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c fade.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_01.o: scene_01.cpp scene_01.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_01.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_02.o: scene_02.cpp scene_02.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_02.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_03.o: scene_03.cpp scene_03.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_03.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_04.o: scene_04.cpp scene_04.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_04.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_05.o: scene_05.cpp scene_05.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_05.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene_06.o: scene_06.cpp scene_06.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene_06.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/button.o: button.cpp button.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c button.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/rules.o: rules.cpp rules.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c rules.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/fixed_timestep.o: engine/core/fixed_timestep.cpp engine/core/fixed_timestep.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c engine/core/fixed_timestep.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/lua_strategy_profile.o: ai/lua_strategy_profile.cpp ai/lua_strategy_profile.h analytics/tactical_advisor.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c ai/lua_strategy_profile.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/match_heatmap.o: analytics/match_heatmap.cpp analytics/match_heatmap.h vector3d.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c analytics/match_heatmap.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/match_metrics.o: analytics/match_metrics.cpp analytics/match_metrics.h vector3d.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c analytics/match_metrics.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/match_telemetry.o: analytics/match_telemetry.cpp analytics/match_telemetry.h analytics/match_heatmap.h analytics/match_metrics.h ball.h player.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c analytics/match_telemetry.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/tactical_advisor.o: analytics/tactical_advisor.cpp analytics/tactical_advisor.h ball.h player.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c analytics/tactical_advisor.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/sky.o: sky.cpp sky.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c sky.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/placar.o: placar.cpp placar.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c placar.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/soccerfield.o: soccerfield.cpp soccerfield.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c soccerfield.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/flagman.o: flagman.cpp flagman.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c flagman.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/ball.o: ball.cpp ball.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c ball.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/player.o: player.cpp  player.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c player.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/sound.o: sound.cpp sound.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c sound.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/color.o: color.cpp color.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c color.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/wav.o: wav.cpp wav.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c wav.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/scene.o: scene.cpp scene.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c scene.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/info.o: info.cpp info.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c info.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/vector3d.o: vector3d.cpp vector3d.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c vector3d.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/jpeg.o: jpeg.cpp jpeg.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c jpeg.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/tga.o: tga.cpp tga.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c tga.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/ogg.o: ogg.cpp ogg.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c ogg.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/keyhelper.o: keyhelper.cpp keyhelper.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c keyhelper.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/camera.o: camera.cpp camera.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c camera.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/gamedata.o: gamedata.cpp gamedata.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c gamedata.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/lights.o: lights.cpp lights.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c lights.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/rigidbody.o: rigidbody.cpp rigidbody.h | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c rigidbody.cpp -o $@ -Wall $(LEVEL) $(ARCH)

$(OBJ_DIR)/main.o: common.h main.cpp $(OBJECTS) | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) $(WSOUND) $(WMUSIC) -c main.cpp -o $@ -Wall $(LEVEL) $(ARCH)

.PHONY: all test smoke core-tests clean

all: $(TARGET)

$(TEST_DIR)/fixed_timestep_test: engine/core/fixed_timestep_test.cpp engine/core/fixed_timestep.cpp engine/core/fixed_timestep.h | $(TEST_DIR)
	$(CXX) $(CPPFLAGS) $(INCPATH) -o $@ engine/core/fixed_timestep_test.cpp engine/core/fixed_timestep.cpp -Wall $(LEVEL) $(ARCH)

core-tests: $(TEST_DIR)/fixed_timestep_test
	$(TEST_DIR)/fixed_timestep_test

smoke: core-tests
	cd ../config && $(XMLLINT) --noout config.xml data/*.xml
	cd ../config && $(LUA) -e 'dofile("scripts/botafogo.lua"); dofile("scripts/flamengo.lua")'

test:
	make clean
	make
	cd ../config && ../output/$(shell uname -s | tr A-Z a-z)/soccer
clean:
	rm -rf *.o *~ $(BUILD_DIR) $(TARGET)
