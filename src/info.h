#ifndef INFO_H
#define INFO_H

//#include "common.h"
#define CARD_SIZE 100
#define CARD_TYPE (CARD_SIZE - 1)

namespace soccer {

	typedef struct {

		int nGols;
		int nFaltas;
		int nLaterais;
		int nEscanteios;
		double Posse;
		int nCartoes;
		char** Cartoes;
		char** Gols;
	} SInfo;

	struct pass_info {
		SInfo *team01;
		SInfo *team02;
	};

	SInfo *createInfo(void);

	//OBS: A posição 99 do cartao diz o tipo do cartao v a

	void deleteInfo(SInfo *t);

	char getCardType(const char *c);

	char *newCard(void);
};

#endif
