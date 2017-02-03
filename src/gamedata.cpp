#include "gamedata.h"
#include "common.h"

namespace soccer {

	int min(int a, int b)
	{
		return (a < b) ? a : b;
	}

	void deleteTeamData( STeamData *s )
	{
		delete [] s->players;

		delete s;
	};

	void deleteConfigData( SGameData *s )
	{

		deleteTeamData( s->team1 );

		deleteTeamData( s->team2 );

		delete [] s->graphics.lights;

		delete s;
	};

    int keyValue( const char*key, bool joy_enable = false, int player = 1) {

        string s = key;

        if( s == "UP" ) {
                return GLUT_KEY_UP | SPECIAL_KEY_SET; // Seta o bit
        }

        if( s == "DOWN" ) {
                return GLUT_KEY_DOWN | SPECIAL_KEY_SET;
        }

        if( s == "LEFT" ) {
                return GLUT_KEY_LEFT | SPECIAL_KEY_SET;
        }

        if( s == "RIGHT" ) {
                return GLUT_KEY_RIGHT | SPECIAL_KEY_SET;
        }

        if( joy_enable )
        {
            if( s == "AXIS-X+" ) {
                    return 0 | SPECIAL_KEY_SET | JOY_SET | JOY_PLUS_SET;
            }

            if( s == "AXIS-Y+" ) {
                    return 1 | SPECIAL_KEY_SET | JOY_SET | JOY_PLUS_SET;
            }

            if( s == "AXIS-Z+" ) {
                    return 2 | SPECIAL_KEY_SET | JOY_SET | JOY_PLUS_SET;
            }

            if( s == "AXIS-X-" ) {
                    return 0 | SPECIAL_KEY_SET | JOY_SET;
            }

            if( s == "AXIS-Y-" ) {
                    return 1 | SPECIAL_KEY_SET | JOY_SET;
            }

            if( s == "AXIS-Z-" ) {
                    return 2 | SPECIAL_KEY_SET | JOY_SET;
            }

            if( s == "BTN1" ) {
                    return 0 | JOY_SET;
            }

            if( s == "BTN2" ) {
                    return 1 | JOY_SET;
            }

            if( s == "BTN3" ) {
                    return 2 | JOY_SET;
            }

            if( s == "BTN4" ) {
                    return 3 | JOY_SET;
            }

        }

        return (int)key[0];

    }

