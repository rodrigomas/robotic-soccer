#include "libsoccer.h"
#include "common.h"

// Constantes utilizadas caso n� existam em GL/gl.h
#ifndef GL_ARB_texture_cube_map
# define GL_NORMAL_MAP					0x8511
# define GL_REFLECTION_MAP				0x8512
# define GL_TEXTURE_CUBE_MAP			0x8513
# define GL_TEXTURE_BINDING_CUBE_MAP	0x8514
# define GL_TEXTURE_CUBE_MAP_POSITIVE_X	0x8515
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_X	0x8516
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Y	0x8517
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	0x8518
# define GL_TEXTURE_CUBE_MAP_POSITIVE_Z	0x8519
# define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	0x851A
# define GL_PROXY_TEXTURE_CUBE_MAP		0x851B
# define GL_MAX_CUBE_MAP_TEXTURE_SIZE	0x851C
#endif

using std::vector;

namespace soccer {

	unsigned int listID;

	unsigned int getlistID(void)
	{
		return listID;
	}

	GLUquadric *sphere;

	// Handle para programa GLSL
	GLuint prog;

	// Handle para vari�el uniform "interna"
	GLint uinterna;

	// Lista de materiais
	vector<MAT*> _materiais(0);

	// Lista de texturas
	vector<TEX*> __textures(0);

	GLubyte *imagem;
	int image_cnt = 0;

	// Procura um material pelo nome na lista e devolve
	// o �dice onde est� ou -1 se n� achar
	int _procuraMaterial(char *nome)
	{
		unsigned int i;
		for(i=0;i<_materiais.size();++i)
			if(!strcmp(nome,_materiais[i]->nome))
				return i;
		return -1;
	}

	void print_upper(char *value)
	{
		register int t;

		for(t=0; value[t]; ++t)  {
			value[t] = toupper(value[t]);
		}
	}

	void print_upper(std::string &value)
	{
		register int t;

		for(t=0; t < (int)value.size(); ++t)  {
			value[t] = toupper(value[t]);
		}
	}

	// Procura um material pelo nome e devolve um
	// apontador para ele ou NULL caso n� ache
	MAT *ProcuraMaterial(char *nome)
	{
		int pos = _procuraMaterial(nome);
		if(pos == -1) return NULL;
		else return _materiais[pos];
	}

	#ifndef __FREEGLUT_EXT_H__
	// Funcao para desenhar um texto na tela com fonte bitmap
	void glutBitmapString(void *fonte,char *texto)
	{
		// Percorre todos os caracteres
		while (*texto)
		glutBitmapCharacter(fonte, *texto++);
	}
	#endif

	void puts2DStroke( const char *string, void *font )
	{
		// Exibe caractere a caractere
		if (!font)
			font = GLUT_STROKE_ROMAN;

		while(*string)
			glutStrokeCharacter(font,*string++);
	}

	void puts2DBitmap(const char *string, void *font)
	{
		// Exibe caractere a caractere
		if (!font)
			font = GLUT_BITMAP_TIMES_ROMAN_24;

		while(*string)
			glutBitmapCharacter(font,*string++);
	}

	GLfloat __cor[4] = {1.0f,1.0f,1.0f, 1.0f};
	GLfloat __core[4] = {1.0f,1.0f,1.0f, 1.0f};
	GLfloat __espec[4] = {1.0f,1.0f,1.0f, 1.0f};

	void resetMaterial(void)
	{
		glColor3f(1.0,1.0,1.0);
		glMaterialfv(GL_FRONT,GL_DIFFUSE, __cor);
		glMaterialfv(GL_FRONT,GL_SPECULAR, __espec);
		glMateriali(GL_FRONT,GL_SHININESS, 60);
		glMaterialfv(GL_FRONT,GL_EMISSION, __core);
	}

	void glutCircleL( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start, GLdouble end,
		GLdouble disc)
	{
		register double i = start;
		glPushMatrix();
		glTranslatef(x,y,z);
		glBegin(GL_LINE_LOOP);
			for( ; i < end; i +=  M_PI / disc )
				glVertex3f( R * sin(i), 0.0, R * cos(i));
		glEnd();
		glPopMatrix();
	}

