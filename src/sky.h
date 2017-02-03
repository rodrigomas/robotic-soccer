#ifndef SKY_H
#define SKY_H

#include "libsoccer.h"
#include "vector3d.h"
#include "common.h"

using std::string;

namespace soccer {
	
	class CSky {

		TEX *texture;
		MAT *material;
		TEX **__textures;

		CVector3D size;	

		bool cubemap;	

		unsigned int mylistID;
		unsigned int listID;

		void buildLists(void)
		{
			glNewList(mylistID,GL_COMPILE);
				glBegin(GL_QUADS);				
				//Quads 1 // Lateral 1
				glNormal3d( -1.0, 0.0, 0.0); 
				glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
				glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V1
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
				glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4				
				glEnd();
			glEndList();

			glNewList(mylistID+1,GL_COMPILE);
				//Quads 3 // Lateral 2
				glBegin(GL_QUADS);				
				glNormal3d( 1.0, 0.0, 0.0); 
				glTexCoord2f(1.0f, 0.5f); glVertex3f( -0.5f, -0.5f, -0.5f);   //V3
				glTexCoord2f(0.5f, 0.5f); glVertex3f( -0.5f,-0.5f, 0.5f);   //V1
				glTexCoord2f(0.5f, 1.0f); glVertex3f( -0.5f,0.5f,0.5f);   //V2
				glTexCoord2f(1.0f, 1.0f); glVertex3f( -0.5f, 0.5f,-0.5f);   //V4
				glEnd();
			glEndList();


			glNewList(mylistID+2,GL_COMPILE);
				//Quad 2 // Frontal
				glBegin(GL_QUADS);				
				glNormal3d( 0.0, 0.0,-1.0);
				glTexCoord2f(0.5f, 0.5f); glVertex3f( -0.5f, -0.5f,0.5f);   //V3
				glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f,-0.5f,0.5f);   //V1
				glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5f,0.5f,0.5f);   //V2
				glTexCoord2f(0.5f, 1.0f); glVertex3f(-0.5f, 0.5f,0.5f);   //V4
				glEnd();
			glEndList();


			glNewList(mylistID+3,GL_COMPILE);
				//Quad 2 // Back
				glBegin(GL_QUADS);				
				glNormal3d( 0.0, 0.0,1.0);
				glTexCoord2f(0.0f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4
				glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V3
				glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V5
				glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6
				glEnd();
			glEndList();
		
			glNewList(mylistID+4,GL_COMPILE);
				//Quad 5 // Topo
				glBegin(GL_QUADS);				
				glNormal3d( 0.0, -1.0, 0.0);
				glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.5f, 0.5f,-0.5f);   //V6
				glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);   //V8
				glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, 0.5f, 0.5f);   //V2
				glTexCoord2f(1.0f, 0.5f); glVertex3f( 0.5f, 0.5f,-0.5f);   //V4
				glEnd();
			glEndList();

			glNewList(mylistID+5,GL_COMPILE);		
				glBegin(GL_QUADS);
				//Quad 6 // Fundo
				glNormal3d( 0.0, 1.0, 0.0);
				glTexCoord2f(1.0f, 0.5f); glVertex3f(-0.5f,-0.5f, 0.5f);   //V4
				glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,-0.5f,-0.5f);   //V3
				glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.5f,-0.5f,-0.5f);   //V1
				glTexCoord2f(0.5f, 0.5f); glVertex3f( 0.5f,-0.5f, 0.5f);   //V2
				glEnd();
			glEndList();
		}

	public:

		CVector3D pos;

		CSky( CVector3D size, bool use_cubemap = false) : pos(0.0,0.0,0.0)
		{
			cubemap = use_cubemap;

			if ( cubemap ) {
				texture = LoadCubeTextures("textures/box",false);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			} else {
				this->__textures = new TEX*[6];
				this->__textures[0] = LoadTexture("textures/box_posx.jpg",true);
				this->__textures[1] = LoadTexture("textures/box_posy.jpg",true);
				this->__textures[2] = LoadTexture("textures/box_posz.jpg",true);
				this->__textures[3] = LoadTexture("textures/box_negx.jpg",true);
				this->__textures[4] = LoadTexture("textures/box_negy.jpg",true);
				this->__textures[5] = LoadTexture("textures/box_negz.jpg",true);
			}			
			
			this->size = size;

			mylistID = glGenLists(6);

			buildLists();

			LoadMaterialsFile("materials/sky.mtl");

			listID = getlistID();
		}	

		~CSky()
		{
			delete __textures;
			glDeleteLists(mylistID,6);
		}

		void Draw(void)
		{
			glPushMatrix();
			// Posiciona e orienta o observador
			resetMaterial();
			glTranslatef(pos.x,pos.y,pos.z);
			glScalef(size.x,size.y,size.z);
			glDisable(GL_CULL_FACE);
			if ( cubemap ) {		
				glEnable(GL_TEXTURE_CUBE_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
			
				glCallList(listID+4);
			
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_CUBE_MAP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			
				// Seleciona o modo de aplica�o da textura
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_CLAMP_TO_EDGE);//GL_CLAMP_TO_EDGE
				
				if( useMaterial ) {
					glEnable(GL_COLOR_MATERIAL);	
					resetMaterial();
					applyMaterial("sky",true);
				}	

				glColor3f(1.0,1.0,1.0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[0]->texid);
				glBegin(GL_QUADS);				
				//Quads 1 // Lateral 1
				glCallList(mylistID);
				glDisable(GL_TEXTURE_2D);
								
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[3]->texid);
				glCallList(mylistID+1);
				glDisable(GL_TEXTURE_2D);		

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[2]->texid);
				//Quad 2 // Frontal
				glCallList(mylistID+2);
				glDisable(GL_TEXTURE_2D);		

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[5]->texid);
				//Quad 2 // Back
				glCallList(mylistID+3);
				glDisable(GL_TEXTURE_2D);
		
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[4]->texid);
				//Quad 5 // Topo
				glCallList(mylistID+4);
				glDisable(GL_TEXTURE_2D);
		
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, this->__textures[1]->texid);	
				glCallList(mylistID+5);
				glDisable(GL_TEXTURE_2D);
						
			}
			glEnable(GL_CULL_FACE);			
			glCullFace(GL_BACK);
			glDisable(GL_COLOR_MATERIAL);
			resetMaterial();
			glPopMatrix();
		
		}

	};
};

#endif