	SGameData *readConfigXml( const char *filename )
	{
    		xmlChar *txt;
    		xmlDocPtr doc;
    		xmlNodePtr cur = NULL, sub = NULL, isub = NULL, iisub = NULL;

		SGameData *data;

		if( !(doc = xmlParseFile( filename ) ) ){
        		fprintf( stderr, "[ERRO]: %s %s.\n", Language[LG_XML_ERROR], filename );
        		xmlFreeDoc( doc );
        		return( NULL );
    		}

		if( !(cur = xmlDocGetRootElement( doc ) ) ){
			//fprintf( stderr, "[ERRO]: O arquivo %s não possui elemento raiz.\n", filename );
			fprintf( stderr, "[ERRO]: %s %s.\n", Language[LG_XML_ROOT_ERROR], filename );
			xmlFreeDoc( doc );
			return( NULL );
		}

		if( xmlStrcmp( cur->name, (const xmlChar *) "robotics" ) ){
			//fprintf( stderr, "[ERRO]: Arquivo de configuração invalido.\n" );
			fprintf( stderr, "[ERRO]: %s.\n", Language[LG_XML_CONF_ERROR] );
			xmlFreeDoc( doc );
			return( NULL );
		}

		//Version Checker
		if( xmlStrcmp( xmlGetProp( cur, (const xmlChar *)"version" ), (const xmlChar *) "0.2.1" ) ){
			//fprintf( stderr, "[ERRO]: Arquivo de configuração com versao invalida.\n" );
			fprintf( stderr, "[ERRO]: %s.\n", Language[LG_XML_VER_ERROR] );
			xmlFreeDoc( doc );
			return( NULL );
		}

		data = new SGameData;

        txt = xmlGetProp( cur,  (const xmlChar *)"languagefile" );
        data->languagefile = (const char*)txt;
        xmlFree( txt );

		sub = cur->children;

		while( sub != NULL ) {
			if( (!xmlStrcmp(sub->name, (const xmlChar *)"teans" ) ) ) {

				txt = xmlGetProp( sub,  (const xmlChar *)"file1" );
				data->team1xml = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"file2" );
				data->team2xml = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"arbitro" );
				data->arbitro = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"lateral1" );
				data->lateral1 = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"lateral2" );
				data->lateral2 = (const char*)txt;
				xmlFree( txt );
			}

			if( (!xmlStrcmp(sub->name, (const xmlChar *)"physics" ) ) ) {

				txt = xmlGetProp( sub,  (const xmlChar *)"method" );
				data->physics.method = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"pentol" );
				data->physics.pentol = atof((const char*)txt);
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"wind" );
				data->physics.wind = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"magnus" );
				data->physics.magnus = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"drag" );
				data->physics.drag = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"maxspeed" );
				data->physics.maxspeed = atof((const char*)txt);
				xmlFree( txt );

				isub = sub->children;

				while( isub != NULL ) {

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"gravity" ) ) ) {
						txt = xmlNodeListGetString( doc, isub->children, 1 );
						data->physics.gravity = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"sndspeed" ) ) ) {
						txt = xmlNodeListGetString( doc, isub->children, 1 );
						data->physics.sndspeed = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"dopplerfactor" ) ) ) {
						txt = xmlNodeListGetString( doc, isub->children, 1 );
						data->physics.dopplerfactor = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"friction" ) ) ) {
						txt = xmlGetProp( isub,  (const xmlChar *)"floorY" );
						data->physics.floorResY = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"floorXZ" );
						data->physics.floorResXZ = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"player" );
						data->physics.friction_player = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"restitution" ) ) ) {
						txt = xmlGetProp( isub,  (const xmlChar *)"playerBall" );
						data->physics.rest_playerBall = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"ballFloor" );
						data->physics.resti_coef = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"air" ) ) ) {
						txt = xmlGetProp( isub,  (const xmlChar *)"viscosity" );
						data->physics.air_viscosity = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"density" );
						data->physics.air_density = atof((const char*)txt);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"wind" ) ) ) {
						txt = xmlGetProp( isub,  (const xmlChar *)"random" );
						data->physics.rand_wind = !strcmp((const char*)txt,"true");
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"dir" );

						sscanf((const char*)txt, "%lf,%lf,%lf", &data->physics.wind_dir[0],
									  &data->physics.wind_dir[1],
									  &data->physics.wind_dir[2]);
					}

					isub = isub->next;
				}

			}

            if( (!xmlStrcmp(sub->name, (const xmlChar *)"controls" ) ) ) {

				txt = xmlGetProp( sub,  (const xmlChar *)"usejoystick" );
				data->usejoystick = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"joypolling" );
				data->pool = atoi((const char*)txt);
				data->usepooling = (data->pool != 0);
				xmlFree( txt );

                txt = xmlGetProp( sub,  (const xmlChar *)"joyx" );
				sscanf((const char*)txt, "%d,%d", &data->axis[0],
									     	      &data->axis[0+3]);
				xmlFree( txt );

                txt = xmlGetProp( sub,  (const xmlChar *)"joyy" );
				sscanf((const char*)txt, "%d,%d", &data->axis[1],
									     	      &data->axis[1+3]);
				xmlFree( txt );

                txt = xmlGetProp( sub,  (const xmlChar *)"joyz" );
				sscanf((const char*)txt, "%d,%d", &data->axis[2],
									     	      &data->axis[2+3]);
				xmlFree( txt );

				isub = sub->children;

				while( isub != NULL ) {

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"player1" ) ) ) {

						txt = xmlGetProp( isub,  (const xmlChar *)"up" );
						data->p1.fwd = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"down" );
						data->p1.bwd = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"left" );
						data->p1.left = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"right" );
						data->p1.right = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"kick1" );
						data->p1.kickXZ = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"kick2" );
						data->p1.kickY = keyValue((const char*)txt, data->usejoystick);
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"player2" ) ) ) {

						txt = xmlGetProp( isub,  (const xmlChar *)"up" );
						data->p2.fwd = keyValue((const char*)txt,false,2);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"down" );
						data->p2.bwd = keyValue((const char*)txt,false,2);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"left" );
						data->p2.left = keyValue((const char*)txt,false,2);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"right" );
						data->p2.right = keyValue((const char*)txt,false,2);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"kick1" );
						data->p2.kickXZ = keyValue((const char*)txt,false,2);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"kick2" );
						data->p2.kickY = keyValue((const char*)txt,false,2);
						xmlFree( txt );
					}

					isub = isub->next;
				}

			}

			if( (!xmlStrcmp(sub->name, (const xmlChar *)"graphics" ) ) ) {

				txt = xmlGetProp( sub,  (const xmlChar *)"motionfactor" );
				data->graphics.motionfactor = atof((const char*)txt);
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"motionframes" );
				data->graphics.motionframes = atoi((const char*)txt);
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"fadesteps" );
				data->graphics.fadesteps = atoi((const char*)txt);
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"fadetimer" );
				data->graphics.fadetimer = atoi((const char*)txt);
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"enablemotion" );
				data->graphics.enablemotion = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"useshader" );
				data->graphics.useshader = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"vextexshader" );
				data->graphics.vextexshader = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"fragshader" );
				data->graphics.fragshader = (const char*)txt;
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"usecubemap" );
				data->graphics.usecubemap = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"fullscreen" );
				data->graphics.fullscreen = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"usegamemode" );
				data->graphics.usegamemode = !strcmp((const char*)txt,"true");
				xmlFree( txt );

				txt = xmlGetProp( sub,  (const xmlChar *)"gamemode" );
				data->graphics.gamemode = (const char*)txt;
				xmlFree( txt );

				isub = sub->children;

				while( isub != NULL ) {

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"fog" ) ) ) {
						txt = xmlGetProp( isub,  (const xmlChar *)"density" );
						data->graphics.fog.density = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"start" );
						data->graphics.fog.start = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"end" );
						data->graphics.fog.end = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"end" );
						data->graphics.fog.end = atof((const char*)txt);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"mode" );
						data->graphics.fog.mode = atoi((const char*)txt);

						switch( data->graphics.fog.mode ) {
							case 0: data->graphics.fog.fog_filter = GL_LINEAR; break;
							case 1: data->graphics.fog.fog_filter = GL_EXP; break;
							case 2: data->graphics.fog.fog_filter = GL_EXP2; break;
						}
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"enable" );
						data->graphics.useFog = !strcmp((const char*)txt,"true");
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"color" );

						sscanf((const char*)txt, "%f,%f,%f", &data->graphics.fog.color[0],
									     	     &data->graphics.fog.color[1],
									             &data->graphics.fog.color[2]);
						data->graphics.fog.color[3] = 1.0f;
						xmlFree( txt );
					}

					if( (!xmlStrcmp(isub->name, (const xmlChar *)"lights" ) ) ) {

						txt = xmlGetProp( isub,  (const xmlChar *)"nlights" );
						data->graphics.nlights = min(atoi((const char*)txt),7);
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"draw" );
						data->graphics.draw_light = !strcmp((const char*)txt,"true");
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"enable" );
						data->graphics.use_light = !strcmp((const char*)txt,"true");
						xmlFree( txt );

						txt = xmlGetProp( isub,  (const xmlChar *)"ambientcolor" );
						sscanf((const char*)txt, "%lf,%lf,%lf,%lf", &data->graphics.ambient_light[0],
									     	     &data->graphics.ambient_light[1],
									             &data->graphics.ambient_light[2],
										     &data->graphics.ambient_light[3]);
						xmlFree( txt );

						data->graphics.lights = new SLightData[data->graphics.nlights];

						iisub = isub->children;

						register int i = 0;
						while( iisub != NULL && i < data->graphics.nlights) {

							if( (!xmlStrcmp(iisub->name, (const xmlChar *)"light" ) ) ) {

								txt = xmlGetProp( iisub,  (const xmlChar *)"type" );
								data->graphics.lights[i].type = (const char*)txt;
								xmlFree( txt );

								txt = xmlGetProp( iisub,  (const xmlChar *)"ambient" );
								sscanf((const char*)txt, "%lf,%lf,%lf",
									&data->graphics.lights[i].ambient[0],
									&data->graphics.lights[i].ambient[1],
									&data->graphics.lights[i].ambient[2]);
								data->graphics.lights[i].ambient[3] = 1.0f;
								xmlFree( txt );

								txt = xmlGetProp( iisub,  (const xmlChar *)"diffuse" );

								sscanf((const char*)txt, "%lf,%lf,%lf",
									&data->graphics.lights[i].diffuse[0],
									&data->graphics.lights[i].diffuse[1],
									&data->graphics.lights[i].diffuse[2]);
								data->graphics.lights[i].diffuse[3] = 1.0f;

// fprintf(stderr,"%s\n",(const char*)txt);
// fprintf(stderr,"%lf,%lf,%lf\n", data->graphics.lights[i].diffuse[0],data->graphics.lights[i].diffuse[1],data->graphics.lights[i].diffuse[2]);

								xmlFree( txt );

								txt = xmlGetProp( iisub,  (const xmlChar *)"specular" );
								sscanf((const char*)txt, "%lf,%lf,%lf",
									&data->graphics.lights[i].specular[0],
									&data->graphics.lights[i].specular[1],
									&data->graphics.lights[i].specular[2]);
								data->graphics.lights[i].ambient[3] = 1.0f;
								xmlFree( txt );

								txt = xmlGetProp( iisub,  (const xmlChar *)"position" );
								sscanf((const char*)txt, "%f,%f,%f",
									&data->graphics.lights[i].position[0],
									&data->graphics.lights[i].position[1],
									&data->graphics.lights[i].position[2]);
								xmlFree( txt );

								txt = xmlGetProp( iisub,  (const xmlChar *)"atenuation" );
								sscanf((const char*)txt, "%lf,%lf,%lf",
									&data->graphics.lights[i].atenuation[0],
									&data->graphics.lights[i].atenuation[1],
									&data->graphics.lights[i].atenuation[2]);
								xmlFree( txt );

								if( data->graphics.lights[i].type == "spot" ) {
									txt = xmlGetProp(iisub,(const xmlChar *)"direction" );
									sscanf((const char*)txt, "%f,%f,%f",
										&data->graphics.lights[i].direction[0],
										&data->graphics.lights[i].direction[1],
										&data->graphics.lights[i].direction[2]);
									xmlFree( txt );

									txt = xmlGetProp( iisub,  (const xmlChar *)"cutoff" );
									data->graphics.lights[i].cutoff =
												atof((const char*)txt);
									xmlFree( txt );

									txt = xmlGetProp(iisub,(const xmlChar*)"exponent" );
									data->graphics.lights[i].exponent =
												atof((const char*)txt);
									xmlFree( txt );
								}

								i++;
							}

							iisub = iisub->next;
						}
						data->graphics.nlights = min( data->graphics.nlights, i);
					}
					isub = isub->next;
				}
			}
			sub = sub->next;
		}

    		xmlFreeDoc( doc );

		if( !(data->team1 = readTeamXml(data->team1xml.c_str())) ) {
			delete [] data->graphics.lights;
			return NULL;
		}

		if( !(data->team2 = readTeamXml(data->team2xml.c_str())) ) {
			deleteTeamData(data->team2);
			delete [] data->graphics.lights;
			return NULL;
		}

		return data;
	}

	STeamData *readTeamXml( const char *filename )
	{
    		xmlChar *txt;
    		xmlDocPtr doc;
    		xmlNodePtr cur, sub, isub;

		STeamData *data;

		if( !(doc = xmlParseFile( filename ) ) ){
        		fprintf( stderr, "[ERRO]: %s %s.\n", Language[LG_XML_ERROR], filename );
        		xmlFreeDoc( doc );
        		return( NULL );
    		}

		if( !(cur = xmlDocGetRootElement( doc ) ) ){
			//fprintf( stderr, "[ERRO]: O arquivo %s não possui elemento raiz.\n", filename );
			fprintf( stderr, "[ERRO]: %s %s.\n", Language[LG_XML_ROOT_ERROR], filename );
			xmlFreeDoc( doc );
			return( NULL );
		}

		if( xmlStrcmp( cur->name, (const xmlChar *) "robotics" ) ){
			//fprintf( stderr, "[ERRO]: Arquivo de configuração invalido.\n" );
			fprintf( stderr, "[ERRO]: %s.\n", Language[LG_XML_CONF_ERROR] );
			xmlFreeDoc( doc );
			return( NULL );
		}

		//Version Checker
		if( xmlStrcmp( xmlGetProp( cur, (const xmlChar *)"version" ), (const xmlChar *) "0.2.1" ) ){
			//fprintf( stderr, "[ERRO]: Arquivo de configuração com versao invalida.\n" );
			fprintf( stderr, "[ERRO]: %s.\n", Language[LG_XML_VER_ERROR] );
			xmlFreeDoc( doc );
			return( NULL );
		}

		data = new STeamData;

		sub = cur->children;
		/*fprintf( stderr, "%s\n", (const char *)cur->name );
		fprintf( stderr, "%s\n", (const char *)cur->children->name );

		txt = xmlNodeListGetString(doc, cur->children->xmlChildrenNode, 1);
		fprintf( stderr,"keyword: %s\n", txt);
		xmlFree(txt);*/

		if( (!xmlStrcmp(sub->name, (const xmlChar *)"team" ) ) ){
			// Nó time

			txt = xmlGetProp( sub,  (const xmlChar *)"name" );
			data->name = (const char*)txt;
			xmlFree( txt );

			txt = xmlGetProp( sub,  (const xmlChar *)"sigla" );
			data->sigla = (const char*)txt;
			xmlFree( txt );

			txt = xmlGetProp( sub,  (const xmlChar *)"nplayers" );
			data->nplayers = atoi((const char*)txt);
			xmlFree( txt );

			data->players = new SPlayerData[data->nplayers];

			sub = sub->children;

			while( sub != NULL ){

				if( (!xmlStrcmp(sub->name, (const xmlChar *)"images" ) ) ){
					txt = xmlGetProp( sub,  (const xmlChar *)"basepath" );
					data->basepath = (const char*)txt;
					xmlFree( txt );

					txt = xmlGetProp( sub,  (const xmlChar *)"imbrasao" );
					data->imbrasao = data->basepath + (const char*)txt;
					xmlFree( txt );

					txt = xmlGetProp( sub,  (const xmlChar *)"imcamisa" );
					data->imcamisa = data->basepath + (const char*)txt;
					xmlFree( txt );
				}

				if( (!xmlStrcmp(sub->name, (const xmlChar *)"coach" ) ) ){
					txt = xmlGetProp( sub,  (const xmlChar *)"name" );
					data->coach = (const char*)txt;
					xmlFree( txt );
				}

				if( (!xmlStrcmp(sub->name, (const xmlChar *)"players" ) ) ){

					txt = xmlGetProp( sub,  (const xmlChar *)"ia" );
					data->ia = (const char*)txt;
					xmlFree( txt );

					txt = xmlGetProp( sub,  (const xmlChar *)"stupid" );
					data->stupid = !strcmp((const char*)txt,"true");
					xmlFree( txt );

					isub = sub->children;
					register int i = 0;
					while( isub != NULL && i < data->nplayers ){
						if( (!xmlStrcmp(isub->name, (const xmlChar *)"player" ) ) ){

							txt = xmlGetProp( isub,  (const xmlChar *)"name" );
							data->players[i].name = (const char*)txt;
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"p0x" );
							data->players[i].p0x = atof((const char*)txt);
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"p0z" );
							data->players[i].p0z = atof((const char*)txt);
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"pfx" );
							data->players[i].pfx = atof((const char*)txt);
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"pfz" );
							data->players[i].pfz = atof((const char*)txt);
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"type" );
							data->players[i].type = (const char*)txt;
							xmlFree( txt );

							txt = xmlGetProp( isub,  (const xmlChar *)"number" );
							data->players[i].number = atoi((const char*)txt);
							xmlFree( txt );

							i++;

						}
						isub = isub->next;
					}

				}
				sub = sub->next;
			}
		}

    		xmlFreeDoc( doc );

		return data;
	};
};
