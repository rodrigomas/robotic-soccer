#ifndef FIELD_H
#define FIELD_H

#include "vector3d.h"
#include "libsoccer.h"
#include "common.h"

//extern bool use_tga;

namespace soccer {

	class CSoccerField {

		Color color;

		TEX *texture;
		TEX *rede;
		TEX *arquibancada;
		TEX *propaganda;
		TEX *cilin;
		TEX *concreto;
		TEX *madeira;

		unsigned int listID;

		void desenhaPropagandaP(int s = 1 )
		{
			glPushMatrix();
				glTranslatef( 0.0, 0.0, s * (70.0));
				glRotatef( - s * 90, 0.0, 1.0, 0.0);
				desenhaPropaganda(1,20,0);
			glPopMatrix();
		}

		void desenhaPropaganda(int s = 1, int size3 = 20, int xd = 65 )
		{

			resetMaterial();

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
				applyMaterial("people",true);
			}

			glColor4f(0.7f,0.7f,0.7f,1.0f);

			glMatrixMode( GL_TEXTURE );
			glPushMatrix();
			glScalef(2.0, 1.0, 1.0);

			glMatrixMode( GL_MODELVIEW );

			// Propaganda
			glPushMatrix();
			glTranslatef( s * (xd), 0.0, 0.0);
			glScalef(0.5, 0.5, size3);

