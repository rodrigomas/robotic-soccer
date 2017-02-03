#ifndef JPEG_H
#define JPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <jpeglib.h>

#include "texture.h"

// Decodifica uma imagem JPG e armazena-a em uma estrutura TEX.
void JPGDecode(jpeg_decompress_struct* cinfo, TEX *pImageData, bool invert);

// Carrega o arquivo JPG e retorna seus dados em uma estrutura tImageJPG.
TEX *LoadJPG(const char *filename, bool invert = true);

void saveJPG( char *arquivo, GLubyte *image_buffer, int w, int h, int quality = 80, bool invert = true );

void deleteJPG( TEX *p );

#endif