	void glutCircleF( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start, GLdouble end,
		GLdouble disc )
	{
		register double i = start;
		glPushMatrix();
		glTranslatef(x,y,z);
		glBegin(GL_TRIANGLE_FAN);
			for( ; i < end; i +=  M_PI / disc )
				glVertex3f( R * sin(i), 0.0, R * cos(i));
		glEnd();
		glPopMatrix();
	}

	void glutCircleS( GLdouble R, GLdouble x, GLdouble y, GLdouble z, GLdouble start, GLdouble end,
		GLdouble disc )
	{
		register double i = start;
		glPushMatrix();
		glTranslatef(x,y,z);
		glBegin(GL_LINE_STRIP);
			for( ; i < end; i +=  M_PI / disc )
				glVertex3f( R * sin(i), 0.0, R * cos(i));
		glEnd();
		glPopMatrix();
	}

	// Escreve uma string na tela, usando uma proje�o ortogr�ica
	// normalizada (0..1, 0..1)
	void puts2D(float x, float y, char *str)
	{
		glMatrixMode(GL_PROJECTION);
		// Salva proje�o perspectiva corrente
		glPushMatrix();
		glLoadIdentity();
		// E cria uma proje�o ortogr�ica normalizada (0..1,0..1)
		gluOrtho2D(0,1,0,1);

		glMatrixMode(GL_MODELVIEW);
		// Salva modelview corrente
		glPushMatrix();
		glLoadIdentity();

		// Posiciona no canto inferior esquerdo da janela
		glRasterPos2f(x,y);
		glColor3f(0,0,0);
		// "Escreve" a mensagem
		//glutBitmapString(GLUT_BITMAP_9_BY_15,(const unsigned char*)str);
		soccer::glutBitmapString(GLUT_BITMAP_9_BY_15,str);

		glMatrixMode(GL_PROJECTION);
		// Restaura a matriz de proje�o anterior
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		// ... e a modelview anterior
		glPopMatrix();
	}
	// Procura uma textura pelo nome na lista e devolve
	// o �dice onde est� ou -1 se n� achar
	int __textureSearch(char *nome)
	{
		unsigned int i;
		for(i=0;i<__textures.size();++i)
			if(!strcmp(nome,__textures[i]->nome))
				return i;
		return -1;
	}

	TGAImg* LoadTGA(char *filename){
		TGAImg* Img = NULL;        // Image loader

		// Load our Texture
		if(Img->Load(filename) != IMG_OK ) {

			fprintf(stderr,"[ERRO]: %s: %s\n",Language[LG_TGA_ERROR], filename);
			return NULL;
		}


		// Specify filtering and edge actions
		return Img;
	}

	void RasterTGA(TGAImg *Img){
		// Create the texture
		if(Img->GetBPP()==24)
			glDrawPixels(Img->GetWidth(), Img->GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, Img->GetImg());
		else if(Img->GetBPP()==32)
			glDrawPixels(Img->GetWidth(), Img->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, Img->GetImg());
	}


	void screenCapture( void ) {

		int W = SCR_WIDTH;
		int H = SCR_HEIGHT;

		imagem = (GLubyte *) malloc( W * H * 3 * sizeof(GLubyte));

		glReadPixels(0,0,SCR_WIDTH,SCR_HEIGHT,GL_RGB,GL_UNSIGNED_BYTE,imagem);
		// GL_COLOR_INDEX, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB,
		// GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA.

		char filename[50];

		sprintf(filename, "Screen%.2d.jpg", image_cnt);

		saveJPG(filename, imagem, W, H);

		image_cnt++;
		free(imagem);

	}

