#include "sound.h"
#include "common.h"

#define NUM_SOUNDS 14
#define NUM_BUFFERS NUM_SOUNDS
// Maximum emissions we will need.
#define NUM_SOURCES NUM_SOUNDS

static const char SOUND_FILES[NUM_SOUNDS][255] = {
	"sounds/apito.wav", "sounds/colisao_jxj.wav", "sounds/grito_torcida.wav", "sounds/arq_normal.wav",
	"sounds/colisao_soco.wav", "sounds/lose.wav","sounds/recebe_soco.wav","sounds/arq_prox.wav",
	"sounds/falta.wav", "sounds/mclick.wav", "sounds/chute.wav", "sounds/gol.wav", "sounds/mover.wav",
	"sounds/win.wav"
};

// Buffers hold sound data.
ALuint Buffers[NUM_BUFFERS];

// Sources are points of emitting sound.
ALuint Sources[NUM_SOURCES];

// Position of the source sounds.
ALfloat SourcesPos[NUM_SOURCES][3];

// Velocity of the source sounds.
ALfloat SourcesVel[NUM_SOURCES][3];

// Position of the listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

bool useSound;

void playSound( int Source )
{
	if( useSound ) {
		alSourcePlay(Sources[Source]);
	}
}

ALboolean LoadALData()
{
    // Variables to load into.

    ALenum format;
    //ALsizei size;
	int size;
    char* data;
	int freq;
    //ALsizei freq;
    //ALboolean loop;

    // Load wav data into buffers.

    alGenBuffers(NUM_BUFFERS, Buffers);

    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    for( register int i = 0 ; i < NUM_SOUNDS ; i++ ) {
    	//alutLoadWAVFile( (ALbyte*)SOUND_FILES[i], &format, &data, &size, &freq, &loop);

	data = loadWAVData(SOUND_FILES[i],size,freq,format);

	//format = AL_FORMAT_MONO16;ad->chans == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,

    	alBufferData( Buffers[i], format, data, size, freq);

	delete[] data;

    	//alutUnloadWAV(format, data, size, freq);
    }
    // Bind buffers into audio sources.

    alGenSources(NUM_SOURCES, Sources);

    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    for( register int i = 0 ; i < NUM_SOUNDS ; i++ ) {
	alSourcei (Sources[i], AL_BUFFER,   Buffers[i]  );
	alSourcef (Sources[i], AL_PITCH,    1.0              );
	alSourcef (Sources[i], AL_GAIN,     1.0              );
	alSourcefv(Sources[i], AL_POSITION, SourcesPos[i]);
	alSourcefv(Sources[i], AL_VELOCITY, SourcesVel[i]);
	alSourcefv(Sources[i], AL_DIRECTION, SourcesVel[i]);
	alSourcei (Sources[i], AL_LOOPING,  AL_FALSE        );
    }

    // Do another error check and return.

    if( alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    return AL_TRUE;
}

void setGain( int Source, int value )
{
	alSourcef (Sources[Source], AL_GAIN, value);
}

void setSourcePos( int Source, float x, float y, float z )
{
	SourcesPos[Source][0] = x;
	SourcesPos[Source][1] = y;
	SourcesPos[Source][2] = z;

	alSourcefv(Sources[Source], AL_POSITION, SourcesPos[Source]);
}

void setListenerPos( float x, float y, float z )
{
	ListenerPos[0] = x;
	ListenerPos[1] = y;
	ListenerPos[2] = z;

	alListenerfv(AL_POSITION,    ListenerPos);
}

void setListenerVel( float x, float y, float z )
{
	ListenerVel[0] = x;
	ListenerVel[1] = y;
	ListenerVel[2] = z;

	alListenerfv(AL_VELOCITY,    ListenerVel);
}

void setSourceVel( int Source, float x, float y, float z )
{
	SourcesVel[Source][0] = x;
	SourcesVel[Source][1] = y;
	SourcesVel[Source][2] = z;

	alSourcefv(Sources[Source], AL_VELOCITY, SourcesVel[Source]);
	alSourcefv(Sources[Source], AL_DIRECTION, SourcesVel[Source]);
}

void setListenerOri( float x, float y, float z )
{
	ListenerOri[0] = x;
	ListenerOri[1] = y;
	ListenerOri[2] = z;

	alListenerfv(AL_ORIENTATION, ListenerOri);
}

void SetListenerValues()
{
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
}

bool initOpenAL(void)
{
	ALCcontext* audiocontext;
	ALCdevice* audiodevice = alcOpenDevice(NULL);
	if(audiodevice)
	{
		audiocontext = alcCreateContext(audiodevice, NULL);
		if(audiocontext)
		{
			alcMakeContextCurrent(audiocontext);
			return true;
		}
		alcCloseDevice(audiodevice);
	}
	return false;
}

bool shutdownOpenAL(void)
{
	ALCcontext* audiocontext = alcGetCurrentContext();
	ALCdevice* audiodevice = alcGetContextsDevice(audiocontext);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(audiocontext);
	alcCloseDevice(audiodevice);

	return true;
}

void KillALData()
{
#ifdef USE_MUSIC
//     if ( ogg_play ) {
// 	   ogg_play = false;
//        ogg.release();
//     }
#endif
  	if( useSound ) {
    		alDeleteBuffers(NUM_BUFFERS, &Buffers[0]);
    		alDeleteSources(NUM_SOURCES, &Sources[0]);
	}

    	shutdownOpenAL();

    //alutExit();
}

void StopMusic(void)
{
#ifdef USE_MUSIC
	try {
		ogg.release();
	} catch( std::string &a ) {
		//std::cerr << a;
	}
#endif
}

void StartMusic(void)
{
#ifdef USE_MUSIC
	try {
        ogg.open("sounds/music.ogg");
	} catch( std::string &a ) {
		fprintf(stderr,"[ERRO]: %s \n",Language[LG_OPENAL_OGG_ERROR]);
	}
	ogg.playback();
	ogg_play = true;
#endif
}

void aluMain(int *argc, char **argv)
{
	if( !(useSound = initOpenAL()) ) {
		fprintf(stderr,"[ERRO]: %s \n",Language[LG_OPENAL_ERROR]);
	}

// 	if( alutInit(argc, argv) == AL_FALSE ) {
// 	    fprintf(stderr,"Error loading data.");
//     	}

	alGetError();

	if(LoadALData() == AL_FALSE)
	{
	    //fprintf(stderr,"[ERRO]: Falha ao carregar arquivo de audio.\n");
	    fprintf(stderr,"[ERRO]: %s \n",Language[LG_OPENAL_FILE_ERROR]);
		return;
	}

	SetListenerValues();

	alDopplerFactor(gdata->physics.dopplerfactor);
	alDopplerVelocity(gdata->physics.sndspeed);
	//alSpeedOfSound(gdata->physics.sndspeed);
	alDistanceModel(AL_INVERSE_DISTANCE);
}

