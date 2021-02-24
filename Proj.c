#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"ourheader.h"

/*--------------------------------------------------------------------------------------Defining Data Types---------------------------------------------------------------------------------------------------*/

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef long LONG;

/*----------------------------------------------------------------------------------------End of Data Types---------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------Ico Image Structures--------------------------------------------------------------------------------------------------*/

typedef struct tagICONDIRENTRY
{
       BYTE bWidth;
       BYTE bHeight;
       BYTE bColorCount;
       BYTE bReserved;
       WORD wPlanes;
       WORD wBitCount;
       unsigned int dwBytesInRes;
       unsigned int dwImageOffset;
}ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct tagICONDIR
{
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?

} ICONDIR, *LPICONDIR;

typedef struct tagBITMAPINFOHEADER
{
unsigned int biSize;  //specifies the number of bytes required by the struct
unsigned int biWidth;  //specifies width in pixels
unsigned int biHeight;  //species height in pixels
WORD biPlanes; //specifies the number of color planes, must be 1
WORD biBitCount; //specifies the number of bit per pixel
 int biCompression;//spcifies the type of compression
 int biSizeImage;  //size of image in bytes
double g1;
double g2; //number of colors that are important
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

typedef struct tagICONIMAGE {

  BITMAPINFOHEADER   icHeader;      // DIB header
   RGBQUAD         icColors[1];   // Color table
   BYTE            icXOR[1];      // DIB bits for XOR mask
   BYTE            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;



/*----------------------------------------------------------------------------------End of Ico Image Structures-----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------Gif Image Structures--------------------------------------------------------------------------------------------------*/

// Structure contains "Header" & "Logical Screen Descriptor"
typedef struct _GifHeader
{
  // Header
  BYTE Signature[3];     /* Header Signature (always "GIF") */
  BYTE Version[3];       /* GIF format version("87a" or "89a") */
  // Logical Screen Descriptor
  WORD ScreenWidth;      /* Width of Display Screen in Pixels */
  WORD ScreenHeight;     /* Height of Display Screen in Pixels */
  BYTE Packed;           /* Screen and Color Map Information */
  BYTE BackgroundColor;  /* Background Color Index */
  BYTE AspectRatio;      /* Pixel Aspect Ratio */
} GIFHEAD;

// Backbone Structure for colour entries of both Global and Local Colour Table
typedef struct _GifColorTable
{
	BYTE Red;          /* Red Color Element       */
	BYTE Green;        /* Green Color Element     */
	BYTE Blue;         /* Blue Color Element      */
} GIFCOLORTABLE;

// Structure for Image Descriptor which contains properties of each the respective image
typedef struct _GifImageDescriptor
{
	BYTE Separator;    /* Image Descriptor identifier */
	WORD Left;         /* X position of image on the display */
	WORD Top;          /* Y position of image on the display */
	WORD Width;        /* Width of the image in pixels */
	WORD Height;       /* Height of the image in pixels */
	BYTE Packed;       /* Image and Color Table Data Information */
} GIFIMGDESC;

// Structure for the Image Data
typedef struct _ImageData
{
	BYTE Compression;
	BYTE Size;
	BYTE Image;
} IMGDATA;

/*----------------------------------------------------------------------------------End of Gif Image Structures-----------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	int i;
	FILE *fr,*fw;                   // fr = source file, fw = dest file
	fr = fopen(argv[1],"rb");
	fw = fopen(argv[2],"wb");
	if(fr == NULL || fw == NULL)
	{
		printf("Fail");
		exit(1);
	}

	ICONDIR* x;
	x = (ICONDIR*)malloc( sizeof( ICONDIR ) );

	ICONDIRENTRY* y;
	y = (ICONDIRENTRY*)malloc( sizeof( ICONDIRENTRY) );

	BITMAPINFOHEADER* z;
	z = (BITMAPINFOHEADER*)malloc( sizeof( BITMAPINFOHEADER) );

	GIFHEAD *header;
	header = (GIFHEAD*)malloc(sizeof(GIFHEAD));

	fread(&(x->idReserved), sizeof(WORD),1,fr);     // reading from ico

	fread(&(x->idType), sizeof(WORD),1,fr);         // reading from ico

	fread(&(x->idCount), sizeof(WORD),1,fr);        // reading from ico


	fread(y,sizeof(ICONDIRENTRY),1,fr);             // reading from ico

	fseek( fr, y->dwImageOffset , SEEK_SET );

	fread(z,sizeof(BITMAPINFOHEADER),1,fr);         // reading from ico

        // GIF Signature ('GIF')
	header->Signature[0] = 71;
	header->Signature[1] = 73;
	header->Signature[2] = 70;
	fwrite(&(header->Signature),sizeof(BYTE),3,fw);

        // GIF Version ('87a')
	header->Version[0] = 56;
	header->Version[1] = 55;
	header->Version[2] = 97;
	fwrite(&(header->Version),sizeof(BYTE),3,fw);

	// reading img width and writing to GIF
        header->ScreenWidth = y->bWidth;
	fwrite(&(header->ScreenWidth),sizeof(WORD),1,fw);

        // reading img height and writing to GIF
 	header->ScreenHeight = y->bHeight;
	fwrite(&(header->ScreenHeight),sizeof(WORD),1,fw);

	header->Packed = 247;
	fwrite(&(header->Packed),sizeof(BYTE),1,fw);

	// to calculate size of global color table
	int SizeOfGCT = three_bit(header->Packed,0);
	int NoOfGCTEnt = (1L << (SizeOfGCT + 1));

	// Background Color = 0
	header->BackgroundColor = 0;
	fwrite(&(header->BackgroundColor),sizeof(BYTE),1,fw);

	// Aspect Ratio = 0
	header->AspectRatio = 0;
	fwrite(&(header->AspectRatio),sizeof(BYTE),1,fw);

	// Global Colour Table reading and writing till index no 127
	RGBQUAD arr[256];
	for(i = 0;i < 256;i++)
	{
		fread(&arr[i], sizeof(RGBQUAD),1,fr);


		        fwrite(&(arr[i].rgbRed),sizeof(BYTE),1,fw);
		        fwrite(&(arr[i].rgbGreen),sizeof(BYTE),1,fw);
		        fwrite(&(arr[i].rgbBlue),sizeof(BYTE),1,fw);

	}

	// To accomodate for padding, height and width need to be inc accordingly (multiple of 4)

	if(y->bWidth % 4 != 0){
        	y->bWidth += 4 - y->bWidth%4;
        }
	BYTE bits[y->bHeight][y->bWidth];

	// reading image data from ICO from last row
	for(int i = y->bHeight - 1;i >= 0;i--)
        {
                for(int j = 0;j < y->bWidth;j++)
                {
	                fread(&bits[i][j],1,1,fr);
                        if (bits[i][j] > 127)
                                bits[i][j] = 32;
                }
	}

	// Local Image Descriptor
	GIFIMGDESC *img;
        img = (GIFIMGDESC*)malloc(sizeof(GIFIMGDESC));
        img->Separator = 44;
        img->Left = 0;
        img->Top = 0;
        img->Width = y->bWidth;
        img->Height = y->bHeight;

	fwrite(&(img->Separator),sizeof(BYTE),1,fw);    // indicates the presence of image or end of file
	fwrite(&(img->Left),sizeof(WORD),1,fw);		// indicates the starting x coordinate
	fwrite(&(img->Top),sizeof(WORD),1,fw);		// indicates the starting y coordinate
	fwrite(&(img->Width),sizeof(WORD),1,fw);	// width of the image
	fwrite(&(img->Height),sizeof(WORD),1,fw);	// height of the image
	img->Packed = 0;				// packed = 0 as local colour table = 0
	fwrite(&(img->Packed),sizeof(BYTE),1,fw);

        BYTE Comp = 7;			// Compression factor = 7
        fwrite(&Comp,1,1,fw);

        BYTE sBlkSiz = y->bWidth + 1;	// Image sub-block size
        BYTE sBlkSizEnd = 1;		// Last sub-block size
        BYTE zero = 0;
        BYTE clCode = 128;		// Clear Code
        BYTE EOICode = 129;		// End of Info

	// writing image data
        for (int i = 0; i < y->bHeight; i++ )
        {
                fwrite(&sBlkSiz,1,1,fw);
                fwrite(&clCode,1,1,fw);
                for(int j = 0;j<y->bWidth;j++)
                        fwrite(&(bits[i][j]),sizeof(BYTE),1,fw);
        }

	// Last sub-block
        fwrite(&sBlkSizEnd,1,1,fw);
        fwrite(&EOICode,1,1,fw);
        fwrite(&zero,1,1,fw);
        BYTE ter = 59;
        fwrite(&ter,1,1,fw);

	// freeing memory locations
	free(x);
	free(y);
	free(z);
	free(header);
	fclose(fr);
	fclose(fw);


	return 0;
}

// Returns the nth bit of number in binary form
int nth_bit(int a,int n)
{
	return (a>>n & 1);
}

// Returns decimal form of 3 bits starting with lsb as nth bit Eg: 111 is returned as 7
int three_bit(int a,int n)
{
	int num = 0,i;
	for(i=n;i<n+3;i++)
	{
		num += pow(2,i-n) * (a>>i & 1);
	}
	return num;
}