	// Fun�o para ler um arquivo JPEG e criar uma
	// textura OpenGL
	// mipmap = true se deseja-se utilizar mipmaps
	TEX *LoadTexture(char *arquivo, bool mipmap)
	{
		GLenum formato;

		if(!arquivo)		// retornamos NULL caso nenhum nome de arquivo seja informado
			return NULL;

		int indice = __textureSearch(arquivo);
		// Se textura j�foi carregada, retorna
		// apontador para ela
		if(indice!=-1)
			return __textures[indice];

		TEX *pImage = LoadJPG(arquivo);	// carrega o arquivo JPEG

		if(pImage == NULL)	// se n� foi poss�el carregar o arquivo, finaliza o programa
			exit(0);

		strcpy(pImage->nome,arquivo);
		// Gera uma identifica�o para a nova textura
		glGenTextures(1, &pImage->texid);

		// Informa o alinhamento da textura na mem�ia
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// Informa que a textura �a corrente
		glBindTexture(GL_TEXTURE_2D, pImage->texid);

		if(pImage->ncomp==1) formato = GL_LUMINANCE;
		else formato = GL_RGB;

		if(mipmap)
		{
			// Cria mipmaps para obter maior qualidade
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, pImage->dimx, pImage->dimy,
				formato, GL_UNSIGNED_BYTE, pImage->data);
			// Ajusta os filtros iniciais para a textura
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			// Envia a textura para OpenGL, usando o formato RGB
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, pImage->dimx, pImage->dimx,
				0, formato, GL_UNSIGNED_BYTE, pImage->data);
			// Ajusta os filtros iniciais para a textura
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// Finalmente, libera a mem�ia ocupada pela imagem (j�que a textura j�foi enviada para OpenGL)

