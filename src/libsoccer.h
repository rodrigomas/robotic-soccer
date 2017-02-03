#ifndef LIBSOCCER_H
#define LIBSOCCER_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <string>
#include <jpeglib.h>
#include <math.h>

/*#ifndef DEBUG
#define DEBUG
#endif*/

//#include "common.h"
#include "defs.h"
#include "tga.h"
#include "jpeg.h"
#include "texture.h"

using std::vector;

namespace soccer {

	//unsigned int listID;
	//unsigned int listID;

	unsigned int getlistID(void);
	
	// Define um material
	typedef struct {
		char nome[20];	// Identifica�o do material
		GLfloat ka[4];	// Ambiente
		GLfloat kd[4];	// Difuso
		GLfloat ks[4];	// Especular
		GLfloat ke[4];	// Emiss�
		GLfloat spec;	// Fator de especularidade
	} MAT;
	
	// Procura um material pelo nome na lista e devolve
	// o �dice onde est� ou -1 se n� achar
	int _procuraMaterial(char *nome);
	
	void print_upper(char *value);
	
	void print_upper(std::string &value);
	
	// Procura um material pelo nome e devolve um
	// apontador para ele ou NULL caso n� ache
	MAT *ProcuraMaterial(char *nome);
	
	#ifndef __FREEGLUT_EXT_H__
	// Funcao para desenhar um texto na tela com fonte bitmap
	void glutBitmapString(void *fonte,char *texto);
	#endif
	
	void puts2DStroke( const char *string, void *font = NULL );
	
	void puts2DBitmap(const char *string, void *font = NULL);
	
	void resetMaterial(void);
	
	void glutCircleL( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start = 0, GLdouble end = 2 * M_PI, 
		GLdouble disc = 10.0 );
	
	void glutCircleF( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start = 0, GLdouble end = 2 * M_PI, 
		GLdouble disc = 10.0 );
	
	void glutCircleS( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start = 0, GLdouble end = 2 * M_PI, 
		GLdouble disc = 10.0 );
	
	// Escreve uma string na tela, usando uma proje�o ortogr�ica
	// normalizada (0..1, 0..1)
	void puts2D(float x, float y, char *str);
	
	// Procura uma textura pelo nome na lista e devolve
	// o �dice onde est� ou -1 se n� achar
	int __textureSearch(char *nome);
	
	TGAImg* LoadTGA(char *filename);
	
	void RasterTGA(TGAImg *Img);
	
	
	void screenCapture( void );
	
	// Fun�o para ler um arquivo JPEG e criar uma
	// textura OpenGL
	// mipmap = true se deseja-se utilizar mipmaps
	TEX *LoadTexture(char *arquivo, bool mipmap);
	
	TEX* LoadTGATexture(char *TexName);
	
	void freeTextures(void);
	
	void glutRect(void);
	
	void SphereList( double Radius );
	
	void SphereInList( double Radius );
	
	void CubeList( double Size );
	
	void CubeDivList( double Size );
	
	void CreateDisplay(void); 
	
	// Libera mem�ia ocupada pela lista de materiais e texturas
	void freeMaterials();
	
	void freeSCCList(void);
	
	void startSCCLists(void);
	
	void deleteTGA( TGAImg *p );
	
	// L�um arquivo que define materiais para um objeto 3D no
	// formato .OBJ
	
	void _leMateriais(char *nomeArquivo);
	
	void LoadMaterialsFile(char *nomeArquivo);
	
	void applyMaterial( MAT *m, bool texture );
	
	void applyMaterial( char*name, bool texture );
	
	// Fun�o para ler 6 arquivos JPEG e criar
	// texturas para cube mapping
	// mipmap = true se for utilizar mipmaps
	TEX *LoadCubeTextures(char *nomebase, bool mipmap);
	
	// Fun�o que recebe o nome de um arquivo-texto, carrega
	// e retorna um apontador para o contedo deste
	GLchar *CarregaArquivo(char *nome);
	
	// Fun�o que recebe o handle de um shader/programa e retorna
	// o infolog da ltima compila�o ou liga�o
	void MostraInfoLog(GLuint shader);
	
	// Fun�o que recebe os nomes de dois arquivos-texto, contendo
	// os c�igos-fonte do vertex shader e do fragment shader
	GLuint CompilaShaders(char *vertsrc, char *fragsrc);
	
	void initShader( const char *v, const char *f);
	
	void renderShader(void);
	
	void resetShader(void);

};
#endif
