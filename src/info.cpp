#include "info.h"

namespace soccer {

	SInfo *createInfo(void)
	{
		SInfo *t;
		
		t = new SInfo;
		t->nGols = 0;
		t->nFaltas = 0;
		t->nLaterais = 0;
		t->nEscanteios = 0;
		t->Posse = 0.0;
		t->nCartoes = 0;
	
		t->Gols = new char*[10];
		t->Cartoes = new char*[10];
	
		return t;
	}
	
	void deleteInfo(SInfo *t)
	{
		for( int i = 0 ; i < t->nGols ; i++ ) {
			delete [] t->Gols[i];
		}
	
		delete [] t->Gols;
	
		for( int i = 0 ; i < t->nCartoes ; i++ ) {
			delete [] t->Cartoes[i];
		}
	
		delete [] t->Cartoes;
	
		delete t;
	}

	char getCardType(const char *c)
	{
		return c[CARD_TYPE];
	}

	char *newCard(void)
	{
		char *c = new char[CARD_SIZE];
		return c;

	}
};

