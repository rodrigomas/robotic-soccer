#ifndef TRANS_H
#define TRANS_H

#include <map>
#include <stdio.h>
#include <string>
#include <string.h>

using std::string;

namespace soccer {

    typedef std::map< int, string, std::less<int> > mis;

    class CTranslator {

        mis Table;

      public:

        CTranslator(){};

        CTranslator(const char *filename)
        {
                LoadFile(filename);
        };

        ~CTranslator()
        {
        };

        const char* operator[]( int key )
        {
            return Table[key].c_str();
        };

        void LoadFile( const char *filename )
        {
            FILE *fp;

            fp = fopen(filename,"r");

            int key;
            char buffer[256], data[256];

            if( !fp )
                return;

            while( !feof(fp) ) {

                fgets(buffer,sizeof(buffer),fp);

                if( buffer[0] == '#' )
                    continue;

                sscanf(buffer,"%d=%[^\n]\n", &key, data);

                //fprintf(stderr,"%d=%s\n", key, data );
                Table.insert( mis::value_type( key, data ));

            }
        };

        void ReloadFile( const char *filename )
        {
            FILE *fp;


            if ( !( fp = fopen(filename,"r") ) )
                return;

            fclose(fp);
            //clear
            Table.clear();

            LoadFile(filename);
        };

    };
};

#endif

