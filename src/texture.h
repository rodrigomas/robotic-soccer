#ifndef TEX_H
#define TEX_H

// Define a estrutura de uma imagem
typedef struct
{
	char nome[50];			// nome do arquivo carregado
	int ncomp;			// nmero de componentes na textura (1-intensidade, 3-RGB, 4-RGBA)
	int dimx;			// largura 
	int dimy;			// altura
	unsigned int texid;		// identificao da textura em OpenGL
	unsigned char *data;		// apontador para a imagem em si
} TEX;

#endif
