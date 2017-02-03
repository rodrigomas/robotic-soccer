#ifndef PLACAR_H
#define PLACAR_H

#include "libsoccer.h"
#include "vector3d.h"
#include "common.h"
#include <string>

using std::string;

namespace soccer {

	class CPlacar {

		TEX *concreto;

		char data[20];

		GLUquadric *cilindro;

		string message;

		unsigned int listID;

	public:

		CVector3D pos;

		int gols1;

		int gols2;

		int angle;

		double size;

		string team1;

		string team2;

		CPlacar() : pos(0.0,0.0,0.0)
		{
			concreto = LoadTexture("textures/concreto.jpg",true);

			gols1 = gols2 = angle = 0;

			size = 2.0;

			team1 = "BRA";
			team2 = "BRA";

			LoadMaterialsFile("materials/placar.mtl");

			cilindro = gluNewQuadric();
			gluQuadricDrawStyle(cilindro,GLU_FILL);
			gluQuadricNormals(cilindro,GLU_SMOOTH);
			gluQuadricOrientation(cilindro,GLU_OUTSIDE);
			gluQuadricTexture(cilindro,GL_TRUE);

			listID = getlistID();
		}	

		~CPlacar()
		{
			gluDeleteQuadric(cilindro);	
		}

		void Draw(void)
		{

			resetMaterial();
	
			if( useMaterial ) {
				glEnable(GL_COLOR_MATERIAL);	
				resetMaterial();
				applyMaterial("placar",true);
			}	

			glPushMatrix();
				// Posiciona e orienta o observador
				glTranslatef(pos.x,pos.y,pos.z);
				glScalef(size,size,size);
				glRotatef(angle,0.0,1.0,0.0);
				glDisable(GL_CULL_FACE);
				glPushMatrix();
					glRotatef(-90.0, 1, 0, 0);	
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, concreto->texid);
					gluCylinder(cilindro, 0.9f,0.9f, 10.0f, 10, 10);
					glDisable(GL_TEXTURE_2D);
					glEnable(GL_CULL_FACE);
				glPopMatrix();
	
				glPushMatrix();
					glTranslatef(0.0,12.0,0.0);
					glPushMatrix();
						glScalef(10.0,6.0,2.0);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, concreto->texid);
						glCallList(listID+2);
						glDisable(GL_TEXTURE_2D);
	
						glDisable(GL_CULL_FACE);
						glColor3f(0.0f,0.0,0.0);
						glScalef(0.8,0.8,1.0);
						glBegin(GL_QUADS);	
							glVertex3f( 0.5, 0.5, 0.51); // v4
							glVertex3f( 0.5,-0.5, 0.51); // v1
							glVertex3f(-0.5,-0.5, 0.51); //v3
							glVertex3f(-0.5, 0.5, 0.51); //v2
						glEnd();
						glEnable(GL_CULL_FACE);
					glPopMatrix();
	
					sprintf(data,"%.2d x %.2d",gols1 % 11, gols2 % 11);
					message = team1 + " " + team2;
	
					glPushMatrix();
						glDisable(GL_CULL_FACE);
						glColor3f(1.0f,1.0,0.0);
						glTranslatef(-3.0,-1.0,1.05);
						glScalef(0.01,0.01,0.01);
						glLineWidth(3.0);
						puts2DStroke(data);						
						glEnable(GL_CULL_FACE);
					glPopMatrix();
	
					glPushMatrix();
						glDisable(GL_CULL_FACE);
						glColor3f(1.0f,1.0,0.0);
						glTranslatef(-3.0,1.0,1.05);
						glScalef(0.01,0.01,0.01);
						glLineWidth(3.0);
						puts2DStroke(message.c_str());	
						glEnable(GL_CULL_FACE);
					glPopMatrix();
			glPopMatrix();

			glDisable(GL_COLOR_MATERIAL);	
			resetMaterial();		
		}

	};
};

#endif
