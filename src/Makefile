#ss -lalut -lXi -lX11 -lz -ldl
INCPATH = -I/usr/X11R6/include/ -I/usr/include -I/usr/include/libxml2 -Isrc -I/usr/local/include -Iinclude
LIBS = -llua5.1 -lglut -lGLU -lGL -lGLEW -lm -ljpeg -lxml2 -lstdc++ -lpthread -lopenal -logg -lvorbis -lvorbisfile
LIBPATH = -L/usr/lib -L/usr/X11R6/lib/ -L/usr/local/lib -Llibs/linux
CC = g++
CXXFLAGS = -g -Wall
TARGET = ../output/linux/soccer
WSOUND = -DUSE_SOUND
WMUSIC = -DUSE_MUSIC
LEVEL = -O3 -msse -msse2 -msse3 -mmmx -pipe
#-m3dnow -ffast-math
ARCH = 	-march=pentium4 
#-mfpmath=sse
FILES = defs.o vector3d.o camera.o tga.o wav.o ogg.o keyhelper.o lights.o color.o rigidbody.o info.o scene.o jpeg.o gamedata.o sound.o libsoccer.o button.o fade.o player.o physics.o scene_01.o scene_02.o scene_03.o scene_04.o scene_05.o scene_06.o sky.o placar.o soccerfield.o flagman.o ball.o rules.o

$(TARGET): main.o
	$(CC) $(INCPATH) $(LIBPATH) $(LIBS) $(CXXFLAGS) -o $(TARGET) main.o $(FILES) $(ARCH) $(LEVEL)

physics.o: physics.cpp physics.h
	$(CC) $(WSOUND) $(WMUSIC) -c physics.cpp -Wall $(LEVEL) $(ARCH)

defs.o: defs.cpp defs.h
	$(CC) $(WSOUND) $(WMUSIC) -c defs.cpp -Wall $(LEVEL) $(ARCH)

libsoccer.o: libsoccer.cpp libsoccer.h
	$(CC) $(WSOUND) $(WMUSIC) -c libsoccer.cpp -Wall $(LEVEL) $(ARCH)

fade.o: fade.cpp fade.h
	$(CC) $(WSOUND) $(WMUSIC) -c fade.cpp -Wall $(LEVEL) $(ARCH)

scene_01.o: scene_01.cpp scene_01.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_01.cpp -Wall $(LEVEL) $(ARCH)

scene_02.o: scene_02.cpp scene_02.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_02.cpp -Wall $(LEVEL) $(ARCH)

scene_03.o: scene_03.cpp scene_03.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_03.cpp -Wall $(LEVEL) $(ARCH)

scene_04.o: scene_04.cpp scene_04.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_04.cpp -Wall $(LEVEL) $(ARCH)

scene_05.o: scene_05.cpp scene_05.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_05.cpp -Wall $(LEVEL) $(ARCH)

scene_06.o: scene_06.cpp scene_06.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene_06.cpp -Wall $(LEVEL) $(ARCH)

button.o: button.cpp button.h
	$(CC) $(WSOUND) $(WMUSIC) -c button.cpp -Wall $(LEVEL) $(ARCH)

rules.o: rules.cpp rules.h
	$(CC) $(WSOUND) $(WMUSIC) -c rules.cpp -Wall $(LEVEL) $(ARCH)

sky.o: sky.cpp sky.h
	$(CC) $(WSOUND) $(WMUSIC) -c sky.cpp -Wall $(LEVEL) $(ARCH)

placar.o: placar.cpp placar.h
	$(CC) $(WSOUND) $(WMUSIC) -c placar.cpp -Wall $(LEVEL) $(ARCH)

soccerfield.o: soccerfield.cpp soccerfield.h
	$(CC) $(WSOUND) $(WMUSIC) -c soccerfield.cpp -Wall $(LEVEL) $(ARCH)

flagman.o: flagman.cpp flagman.h
	$(CC) $(WSOUND) $(WMUSIC) -c flagman.cpp -Wall $(LEVEL) $(ARCH)

ball.o: ball.cpp ball.h
	$(CC) $(WSOUND) $(WMUSIC) -c ball.cpp -Wall $(LEVEL) $(ARCH)

player.o: player.cpp  player.h
	$(CC) $(WSOUND) $(WMUSIC) -c player.cpp -Wall $(LEVEL) $(ARCH)

sound.o: sound.cpp sound.h
	$(CC) $(WSOUND) $(WMUSIC) -c sound.cpp -Wall $(LEVEL) $(ARCH)

color.o: color.cpp color.h
	$(CC) $(WSOUND) $(WMUSIC) -c color.cpp -Wall $(LEVEL) $(ARCH)

wav.o: wav.cpp wav.h
	$(CC) $(WSOUND) $(WMUSIC) -c wav.cpp -Wall $(LEVEL) $(ARCH)

scene.o: scene.cpp scene.h
	$(CC) $(WSOUND) $(WMUSIC) -c scene.cpp -Wall $(LEVEL) $(ARCH)

info.o: info.cpp info.h
	$(CC) $(WSOUND) $(WMUSIC) -c info.cpp -Wall $(LEVEL) $(ARCH)

vector3d.o: vector3d.cpp vector3d.h
	$(CC) $(WSOUND) $(WMUSIC) -c vector3d.cpp -Wall $(LEVEL) $(ARCH)

jpeg.o: jpeg.cpp jpeg.h
	$(CC) $(WSOUND) $(WMUSIC) -c jpeg.cpp -Wall $(LEVEL) $(ARCH)

tga.o: tga.cpp tga.h
	$(CC) $(WSOUND) $(WMUSIC) -c tga.cpp -Wall $(LEVEL) $(ARCH)

ogg.o: ogg.cpp ogg.h 
	$(CC) $(WSOUND) $(WMUSIC) -c ogg.cpp -Wall $(LEVEL) $(ARCH)

keyhelper.o: keyhelper.cpp keyhelper.h
	$(CC) $(WSOUND) $(WMUSIC) -c keyhelper.cpp -Wall $(LEVEL) $(ARCH)

camera.o: camera.cpp camera.h
	$(CC) $(WSOUND) $(WMUSIC) -c camera.cpp -Wall $(LEVEL) $(ARCH)

gamedata.o: gamedata.cpp gamedata.h
	$(CC) $(WSOUND) $(WMUSIC) -c gamedata.cpp -Wall $(LEVEL) $(ARCH)

lights.o: lights.cpp lights.h
	$(CC) $(WSOUND) $(WMUSIC) -c lights.cpp -Wall $(LEVEL) $(ARCH)

rigidbody.o: rigidbody.cpp rigidbody.h
	$(CC) $(WSOUND) $(WMUSIC) -c rigidbody.cpp -Wall $(LEVEL) $(ARCH)

main.o: common.h main.cpp $(FILES)
	$(CC) $(WSOUND) $(WMUSIC) -c main.cpp -Wall $(LEVEL) $(ARCH)
all:
	make	
test:
	make clean
	make
	./$(TARGET)
clean:
	rm -rf *.o *~ $(TARGET)
