#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <string>

using std::string;

namespace soccer {

	int min(int a, int b);

	typedef struct {

		string name;
		double p0x;
		double p0z;
		double pfx;
		double pfz;
		int number;
		string type;

	} SPlayerData;

	typedef struct {

		string	name;
		string	sigla;
		int nplayers;
		string basepath;
		string imbrasao;
		string imcamisa;
		string coach;
		string ia;
		bool stupid;

		SPlayerData *players;

	} STeamData;

	typedef struct {
		double gravity;
		double floorResY;
		double floorResXZ;
		double friction_player;
		double rest_playerBall;
		double resti_coef;
		double pentol;
		double air_viscosity;
		double air_density;
		bool magnus;
		bool drag;
		bool wind;
		bool rand_wind;
		double wind_dir[3];
		double maxspeed;
		double sndspeed;
		double dopplerfactor;
		string method;
	} SPhysicsData;

	typedef struct {

		unsigned int fog_filter;

		float color[4];

		float density;
		float start;
		float end;

		unsigned int mode;

	} SFogData;

	typedef struct {

		double ambient[4];
		double diffuse[4];
		double specular[4];
		float position[3];
		float direction[3];
		double atenuation[3];
		float cutoff;
		float exponent;

		string type;

	} SLightData;

    typedef struct {

        int fwd;
        int bwd;
        int left;
        int right;
        int kickY;
        int kickXZ;

    } SUserPad;

	typedef struct {
		bool use_light;
		bool draw_light;

		double ambient_light[4];

		bool useFog;

		SLightData *lights;

		int nlights;

		SFogData fog;

		float motionfactor;
		int motionframes;
		int fadesteps;
		int fadetimer;
		bool enablemotion;
		bool useshader;
		bool usecubemap;
		string vextexshader;
		string fragshader;
		bool fullscreen;
		bool usegamemode;
		string gamemode;

	} SGraphicsData;


	typedef struct {
		string team1xml;
		STeamData *team1;

		string team2xml;
		STeamData *team2;

		SPhysicsData physics;

		SGraphicsData graphics;

		string arbitro;

		string lateral1;

		string lateral2;

		SUserPad p1;
		SUserPad p2;

		bool usejoystick;
		bool usepooling;
		int pool;

		int axis[6];

		string languagefile;

	} SGameData;

	void deleteTeamData( STeamData *s );

	void deleteConfigData( SGameData *s );

	STeamData *readTeamXml( const char *filename );

	SGameData *readConfigXml( const char *filename );
};

#endif
