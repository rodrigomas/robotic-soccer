extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <iostream>
#include <pthread.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WIN32
	#include <windows.h>
#endif

#ifdef USE_MUSIC
	#ifndef USE_SOUND
	#define USE_SOUND
	#endif
#endif

#ifdef USE_SOUND
#include "sound.h"
#endif

#ifdef USE_MUSIC
ogg_stream ogg;
bool ogg_play;
#endif

#include "scene_01.h"
#include "scene_02.h"
#include "scene_03.h"
#include "scene_04.h"
#include "scene_05.h"
#include "scene_06.h"
#include "keyhelper.h"
#include "gamedata.h"

#include "translator.h"

using namespace std;
using namespace soccer;

bool useBodyView = false;
bool use_tga = false;
bool useShader = false;
SGameData *gdata;
lua_State *lua_team1, *lua_team2;

bool useMaterial;
bool useFog;
bool useLight;
int LastCollisionIndex;

CScene01 *Scene01 = NULL;
CScene02 *Scene02 = NULL;
CScene03 *Scene03 = NULL;
CScene04 *Scene04 = NULL;
CScene05 *Scene05 = NULL;
CScene06 *Scene06 = NULL;

CScene *CurrScene = NULL;

soccer::CTranslator Language;

pthread_t sound_id;
pthread_mutex_t snd_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef USE_MUSIC
void *updateSound( void* s )
{
	//pthread_t myid = pthread_self();

	bool local_ogg_play = false;

	#ifdef USE_MUSIC
	while ( true ) {

		pthread_mutex_lock(&snd_mutex);
		local_ogg_play = ogg_play;
		pthread_mutex_unlock(&snd_mutex);

		if( local_ogg_play ) {
			if( ogg.update() )  {
				ogg.playback();
			} else {
				break;
			}
		} else {
			break;
		}
		//std::cerr << "[" << (int)myid << "]:UPDATE!" << std::endl;
	}
	#endif

	//std::cerr << "[" << (int)myid << "]:OK!" << std::endl;

	pthread_exit(NULL);

	return NULL;
}
void initSound(void)
{

	StartMusic();
	pthread_create( &sound_id, NULL, updateSound, NULL );
}

void releaseSound( void )
{
	pthread_mutex_lock(&snd_mutex);
	ogg_play = false;
	pthread_mutex_unlock(&snd_mutex);

	pthread_join(sound_id,NULL);

	StopMusic();
}
#endif
static void glutSpecial0( int key, int x, int y)
{

	if( key == GLUT_KEY_F10 )
		screenCapture();

	specialKeyboard(key,x,y);
	CurrScene->glutSpecial(key,x,y);
}

static void glutSpecialUp0( int key, int x, int y)
{
	specialKeyboardUp(key,x,y);
}

static void glutKeyboard0( unsigned char key, int x, int y)
{
	if( key == 27 )
		Terminate();

	keyboard(key,x,y);
	CurrScene->glutKeyboard(key,x,y);
}

static void glutKeyboardUp0( unsigned char key, int x, int y)
{
	keyboardUp(key,x,y);
}

static void glutMouse0( int button, int state, int x, int y)
{
	CurrScene->glutMouse( button, state, x, y);
}

static void glutMotion0( int x, int y)
{
	CurrScene->glutMotion(x,y);
}

static void glutPassiveMotion0( int x, int y)
{
	CurrScene->glutPassiveMotion(x,y);
}

static void glutDisplay0(void)
{
	CurrScene->glutDisplay();
}

static void glutReshape0( int w, int h )
{
	CurrScene->glutReshape(w,h);
}

static void glutIdle0(void)
{
	CurrScene->glutIdle();
}

void RegisterCallBacks(void) {

	glutSpecialFunc(glutSpecial0);
	glutKeyboardFunc(glutKeyboard0);
	glutMouseFunc(glutMouse0);
	glutMotionFunc(glutMotion0);
	glutPassiveMotionFunc(glutPassiveMotion0);
	glutDisplayFunc(glutDisplay0);
	glutReshapeFunc(glutReshape0);
	glutIdleFunc(glutIdle0);
    	glutKeyboardUpFunc( glutKeyboardUp0 );
	glutSpecialUpFunc( glutSpecialUp0 );

	glutIgnoreKeyRepeat(0);
}