		free(pImage->data); 	// libera a mem�ia ocupada pela imagem
	#ifdef DEBUG
		//fprintf(stderr,"Textura: %s - %d\n",arquivo,pImage->texid);
	#endif
		// Inclui textura na lista
		__textures.push_back(pImage);
		// E retorna apontador para a nova textura
		return pImage;
	}

	TEX* LoadTGATexture(char *TexName)
	{
		TGAImg Img;        // Image loader
//		GLuint Texture;

		TEX *pImage;

		// Load our Texture
		if(Img.Load(TexName)!=IMG_OK) {
			//fprintf(stderr,"[ERRO]: Falha ao carregar o arquivo TGA: %s\n",TexName);
			fprintf(stderr,"[ERRO]: %s: %s\n",Language[LG_TGA_ERROR], TexName);
			return NULL;
		}

		pImage = new TEX;

		glGenTextures(1,&pImage->texid);            // Allocate space for texture
		glBindTexture(GL_TEXTURE_2D,pImage->texid); // Set our Tex handle as current

		// Create the texture
		if(Img.GetBPP()==24)
			glTexImage2D(GL_TEXTURE_2D,0,3,Img.GetWidth(),Img.GetHeight(),0,GL_RGB,GL_UNSIGNED_BYTE,Img.GetImg());
		else if(Img.GetBPP()==32)
			glTexImage2D(GL_TEXTURE_2D,0,4,Img.GetWidth(),Img.GetHeight(),0,GL_RGBA,GL_UNSIGNED_BYTE,Img.GetImg());
		else
			return NULL;

		// Specify filtering and edge actions
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		pImage->dimx = Img.GetWidth();

		pImage->dimy = Img.GetHeight();

		pImage->ncomp = 4; // RGBA

		strcpy(pImage->nome,TexName);

		// Inclui textura na lista
		__textures.push_back(pImage);
		// E retorna apontador para a nova textura
		return pImage;
	}

	void freeTextures(void)
	{
		unsigned int i;
	#ifdef DEBUG
		printf("Total de texturas: %d\n",__textures.size());
	#endif
		// Para cada textura
		for(i=0;i<__textures.size();++i)
		{
			// Libera textura - n� �necess�io liberar a imagem, pois esta j�		// foi liberada durante a carga da textura - ver CarregaTextura
	#ifdef DEBUG
			printf("%s: %d x %d (id: %d)\n",__textures[i]->nome,__textures[i]->dimx,
					__textures[i]->dimy,__textures[i]->texid);
	#endif
			free(__textures[i]);
		}
		// Limpa lista
		__textures.clear();
	}

	void glutRect(void)
	{
		glPushMatrix();

	// 		glBegin(GL_TRIANGLE_STRIP);
	// 			glVertex3f(-1.0f, 1.0f ,0);
	// 			glVertex3f(-1.0f,-1.0f ,0);
	// 			glVertex3f( 1.0f, 1.0f ,0);
	// 			glVertex3f( 1.0f,-1.0f ,0);
	// 		glEnd();

			glBegin(GL_QUAD_STRIP);
				glVertex3f(-1.0f, 1.0f ,0);
				glVertex3f(-1.0f,-1.0f ,0);
				glVertex3f( 1.0f, 1.0f ,0);
				glVertex3f( 1.0f,-1.0f ,0);
			glEnd();

		glPopMatrix();
	}

	void SphereList( double Radius )
	{
		glPushMatrix();
		glScalef(Radius,Radius,Radius);
		glCallList(listID);
		glPopMatrix();
	}

	void SphereInList( double Radius )
	{
		glPushMatrix();
		glScalef(Radius,Radius,Radius);
		glCallList(listID+3);
		glPopMatrix();
	}

	void CubeList( double Size )
	{
		glPushMatrix();
		glScalef(Size,Size,Size);
		glCallList(listID+1);
		glPopMatrix();
	}

	void CubeDivList( double Size )
	{
		glPushMatrix();
		glScalef(Size,Size,Size);
		glCallList(listID+2);
		glPopMatrix();
	}

	void CreateDisplay(void) {
		listID = glGenLists(5);
		glNewList(listID,GL_COMPILE);
			//gluSphere(head,0.15f,10,10);
			gluSphere(sphere,1.0f,10,10);
		glEndList();

		glNewList(listID+1,GL_COMPILE);
			glBegin(GL_QUAD_STRIP);
			//Quads 1 2 3 4
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V1
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
			glTexCoord2f(2.0f, 1.0f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6
			glTexCoord2f(2.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V5
			glTexCoord2f(3.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);   //V8
			glTexCoord2f(3.0f, 0.0f); glVertex3f(-0.5f,-0.5f, 0.5f);   //V7
			glTexCoord2f(4.0f, 1.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
			glTexCoord2f(4.0f, 0.0f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V1
			glEnd();
			glBegin(GL_QUADS);
			//Quad 5
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);   //V8
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4
			//Quad 6
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,-0.5f, 0.5f);   //V7
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V5
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V1
			glEnd();
		glEndList();

		glNewList(listID+2,GL_COMPILE);

			glBegin(GL_QUADS);

			//Quads 1 // Lateral 1
			glNormal3d( 1.0, 0.0, 0.0);
			glTexCoord2f(0.5f, 1.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
			glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V1
			glTexCoord2f(1.0f, 0.5f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4

			//Quads 3 // Lateral 2
			glNormal3d( -1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.5f); glVertex3f( -0.5f, -0.5f, -0.5f);   //V3
			glTexCoord2f(0.5f, 0.5f); glVertex3f( -0.5f,-0.5f, 0.5f);   //V1
			glTexCoord2f(0.5f, 1.0f); glVertex3f( -0.5f,0.5f,0.5f);   //V2
			glTexCoord2f(1.0f, 1.0f); glVertex3f( -0.5f, 0.5f,-0.5f);   //V4

			//Quad 2 // Frontal
			glNormal3d( 0.0, 0.0,1.0);
			glTexCoord2f(0.5f, 0.5f); glVertex3f( -0.5f, -0.5f,0.5f);   //V3
			glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,-0.5f,0.5f);   //V1
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f,0.5f,0.5f);   //V2
			glTexCoord2f(0.5f, 1.0f); glVertex3f(-0.5f, 0.5f,0.5f);   //V4

			//Quad 2 // Back
			glNormal3d( 0.0, 0.0,-1.0);
			glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
			glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V5
			glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6

			//Quad 5 // Topo
			glNormal3d( 0.0, 1.0, 0.0);
			glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6
			glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);   //V8
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
			glTexCoord2f(1.0f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4

			//Quad 6 // Fundo
			glNormal3d( 0.0, -1.0, 0.0);
			glTexCoord2f(1.0f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);   //V4
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V3
			glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V1
			glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V2
			glEnd();
		glEndList();

		gluQuadricDrawStyle(sphere,GLU_FILL);
		gluQuadricNormals(sphere,GLU_SMOOTH);
		gluQuadricOrientation(sphere,GLU_INSIDE);
		gluQuadricTexture(sphere,GL_TRUE);

		glNewList(listID+3,GL_COMPILE);
			gluSphere(sphere,1.0f,10,10);
		glEndList();

		gluQuadricDrawStyle(sphere,GLU_FILL);
		gluQuadricNormals(sphere,GLU_SMOOTH);
		gluQuadricOrientation(sphere,GLU_OUTSIDE);
		gluQuadricTexture(sphere,GL_TRUE);

		glNewList(listID+4,GL_COMPILE);

	#   define V(a,b,c) glVertex3d( a 0.5, b 0.5, c 0.5 );
	#   define N(a,b,c) glNormal3d( a, b, c );
			glBegin( GL_QUADS );
				N( -1.0, 0.0, 0.0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+);
				N( 0.0, -1.0, 0.0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+);
				N( 0.0, 0.0, -1.0); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+);
				N(1.0, 0.0, 0.0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-);
				N( 0.0,1.0, 0.0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+);
				N( 0.0, 0.0,1.0); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-);
			glEnd();
	#   undef V
	#   undef N
		glEndList();
	}

	// Libera mem�ia ocupada pela lista de materiais e texturas
	void freeMaterials()
	{
		unsigned int i;
	#ifdef DEBUG
		printf("Total de materiais: %d\n",_materiais.size());
	#endif
		// Para cada material
		for(i=0;i<_materiais.size();++i)
		{
	#ifdef DEBUG
			printf("%s a: (%f,%f,%f,%f) - d: (%f,%f,%f,%f) - e: (%f,%f,%f,%f - %f)\n",_materiais[i]->nome,
					_materiais[i]->ka[0],_materiais[i]->ka[1],_materiais[i]->ka[2], _materiais[i]->ka[3],
					_materiais[i]->kd[0],_materiais[i]->kd[1],_materiais[i]->kd[2], _materiais[i]->kd[3],
					_materiais[i]->ks[0],_materiais[i]->ks[1],_materiais[i]->ks[2], _materiais[i]->ks[3],
					_materiais[i]->spec);
	#endif
			// Libera material
			free(_materiais[i]);
		}
		// Limpa lista
		_materiais.clear();
	}

	void freeSCCList(void)
	{
		gluDeleteQuadric(sphere);
		glDeleteLists(listID,5);
		freeMaterials();
		freeTextures();
	}

	void startSCCLists(void)
	{
		sphere = gluNewQuadric();

		gluQuadricDrawStyle(sphere,GLU_FILL);
		gluQuadricNormals(sphere,GLU_SMOOTH);
		gluQuadricOrientation(sphere,GLU_OUTSIDE);
		gluQuadricTexture(sphere,GL_TRUE);

		CreateDisplay();
	}

	void deleteTGA( TGAImg *p )
	{
		delete p;
	}

	// L�um arquivo que define materiais para um objeto 3D no
	// formato .OBJ


	void _leMateriais(char *nomeArquivo)
	{
		char aux[256];
		FILE *fp;
		MAT *ptr;
		fp = fopen(nomeArquivo,"r");

		/* Especifica�o do arquivo de materiais (.mtl):
		*
		* #,!,$  - coment�io
		* newmtl - nome do novo material(sem espa�s)
		* Ka	  - Cor ambiente, especificada como 3 floats
		* Kd	  - Cor difusa, idem
		* Ks	  - Cor especular, idem
		* Ns	  - Coeficiente de especularidade, entre 0 and 1000
		*          (convertido para a faixa v�ida em OpenGL (0..128) durante a leitura)
		* d	  - Transpar�cia - mesmo significado que o alpha em OpenGL: 0.0 a 1.0 (0.0 = transparente)
		*
		* Os demais campos s� ignorados por este leitor:
		* Tr	  - Transpar�cia (0 = opaco, 1 = transparente)
		* map_Kd - Especifica arquivo com a textura a ser mapeada como material difuso
		* refl	  - Especifica arquivo com um mapa de reflex�
		* Ni	  - �dice de refra�o, 1.0 ou maior, onde 1.0 representa sem refra�o
		* bump	  - Especifica arquivo para "bump mapping"
		* illum  - Modelo de ilumina�o (1 = apenas cores, 2 = textura, 3 = reflex�s e raytracing)
		* map_Ka - Especifica arquivo com a textura a ser mapeada como cor ambiente
		* map_Ks - Especifica arquivo com a textura a ser mapeada como cor especular
		* map_Ns - Especifica arquivo com a textura a ser mapeada como reflex� especular
		* map_d  - Especifica arquivo cmo a textura a ser mapeada como transpar�cia (branco �opaco, preto �transparente)
		*
		*/

		while(!feof(fp))
		{
			fgets(aux,255,fp);
			if (strlen(aux)>1)
				aux[strlen(aux)-1]=0;	// elimina o \n lido do arquivo
			if(aux[0]=='#') continue;
			if(!strncmp(aux,"newmtl",6)) // Novo material ?
			{
				// Se material j�existe na lista, pula para o
				// pr�imo
				if(_procuraMaterial(&aux[7])!=-1)
				{
					ptr = NULL;
					continue;
				}
				if((ptr = (MAT *) malloc(sizeof(MAT)))==NULL)
				{
					//fprintf(stderr,"[ERRO]: Falta de Memoria.\n");
					fprintf(stderr,"[ERRO]: %s \n", Language[LG_MEMORY_ERROR]);
					exit(1);
				}
				// Adiciona �lista
				_materiais.push_back(ptr);
				// Copia nome do material
				strcpy(ptr->nome,&aux[7]);
				// N� existe "emission" na defini�o do material
				// mas o valor pode ser setado mais tarde,
				// via SetaEmissaoMaterial(..)
				ptr->ke[0] = ptr->ke[1] = ptr->ke[2] = 0.0;
			}
			if(!strncmp(aux,"Ka ",3)) // Ambiente
			{
				if(ptr==NULL) continue;
				sscanf(aux,"Ka %f %f %f",&ptr->ka[0],&ptr->ka[1],&ptr->ka[2]);
			}
			if(!strncmp(aux,"Kd ",3)) // Difuso
			{
				if(ptr==NULL) continue;
				sscanf(aux,"Kd %f %f %f",&ptr->kd[0],&ptr->kd[1],&ptr->kd[2]);
			}
			if(!strncmp(aux,"Ke ",3)) // Difuso
			{
				if(ptr==NULL) continue;
				sscanf(aux,"Ke %f %f %f",&ptr->ke[0],&ptr->ke[1],&ptr->ke[2]);
			}
			if(!strncmp(aux,"Ks ",3)) // Especular
			{
				if(ptr==NULL) continue;
				sscanf(aux,"Ks %f %f %f",&ptr->ks[0],&ptr->ks[1],&ptr->ks[2]);
			}
			if(!strncmp(aux,"Ns ",3)) // Fator de especularidade
			{
				if(ptr==NULL) continue;
				sscanf(aux,"Ns %f",&ptr->spec);
				// Converte da faixa lida (0...1000) para o intervalo
				// v�ido em OpenGL (0...128)
				ptr->spec = ptr->spec/1000.0 * 128;
			}
			if(!strncmp(aux,"d ",2)) // Alpha
			{
				if(ptr==NULL) continue;
				// N� existe alpha na defini�o de cada componente
				// mas o valor �lido em separado e vale para todos
				float alpha;
				sscanf(aux,"d %f",&alpha);
				ptr->ka[3] = alpha;
				ptr->kd[3] = alpha;
				ptr->ks[3] = alpha;
			}
		}
		fclose(fp);
	}

	void LoadMaterialsFile(char *nomeArquivo)
	{
		_leMateriais(nomeArquivo);
	}

	void applyMaterial( MAT *m, bool texture )
	{
		GLfloat branco[4] = { 1.0, 1.0, 1.0, 1.0 };

		glMaterialfv(GL_FRONT,GL_AMBIENT,m->ka);
		// Se a face tem textura, ignora a cor difusa do material
		// (caso contr�io, a textura �colorizada em GL_MODULATE)
		if(texture)
			glMaterialfv(GL_FRONT,GL_DIFFUSE,branco);
		else
			glMaterialfv(GL_FRONT,GL_DIFFUSE,m->kd);
		glMaterialfv(GL_FRONT,GL_SPECULAR,m->ks);
		glMaterialfv(GL_FRONT,GL_EMISSION,m->ke);
		glMaterialf(GL_FRONT,GL_SHININESS,m->spec);
	}

	void applyMaterial( char*name, bool texture )
	{
		int i = _procuraMaterial(name);

		if( i != -1 )
			applyMaterial(_materiais[i],texture);
	}

	// Identificadores OpenGL para cada uma das faces
	// do cubemap
	GLenum faces[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	char *nomes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };

	// Fun�o para ler 6 arquivos JPEG e criar
	// texturas para cube mapping
	// mipmap = true se for utilizar mipmaps
	TEX *LoadCubeTextures(char *nomebase, bool mipmap)
	{
		GLenum formato;
		TEX *primeira = NULL;

		if(!nomebase)		// retornamos NULL caso nenhum nome de arquivo seja informado
			return NULL;

		// Informa o alinhamento da textura na mem�ia
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Carrega as texturas do cubemap e envia para OpenGL
		for(int i=0;i<6;++i)
		{
			char arquivo[100];
			sprintf(arquivo,"%s_%s.jpg",nomebase,nomes[i]);

			int indice = __textureSearch(nomebase);
			// Se textura j�foi carregada, retorna
			if(indice!=-1)
				return __textures[indice];

			// Carrega o arquivo JPEG, sem inverter a
			// ordem das linhas (necess�io para a
			// textura n� ficar de cabe� para baixo
			// no cube map)
			TEX *pImage = LoadJPG(arquivo,false);

			if(pImage == NULL)	// se n� foi poss�el carregar o arquivo, finaliza o programa
				exit(0);

			// Se for a primeira textura...
			if(!i)
			{
				// Salva apontador para retornar depois
				primeira = pImage;
				// Gera uma identifica�o para a nova textura
				glGenTextures(1, &pImage->texid);
				glBindTexture(GL_TEXTURE_CUBE_MAP, pImage->texid);
				strcpy(pImage->nome,arquivo);
			}

			if(pImage->ncomp==1) formato = GL_LUMINANCE;
			else formato = GL_RGB;

			if(mipmap)
				// Cria mipmaps para obter maior qualidade
				gluBuild2DMipmaps(faces[i], GL_RGB, pImage->dimx, pImage->dimy,
				formato, GL_UNSIGNED_BYTE, pImage->data);
			else
				// Envia a textura para OpenGL, usando o formato RGB
				glTexImage2D (faces[i], 0, GL_RGB, pImage->dimx, pImage->dimx,
					0, formato, GL_UNSIGNED_BYTE, pImage->data);

			// Finalmente, libera a mem�ia ocupada pela imagem (j�que a textura j�foi enviada para OpenGL)

			free(pImage->data); 	// libera a mem�ia ocupada pela imagem

			// Inclui somente a primeira textura na lista
			if(!i) __textures.push_back(pImage);
			else free(pImage);
		}

		// Ajusta os filtros iniciais para o cube map
		if(mipmap)
		{
			glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		// Retorna apontador para a primeira imagem (que cont� a id)
		return primeira;
	}

	// Fun�o que recebe o nome de um arquivo-texto, carrega
	// e retorna um apontador para o contedo deste
	GLchar *CarregaArquivo(char *nome)
	{
		FILE *arq;
		char *conteudo = NULL;
		char buffer[512];

		register int cont;
		arq = fopen(nome, "rb");
		cont = 0;

		while( !feof(arq) ) {
			//cont += fseek(arq, 1024, SEEK_CUR);
			cont += fread(buffer,sizeof(char),512,arq);
		}

		fclose(arq);

		// Se for poss�el abrir o arquivo...
		if (nome != NULL)
		{
			// Abre como arquivo-texto
			arq = fopen(nome,"rt");

			// Se conseguiu, l�o contedo
			if (arq != NULL)
			{
				if (cont > 0)
				{
					// Aloca mem�ia para o contedo do arquivo
					conteudo = (char *)malloc(sizeof(char) * (cont+1));
					// L�o arquivo
					cont = fread(conteudo,sizeof(char),cont,arq);
					// E marca o final com \0
					conteudo[cont] = '\0';
				}
				fclose(arq);
			}
		}

		// Finalmente, retorna o contedo
		return conteudo;
	}

	// Fun�o que recebe o handle de um shader/programa e retorna
	// o infolog da ltima compila�o ou liga�o
	void MostraInfoLog(GLuint shader)
	{
		int tamanho = 0;	// tamanho do infolog
		int total = 0;		// total de caracteres escritos na string
		char *infolog;		// infolog

		// Obt� tamanho do infolog
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tamanho);

		// Se h�algo no infolog...
		if (tamanho > 1)
		{
			// Aloca mem�ia e obt� o infolog
			infolog = (char *)malloc(tamanho);
			glGetShaderInfoLog(shader, tamanho, &total, infolog);
			// Exibe na tela
			puts(infolog);
			// Finalmente, libera o infolog
			free(infolog);
		}
	}

	// Fun�o que recebe os nomes de dois arquivos-texto, contendo
	// os c�igos-fonte do vertex shader e do fragment shader
	GLuint CompilaShaders(char *vertsrc, char *fragsrc)
	{
		// Cria os dois shaders
		GLuint v = glCreateShader(GL_VERTEX_SHADER);
		GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

		// Carrega os arquivos
		GLchar *vs = CarregaArquivo(vertsrc);
		GLchar *fs = CarregaArquivo(fragsrc);

		// Associa aos apontadores
		const GLchar *ff = fs;
		const GLchar *vv = vs;

		// Envia os c�igos-fonte para OpenGL
		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		// E libera os mesmos
		free(vs);
		free(fs);

		// Compila o vertex shader e mostra o log da compila�o
		glCompileShader(v);
		MostraInfoLog(v);

		// Verifica se a compila�o teve sucesso
		GLint compilacaoOK;
		glGetShaderiv(v, GL_COMPILE_STATUS, &compilacaoOK);
		if(!compilacaoOK)
		{
			//fprintf(stderr, "[ERRO]:  %s\n",vertsrc);
			fprintf(stderr,"[ERRO]: %s %s\n", Language[LG_VERTEX_SHADER_ERROR], vertsrc);
			exit(1);
		}

		// Compila o fragment shader e mostra o log da compila�o
		glCompileShader(f);
		MostraInfoLog(f);

		// Verifica se a compila�o teve sucesso
		glGetShaderiv(f, GL_COMPILE_STATUS, &compilacaoOK);
		if(!compilacaoOK)
		{
			//fprintf(stderr, "[ERRO]: Nao foi possiel compilar o fragment shader %s\n",fragsrc);
			fprintf(stderr,"[ERRO]: %s %s\n", Language[LG_FRAG_SHADER_ERROR], fragsrc);
			exit(1);
		}

		// Cria o programa GLSL
		GLuint prog = glCreateProgram();
		// E associa os shaders a ele
		glAttachShader(prog,f);
		glAttachShader(prog,v);

		// Faz a liga�o dos shaders
		glLinkProgram(prog);
		MostraInfoLog(prog);

		// Verifica se a liga�o teve sucesso
		GLint ligacaoOK;
		glGetShaderiv(prog, GL_LINK_STATUS,
			&ligacaoOK);
		if (!ligacaoOK)
		{
			//fprintf(stderr, "[ERRO]: Nao foi possiel fazer a ligacao dos shaders\n");
			fprintf(stderr,"[ERRO]: %s \n", Language[LG_SHADER_ERROR]);
			exit(1);
		}

		// Finalmente, retorna o programa
		return prog;
	}

	void initShader( const char *v, const char *f) {
		// Carrega e compila os shaders
		//prog = CompilaShaders("spotlight2.vert","spotlight2.frag");
		prog = CompilaShaders((char*)v,(char*)f);

		// Armazena posi�o da vari�el "interna"
		uinterna = glGetUniformLocation(prog,"interna");
	}

	void renderShader(void) {
			// Ativa programa
			glUseProgram(prog);
			// E envia o valor atual da borda
			double cosborda = cos((20.0-1)*M_PI/180.0);
			glUniform1f(uinterna, cosborda);
	}

	void resetShader(void) {
		glUseProgram(0);
	}

};
