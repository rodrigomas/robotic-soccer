#ifndef COMMON_H
#define COMMON_H

#include <math.h>
#include <iostream>
#include <GL/gl.h>

#include "defs.h"
#include "color.h"
#include "translator.h"

#define PI M_PI

#define SPECIAL_KEY_CLEAR 0xefffffff
#define SPECIAL_KEY_SET   0x10000000

#define JOY_CLEAR 0xfeffffff
#define JOY_SET   0x01000000

#define JOY_PLUS_CLEAR 0xffefffff
#define JOY_PLUS_SET   0x00100000

#define LG_JPEG_READ_ERROR 10
#define LG_JPEG_WRITE_ERROR 11

#define LG_SC1_BT1 30
#define LG_SC1_BT2 31
#define LG_SC1_BT3 32
#define LG_SC1_BT4 33
#define LG_SC1_BT5 34

#define LG_SC2_BT1 35
#define LG_SC3_BT1 36
#define LG_SC4_BT1 37
#define LG_SC6_BT1 38

#define LG_SC5_PAUSE_TXT 39
#define LG_SC5_END_TXT 40

#define LG_LUA_ERROR_FCN 60
#define LG_LUA_SCRIPT_ERROR 61

#define LG_OPENAL_ERROR 70
#define LG_OPENAL_FILE_ERROR 71
#define LG_OPENAL_OGG_ERROR 72

#define LG_TGA_ERROR 20
#define LG_MEMORY_ERROR 21
#define LG_VERTEX_SHADER_ERROR 22
#define LG_FRAG_SHADER_ERROR 23
#define LG_SHADER_ERROR 24
#define LG_SHADER_OPEN_ERROR 25
#define LG_SHADER_SUPPORT_ERROR 26


#define LG_XML_ERROR 90
#define LG_XML_ROOT_ERROR 91
#define LG_XML_CONF_ERROR 92
#define LG_XML_VER_ERROR 93

#define LG_ABOUT_GAME 1
#define LG_EXIT_GAME 2

#define THANKS "\nObrigado por jogar!\n\nAutor: Rodrigo Marques Almeida da Silva\n\n"

extern bool useMaterial;
extern bool useFog;
extern bool useLight;
extern bool useShader;
extern bool use_tga;
extern bool useBodyView;
extern soccer::CTranslator Language;

void Terminate(void);

void ChangeScene( int ID, void* info = NULL );

#endif