void RenderParameters(void) {
	// Transparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Antialising
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	//glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);//GL_NICEST, GL_DONT_CARE, GL_FASTEST
 	glEnable(GL_AUTO_NORMAL);
 	glEnable(GL_NORMALIZE);

	//FADE_STEPS = gdata->graphics.fadesteps;
	initEffects( gdata->graphics.fadetimer );
//	FADE_TIMER = gdata->graphics.fadetimer;

	Scene01 = new CScene01(gdata->graphics.fadesteps);
	Scene02 = new CScene02(gdata->graphics.fadesteps);
	Scene03 = new CScene03(gdata->graphics.fadesteps);
	Scene04 = new CScene04(gdata->graphics.fadesteps);
	Scene06 = new CScene06(gdata->graphics.fadesteps);

	// Inicializa a GLEW e verifica se houve algum erro
	GLint erro = glewInit();
	if(erro!=GLEW_OK)
	{
		fprintf(stderr,"[ERRO]: %s\n",Language[LG_SHADER_OPEN_ERROR]);
		useShader = false;
		gdata->graphics.useshader = false;
	}

	// Verifica se h�suporte para as extens�s GLSL
	if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
	{
		fprintf(stderr,"[ERRO]: %s\n",Language[LG_SHADER_SUPPORT_ERROR]);
		useShader = false;
		gdata->graphics.useshader = false;
	}

	if( useShader ){
		initShader(gdata->graphics.vextexshader.c_str(),  gdata->graphics.fragshader.c_str());
	}

	useLight = gdata->graphics.use_light;

	//glutInitJoystick("/dev/input/js0");

}

void initIA(void)
{
	//lua_team1 = lua_open();
	lua_team1 = luaL_newstate();
	luaL_openlibs(lua_team1);

	int status = luaL_loadfile(lua_team1, gdata->team1->ia.c_str()) || lua_pcall(lua_team1, 0, 0, 0);
	if (status) {
		fprintf(stderr, "[ERRO]: %s %s.\n", Language[LG_LUA_SCRIPT_ERROR], gdata->team1->ia.c_str());
		Terminate();
	}

 	//lua_team2 = lua_open();
 	lua_team2 = luaL_newstate();
 	luaL_openlibs(lua_team2);

 	status = luaL_loadfile(lua_team2, gdata->team2->ia.c_str()) || lua_pcall(lua_team2, 0, 0, 0);
 	if (status) {
 		fprintf(stderr, "[ERRO]: %s %s.\n", Language[LG_LUA_SCRIPT_ERROR], gdata->team2->ia.c_str());
 		Terminate();
 	}
}

void releaseIA(void)
{
	//lua_pop(lua_team1, 1);
	lua_close(lua_team1);

	//lua_pop(lua_team2, 1);
	lua_close(lua_team2);
}

void Terminate(void)
{
	if( Scene01 )
		delete Scene01;

	if( Scene02 )
		delete Scene02;

	if( Scene03 )
		delete Scene03;

	if( Scene04 )
		delete Scene04;

	if( Scene05 )
		delete Scene05;

	if( Scene06 )
		delete Scene06;

	//glutLeaveMainLoop();
	freeSCCList();

	deleteConfigData(gdata);

	releaseIA();

	#ifdef USE_MUSIC
	releaseSound();
	#endif

	#ifdef USE_SOUND
	KillALData();
	#endif

	fclose(stderr);

	#ifdef WIN32
        HWND handle = GetForegroundWindow();
		MessageBox( handle, THANKS,"Robotics Soccer", MB_ICONINFORMATION | MB_OK );
	#else
		//std::cout << std::endl << "Obrigado por jogar!" << std::endl << std::endl;
        fprintf(stdout, THANKS);
	#endif

	exit(0);
}

