#include "jpeg.h"
#include "common.h"

// Decodifica uma imagem JPG e armazena-a em uma estrutura TEX.
void JPGDecode(jpeg_decompress_struct* cinfo, TEX *pImageData, bool invert)
{
	// L�o cabe�lho de um arquivo jpeg
	jpeg_read_header(cinfo, TRUE);

	// Come� a descompactar um arquivo jpeg com a informa�o
	// obtida do cabe�lho
	jpeg_start_decompress(cinfo);

	// Pega as dimens�s da imagem e varre as linhas para ler os dados do pixel
	pImageData->ncomp = cinfo->num_components;
	pImageData->dimx  = cinfo->image_width;
	pImageData->dimy  = cinfo->image_height;

	int rowSpan = pImageData->ncomp * pImageData->dimx;
	// Aloca mem�ia para o buffer do pixel
	pImageData->data = new unsigned char[rowSpan * pImageData->dimy];

	// Aqui se usa a vari�el de estado da biblioteca cinfo.output_scanline
	// como o contador de loop

	// Cria um array de apontadores para linhas
	int height = pImageData->dimy - 1;
	unsigned char** rowPtr = new unsigned char*[pImageData->dimy];
	if(invert)
		for (int i = 0; i <= height; i++)
			rowPtr[height - i] = &(pImageData->data[i*rowSpan]);
	else
		for (int i = 0; i <= height; i++)
			rowPtr[i] = &(pImageData->data[i*rowSpan]);

	// Aqui se extrai todos os dados de todos os pixels
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height)
	{
		// L�a linha corrente de pixels e incrementa o contador de linhas lidas
		rowsRead += jpeg_read_scanlines(cinfo, &rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}

	// Libera a mem�ia
	delete [] rowPtr;

	// Termina a decompacta�o dos dados
	jpeg_finish_decompress(cinfo);
}

// Carrega o arquivo JPG e retorna seus dados em uma estrutura tImageJPG.
TEX *LoadJPG(const char *filename, bool invert)
{
	struct jpeg_decompress_struct cinfo;
	TEX *pImageData = NULL;
	FILE *pFile;

	// Abre um arquivo JPG e verifica se n� ocorreu algum problema na abertura
	if((pFile = fopen(filename, "rb")) == NULL)
	{
		// Exibe uma mensagem de erro avisando que o arquivo n� foi encontrado
		// e retorna NULL
		//fprintf(stderr,"[ERRO]: Falha ao carregar o arquivo JPG: %s\n",filename);
		fprintf(stderr,"[ERRO]: %s: %s\n",Language[LG_JPEG_READ_ERROR],filename);
		return NULL;
	}

	// Cria um gerenciado de erro
	jpeg_error_mgr jerr;

	// Objeto com informa�es de compacta�o para o endere� do gerenciador de erro
	cinfo.err = jpeg_std_error(&jerr);

	// Inicializa o objeto de decompacta�o
	jpeg_create_decompress(&cinfo);

	// Especifica a origem dos dados (apontador para o arquivo)
	jpeg_stdio_src(&cinfo, pFile);

	// Aloca a estrutura que conter�os dados jpeg
	//pImageData = (TEX*)malloc(sizeof(TEX));
	pImageData = new TEX;

	// Decodifica o arquivo JPG e preenche a estrutura de dados da imagem
	JPGDecode(&cinfo, pImageData, invert);

	// Libera a mem�ia alocada para leitura e decodifica�o do arquivo JPG
	jpeg_destroy_decompress(&cinfo);

	// Fecha o arquivo
	fclose(pFile);

	// Retorna os dados JPG (esta mem�ia deve ser liberada depois de usada)
	return pImageData;
}

void saveJPG( char *arquivo, GLubyte *image_buffer, int w, int h, int quality, bool invert )
{

	struct jpeg_compress_struct cinfo;

	struct jpeg_error_mgr jerr;

	FILE * outfile;

	int row_stride;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(arquivo, "wb")) == NULL) {
		//fprintf(stderr,"[ERRO]: Falha ao salvar o arquivo JPG: %s\n",arquivo);
		fprintf(stderr,"[ERRO]: %s: %s\n",Language[LG_JPEG_WRITE_ERROR],arquivo);
		exit(1);
	}

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = w;
	cinfo.image_height = h;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, quality, TRUE );

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = w * 3;

	int height = h - 1;
	unsigned char** rowPtr = new unsigned char*[h];

	if(invert)
		for (int i = 0; i <= height; i++)
			rowPtr[height - i] = &(image_buffer[i*row_stride]);
	else
		for (int i = 0; i <= height; i++)
			rowPtr[i] = &(image_buffer[i*row_stride]);

	int rowsWrite = 0;
	while (cinfo.next_scanline < cinfo.image_height)
	{
		rowsWrite += jpeg_write_scanlines(&cinfo, &rowPtr[rowsWrite], cinfo.image_height - rowsWrite);
	}

	jpeg_finish_compress(&cinfo);

	fclose(outfile);

	jpeg_destroy_compress(&cinfo);

	delete [] rowPtr;
}

void deleteJPG( TEX *p )
{
	if ( p->data )
		delete [] p->data;
	delete p;
}

