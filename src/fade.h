#ifndef FADE_H
#define FADE_H

#include "scene.h"

using soccer::CScene;

typedef void(*fFade)(void);

void glutFadeIn( int value );

void glutFadeOut( int value );

void SceneFadeIn( fFade afadein, CScene *Scene );

void SceneFadeOut( fFade afadeout, CScene *Scene );

void initEffects( int ft );

#endif