void ChangeScene( int ID,  void* info )
{

	int w, h;
	bool fadeIn = false;

	if ( CurrScene ) {
		CurrScene->getWH(w,h);
	} else {
		w = glutGet(GLUT_WINDOW_WIDTH);
		h = glutGet(GLUT_WINDOW_HEIGHT);
	}
	#ifdef USE_SOUND
	setListenerPos( 0.0, 0.0, 0.0 );
	setListenerVel( 0.0, 0.0, 0.0 );
	#endif

	switch(ID)
	{
		case 1:
			Scene01->setWH(w,h);
			CurrScene = Scene01;
		break;

		case 2:
			Scene02->setWH(w,h);
			CurrScene = Scene02;
		break;

		case 3:
			Scene03->setWH(w,h);
			Scene03->getInfo(info);
			Scene03->loading = false;
			CurrScene = Scene03;
			fadeIn = true;

		break;

		case 4:
			if ( Scene05 ) {
				delete Scene05;
				Scene05 = NULL;
			}

			#ifdef USE_MUSIC
			initSound();
			#endif

			Scene04->setWH(w,h);
			Scene04->getInfo(info);
			CurrScene = Scene04;
			fadeIn = true;
		break;

		case 5:
			if ( !Scene05 )
				Scene05 = new CScene05( (int)info, gdata->graphics.fadesteps );

			#ifdef USE_MUSIC
			//StopMusic();
			releaseSound();
			#endif

			Scene05->setWH(w,h);
			CurrScene = Scene05;

			fadeIn = true;
		break;

		case 6:
			Scene06->setWH(w,h);
			CurrScene = Scene06;
		break;

		default:
		break;

	}

	CurrScene->glutMotion(0,0);
	if ( !fadeIn ) {
		glutPostRedisplay();
	} else {
		SceneFadeIn(NULL,CurrScene);
	}

}

void menu(int value)
{
     switch(value)
     {
          case 1:
            Terminate();
          break;
          case 2:
            #ifdef WIN32
                HWND handle = GetForegroundWindow();
                MessageBox( handle, "\nAutor: Rodrigo Marques Almeida da Silva\n"
                                  ,"Robotics Soccer", MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
            #else
                //std::cout << std::endl << "Obrigado por jogar!" << std::endl << std::endl;
                fprintf(stdout, THANKS);
            #endif
          break;
          default:
          break;
     }

     glutPostRedisplay();
}

void initMenus(void)
{
     glutCreateMenu(menu);
     glutAddMenuEntry(Language[LG_EXIT_GAME] , 1);
     glutAddMenuEntry(Language[LG_ABOUT_GAME] , 2);
     glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initControls(void)
{
    if( gdata->usepooling ) {
        glutJoystickFunc(joystick, gdata->pool);
    } else {
        glutJoystickFunc(joystick, 0);
    }
}


// Programa Principal
int main( int argc, char *argv[] )
{
 	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM );

	int X = glutGet(GLUT_SCREEN_WIDTH);
	int Y = glutGet(GLUT_SCREEN_HEIGHT);

	X = (X - SCR_WIDTH)/2;
	Y = (Y - SCR_HEIGHT)/2;

    freopen("log.log","w", stderr);

	Language.LoadFile("default.lng");

	if( !(gdata = readConfigXml("config.xml")) ) {
		return -1;
	}

	glutGameModeString(gdata->graphics.gamemode.c_str());

    if ( gdata->graphics.usegamemode && glutGameModeGet(GLUT_GAME_MODE_POSSIBLE) ) {
        glutEnterGameMode();
    } else {
        if( gdata->graphics.fullscreen ) {
                glutInitWindowPosition(0,0);
                glutCreateWindow("Robotics Soccer by Rodrigo Marques Almeida da Silva");
                glutFullScreen();
        } else {
                glutInitWindowPosition(X,Y);
                glutInitWindowSize(SCR_WIDTH,SCR_HEIGHT);
                glutCreateWindow("Robotics Soccer by Rodrigo Marques Almeida da Silva");
        }

        initMenus();
    }

    if( gdata->languagefile != "default.lng" ) {
        Language.ReloadFile(gdata->languagefile.c_str());
    }

    glutSetCursor(  GLUT_CURSOR_CROSSHAIR );

	useShader = gdata->graphics.useshader;

	initControls();

	LIBXML_TEST_VERSION

	xmlCleanupParser();

    xmlMemoryDump();

	#ifdef USE_SOUND
	aluMain(&argc,argv);
	#endif

 	srand(time(NULL));

	RenderParameters();
	initPhysics(gdata);
	initIA();
	startSCCLists();

	#ifdef USE_MUSIC
	initSound();
	#endif

	ChangeScene(1);
	SceneFadeIn(NULL,CurrScene);

	RegisterCallBacks();

	glutMainLoop();

	return 0;
}