				glDisable(GL_CULL_FACE);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, propaganda->texid);
				glBegin(GL_QUADS);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(3.0, 0.0, -3.0); // v6
					glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0, 3.0, -3.0); //v2
					glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0, 3.0, 3.0); //v3
					glTexCoord2f(0.0f, 0.0f); glVertex3f(3.0, 0.0, 3.0); // v5
				glEnd();

				glDisable(GL_TEXTURE_2D);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, propaganda->texid);
				glBegin(GL_QUADS);
					glTexCoord2f(1.0f, 0.0f); glVertex3f(-3.0, 0.0, -3.0); // v1
					glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.0, 3.0, -3.0); //v2
					glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0, 3.0, 3.0); //v3
					glTexCoord2f(0.0f, 0.0f); glVertex3f(-3.0, 0.0, 3.0); // v4
				glEnd();
				glDisable(GL_TEXTURE_2D);

				glEnable(GL_CULL_FACE);

			glPopMatrix();

			glMatrixMode( GL_TEXTURE );
			glPopMatrix();

			glMatrixMode( GL_MODELVIEW );

		}

		void desenhaArquiG( int s = 1, int size2 = 60, int xd = 70 )
		{
			resetMaterial();
			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
				applyMaterial("people",true);
			}

			glColor4f(0.7f,0.7f,0.7f,1.0f);

			glPushMatrix();
				glTranslatef( s * (xd), 0.0, 0.0);
				//Base
                glMatrixMode( GL_TEXTURE );
                glPushMatrix();
                glScalef(4.0, 2.0, 1.0);
                glMatrixMode( GL_MODELVIEW );

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, concreto->texid);

				glPushMatrix();
					glTranslatef( s * (10.0), 1.5, 0.0);
					glScalef(20.0, 3.0, 2 * size2);
					glCallList(listID+2);
				glPopMatrix();

				glDisable(GL_TEXTURE_2D);

                glMatrixMode( GL_TEXTURE );
                glPopMatrix();
                glMatrixMode( GL_MODELVIEW );

				//Para-Peito
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, cilin->texid);

				glPushMatrix();
					glTranslatef( s * (0.25), 3.5, 0.0);
					glScalef(0.5, 1.0, 2 * size2);
					glCallList(listID+2);
				glPopMatrix();

				glDisable(GL_TEXTURE_2D);

				// Arquibancada
				glDisable(GL_CULL_FACE);
				glPushMatrix();
					glTranslatef( 2.0 * s, 3.0, 0.0);
					glScalef( s * 18.0, 8.0, size2);

                    glMatrixMode( GL_TEXTURE );
                    glPushMatrix();
                    glScalef(4.0, 2.0, 1.0);
                    glMatrixMode( GL_MODELVIEW );
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, arquibancada->texid);

					glBegin(GL_QUADS);
						glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0, 0.0, -1.0); // v1
						glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0, 1.0, -1.0); //v2
						glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0, 1.0, 1.0); //v3
						glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0, 0.0, 1.0); // v4
					glEnd();
					glDisable(GL_TEXTURE_2D);
                    glMatrixMode( GL_TEXTURE );
                    glPopMatrix();
                    glMatrixMode( GL_MODELVIEW );

					// Arquibancada - Lateral
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, concreto->texid);

					glBegin(GL_TRIANGLES);
						glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0, 0.0, 1.0); // v4
						glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0, 1.0, 1.0); //v3
						glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0, 0.0, 1.0); //v5
					glEnd();
					glDisable(GL_TEXTURE_2D);

					// Arquibancada - Lateral 2
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, concreto->texid);

					glBegin(GL_TRIANGLES);
						glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0, 0.0, -1.0); // v4
						glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0, 1.0, -1.0); //v3
						glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0, 0.0, -1.0); //v5
					glEnd();
					glDisable(GL_TEXTURE_2D);

				glPopMatrix();
				glEnable(GL_CULL_FACE);

                glMatrixMode( GL_TEXTURE );
                glPushMatrix();
                glScalef(4.0, 2.0, 1.0);
                glMatrixMode( GL_MODELVIEW );

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, concreto->texid);
				glPushMatrix();
					glTranslatef( s * (19.5), 7.5, 0.0);
					glScalef(0.5, 9.0, 2 * size2);
					glCallList(listID+2);
				glPopMatrix();
				glDisable(GL_TEXTURE_2D);

                glMatrixMode( GL_TEXTURE );
                glPopMatrix();
                glMatrixMode( GL_MODELVIEW );
			glPopMatrix();

			glDisable(GL_COLOR_MATERIAL);
			resetMaterial();
		}

		void desenhaArquiP( int s = 1 )
		{
			glPushMatrix();
				glTranslatef( 0.0, 0.0, s * (75.0 + 1.5));
				glRotatef( - s * 90, 0.0, 1.0, 0.0);
				desenhaArquiG(1,40,0);
			glPopMatrix();
		}

		void desenhaTrave( int s = 1)
		{
			// Traves
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glBegin(GL_LINE_LOOP);
				glVertex3f(-goalWidth2, 0.0, s * 60);
				glVertex3f( goalWidth2, 0.0, s * 60);
				glVertex3f( goalWidth2, goalHeight, s * 60);
				glVertex3f(-goalWidth2, goalHeight, s * 60);
			glEnd();

			glColor4f(0.7f,0.7f,0.7f,0.7f);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_CLAMP_TO_EDGE);//GL_CLAMP_TO_EDGE
			glDisable(GL_CULL_FACE);


			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rede->texid);
			glBegin(GL_TRIANGLES);
				glNormal3d(-1.0,0.0,0.0);
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-goalWidth2, 0.0, s * 60); // v5
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-goalWidth2, 0.0, s * (60 + 3.0)); //v4
				glTexCoord2f(1.0f, 1.0f); glVertex3f(-goalWidth2, goalHeight, s * 60); //v3
			glEnd();
			glDisable(GL_TEXTURE_2D);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rede->texid);
			glBegin(GL_TRIANGLES);
				glNormal3d(1.0,0.0,0.0);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( goalWidth2, 0.0, s * 60);
				glTexCoord2f(0.0f, 0.0f); glVertex3f( goalWidth2, 0.0, s * (60 + 3.0));
				glTexCoord2f(1.0f, 1.0f); glVertex3f( goalWidth2, goalHeight, s * 60);
			glEnd();
			glDisable(GL_TEXTURE_2D);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rede->texid);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-goalWidth2, 0.0, s * (60 + 3.0)); // v4
				glTexCoord2f(1.0f, 0.0f); glVertex3f( goalWidth2, 0.0, s * (60 + 3.0)); // v1
				glTexCoord2f(1.0f, 1.0f); glVertex3f( goalWidth2, goalHeight, s * 60); //v3
				glTexCoord2f(0.0f, 1.0f); glVertex3f(-goalWidth2, goalHeight, s * 60); //v2
			glEnd();
			glDisable(GL_TEXTURE_2D);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		void desenhaBanco( int s = 1 )
		{

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
				applyMaterial("people",true);
			}

			// Parede ESq
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, concreto->texid);
			glPushMatrix();
			glTranslatef(-54.0,1.5, s * 35.0);
			glScalef(8.0,3.0,1.0);
				glColor4f(0.7f,0.7f,0.7f,1.0f);
				glCallList(listID+2);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			// Parede Dir
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, concreto->texid);
			glPushMatrix();
			glTranslatef(-54.0,1.5, s * 25.0);
			glScalef(8.0,3.0,1.0);
				glColor4f(0.7f,0.7f,0.7f,1.0f);
				glCallList(listID+2);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			// Parede de Tras
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, concreto->texid);
			glPushMatrix();
			glTranslatef(-58.0,1.5, s * 30.0);
			glScalef(1.0,3.0,10.0);
				glColor4f(0.7f,0.7f,0.7f,1.0f);
				glCallList(listID+2);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			// Teto
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, concreto->texid);
			glPushMatrix();
			glTranslatef(-54.0,3.5, s * 30.0);
			glScalef(9.0,1.0,11.0);
				glColor4f(0.7f,0.7f,0.7f,1.0f);
				glCallList(listID+2);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);
			glColor4f(0.4f,0.4f,0.4f,1.0f);

			// Banco
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, madeira->texid);
			glPushMatrix();
			glTranslatef(-55.0,0.75, s * 30.0);
			glScalef(2.0,1.5,9.0);
				glColor4f(0.7f,0.7f,0.7f,1.0f);
				glCallList(listID+2);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);

			glDisable(GL_COLOR_MATERIAL);
		}

		GLUquadric *cilindro;

	public:
		double width;
		double height;
		double lenght;

		double fieldwidth;
		double fieldlenght;

		double goalHeight;
		double goalWidth;
		double goalWidth2;

		CVector3D front;

		CVector3D pos;

		CSoccerField() : color(0.5,1,0.5,1), front(0,1,0), pos(0,0,0)
		{
			width = 120;
			height = 5;
			lenght = 140;

			fieldwidth = 90;
			fieldlenght = 120;

			//goalHeight = 2.4;
			goalHeight = 3.5;
			goalWidth = 7.3;
			goalWidth2 = goalWidth / 2.0;

			// Carrega a textura e envia para OpenGL
			texture = LoadTexture("textures/grass.jpg",true);

			if ( use_tga )
				rede =  LoadTGATexture("textures/web.tga");
			else
				rede =  LoadTexture("textures/web.jpg",true);

			arquibancada = LoadTexture("textures/arquibancada.jpg",true);
			propaganda = LoadTexture("textures/propaganda.jpg",true);
			cilin = LoadTexture("textures/cilin.jpg",true);

			concreto = LoadTexture("textures/concreto.jpg",true);

			madeira = LoadTexture("textures/madeira.jpg",true);

			LoadMaterialsFile("materials/field.mtl");

			cilindro = gluNewQuadric();
			gluQuadricDrawStyle(cilindro,GLU_FILL);
			gluQuadricNormals(cilindro,GLU_SMOOTH);
			gluQuadricOrientation(cilindro,GLU_OUTSIDE);
			gluQuadricTexture(cilindro,GL_TRUE);

			listID = getlistID();

		}

		void Draw( int mode = 1 )
		{
			glPushMatrix();
			glTranslatef(pos.x,pos.y-height/2.0,pos.z);
			resetMaterial();

			if ( mode == 0 || mode == 2 ) {

				if( useMaterial ) {
					glEnable(GL_COLOR_MATERIAL);

					resetMaterial();

					applyMaterial("field",true);
				}


				glMatrixMode( GL_TEXTURE );
				glPushMatrix();
				glScalef( 14.0f, 14.0f, 1.0f );
				glMatrixMode( GL_MODELVIEW );

				glEnable(GL_TEXTURE_2D);

				// Associa a textura aos comandos seguintes
				glBindTexture(GL_TEXTURE_2D, texture->texid);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// GL_LINEAR
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//

				// Seleciona o modo de aplica�o da textura
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//GL_REPLACE

				// Seleciona o modo de repeti�o inicial
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_REPEAT, GL_CLAMP_TO_EDGE

				glColor4f(color.R, color.G, color.B, color.A);

				glPushMatrix();
				glScalef(width,height,lenght);
					glCallList(listID+2);
				glPopMatrix();

				glMatrixMode( GL_TEXTURE );
				glPopMatrix();
				glMatrixMode( GL_MODELVIEW );

				glDisable(GL_TEXTURE_2D);
				//glDisable(GL_COLOR_MATERIAL);

				resetMaterial();

			}

			if ( mode == 1 || mode == 2 ) {
				glColor3f(1.0f,1.0f,1.0f);
				glPushMatrix();
					glScalef(width,height,lenght);
					glutWireCube(1);
				glPopMatrix();
			}
			glDisable(GL_COLOR_MATERIAL);
			glPushMatrix();
			glTranslatef(0, height/2.0 + 0.1,0);
			//glRotatef(180,0,1,0);
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			glLineWidth(3.0f);
			glPointSize(6.0f);

			// Centro
 			glBegin(GL_POINTS);
 				glVertex3f(0.0,0,0.0);
 			glEnd();

			// Circulo Central
			glutCircleL(9.15f,0,0,0);

			// Linhas Laterais
			glBegin(GL_LINE_LOOP);
				glVertex3f(-45.0,0, 60.0);
				glVertex3f( 45.0,0, 60.0);
				glVertex3f( 45.0,0,-60.0);
				glVertex3f(-45.0,0,-60.0);
			glEnd();

			glutCircleS(1.0f,-45.0,0, 60.0, TO_RADIANS(90),  TO_RADIANS(180));
			glPushMatrix();
				glDisable(GL_CULL_FACE);
				glTranslatef(-45.0, 0,60.0);
				glRotatef(-90.0, 1, 0, 0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, cilin->texid);
				gluCylinder(cilindro, 0.25f,0.25f, 3.0f, 10, 10);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_CULL_FACE);
			glPopMatrix();

			glutCircleS(1.0f, 45.0,0, 60.0, TO_RADIANS(180),  TO_RADIANS(270));
			glPushMatrix();
				glDisable(GL_CULL_FACE);
				glTranslatef(45.0, 0,60.0);
				glRotatef(-90.0, 1, 0, 0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, cilin->texid);
				gluCylinder(cilindro, 0.25f,0.25f, 3.0f, 10, 10);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_CULL_FACE);
			glPopMatrix();

			glutCircleS(1.0f,-45.0,0,-60.0,0, TO_RADIANS(90));//
			glPushMatrix();
				glDisable(GL_CULL_FACE);
				glTranslatef(-45.0, 0,-60.0);
				glRotatef(-90.0, 1, 0, 0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, cilin->texid);
				gluCylinder(cilindro, 0.25f,0.25f, 3.0f, 10, 10);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_CULL_FACE);
			glPopMatrix();

			glutCircleS(1.0f, 45.0,0,-60.0, TO_RADIANS(-90), 0);
			glPushMatrix();
				glDisable(GL_CULL_FACE);
				glTranslatef(45.0, 0,-60.0);
				glRotatef(-90.0, 1, 0, 0);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, cilin->texid);
				gluCylinder(cilindro, 0.25f,0.25f, 3.0f, 10, 10);
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_CULL_FACE);
			glPopMatrix();

			// Linha do Meio de Campo
			glBegin(GL_LINES);
				glVertex3f(-45.0,0,  0.0);
				glVertex3f( 45.0,0,  0.0);
			glEnd();

			// Grande Área (1)
			glBegin(GL_LINE_LOOP);
				glVertex3f( 20.15,0, 60.0);
				glVertex3f(-20.15,0, 60.0);
				glVertex3f(-20.15,0, 43.5);
				glVertex3f( 20.15,0, 43.5);
			glEnd();
			// Pequena Area
			glBegin(GL_LINE_LOOP);
				glVertex3f( 9.15,0, 60.0);
				glVertex3f(-9.15,0, 60.0);
				glVertex3f(-9.15,0, 54.5);
				glVertex3f( 9.15,0, 54.5);
			glEnd();

			// Meia Lua
			glutCircleS(9.15f,0,0,49.0, TO_RADIANS(180 - 54.63), TO_RADIANS(180 + 54.63));

			// Circulo do Penalti
			glBegin(GL_POINTS);
				glVertex3f(0.0,0,49.0);
			glEnd();

			// Grande Área (2)
			glBegin(GL_LINE_LOOP);
				glVertex3f( 20.15,0,-60.0);
				glVertex3f(-20.15,0,-60.0);
				glVertex3f(-20.15,0,-43.5);
				glVertex3f( 20.15,0,-43.5);
			glEnd();

			// Pequena Area
			glBegin(GL_LINE_LOOP);
				glVertex3f( 9.15,0, -60.0);
				glVertex3f(-9.15,0, -60.0);
				glVertex3f(-9.15,0, -54.5);
				glVertex3f( 9.15,0, -54.5);
			glEnd();

			// Circulo do Penalti
			glBegin(GL_POINTS);
				glVertex3f(0.0,0,-49.0);
			glEnd();


			glutCircleS(9.15f,0,0,-49.0, -TO_RADIANS(54.63), TO_RADIANS(54.63));

			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);
				resetMaterial();
			}

			desenhaTrave();
			desenhaTrave(-1);

			desenhaBanco();
			desenhaBanco(-1);

			desenhaArquiG();
			desenhaPropaganda();

			desenhaPropagandaP();
			desenhaPropagandaP(-1);

			desenhaArquiG(-1);

			desenhaArquiP(-1);
			desenhaArquiP();

			glPopMatrix();
			glLineWidth(1.0f);
			glPointSize(1.0f);

			glPopMatrix();

			glDisable(GL_COLOR_MATERIAL);
			resetMaterial();
		}

		~CSoccerField()
		{
			gluDeleteQuadric(cilindro);
		}

	};

};

#endif
