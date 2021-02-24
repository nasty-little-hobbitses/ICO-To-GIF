#include<stdio.h>
#include<stdlib.h>


int main() {

  typedef unsigned char BYTE;
  typedef unsigned long DWORD;
  typedef unsigned short WORD;
  typedef long LONG;

    #pragma pack(1)

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








FILE *fp;
ICONDIR* x;
ICONDIRENTRY* y;
BITMAPINFOHEADER* z;



fp = fopen("1.ico","rb");
    if (fp == NULL)
        printf("mcph\n" );

printf("File Pointer: %ld\n",ftell(fp) );

x = (ICONDIR*)malloc( sizeof( ICONDIR ) );

fread(&(x->idReserved), sizeof(WORD),1,fp);
printf("Reserved : %d\n", x->idReserved);


fread(&(x->idType), sizeof(WORD),1,fp);
printf("Id Type: %d\n", x->idType);


fread(&(x->idCount), sizeof(WORD),1,fp);
printf("Number of Images : %d\n", x->idCount);

y = (ICONDIRENTRY*)malloc( sizeof( ICONDIRENTRY) );




fread(y,sizeof(ICONDIRENTRY),1,fp);
printf("Width = %d Height = %d Bits per pixel = %d colour count = %d\n", y->bWidth,y->bHeight,y->wBitCount, y->bColorCount);

printf("Size of image resource : %d\n",  y->dwBytesInRes);
printf("Offset to the image : %d\n",  y->dwImageOffset);
fseek( fp, y->dwImageOffset , SEEK_SET );
z = (BITMAPINFOHEADER*)malloc( sizeof( BITMAPINFOHEADER) );
fread(z,sizeof(BITMAPINFOHEADER),1,fp);
printf("Bits per pixel = %d Width = %d combined height of both masks = %d\n",z->biBitCount,z->biWidth,z->biHeight);
printf("File pointer = %ld\n", ftell(fp));
printf("Colour Table:\n");
printf("I B G R Res.\n");

int dim = y->bHeight*y->bWidth;
RGBQUAD arr[256];
int i = 0;
while(i < 256){
  fread(&arr[i], sizeof(RGBQUAD),1,fp);
  printf("%d %d %d %d %d\n",i, arr[i].rgbBlue,arr[i].rgbGreen,arr[i].rgbRed,arr[i].rgbReserved);
  i++;
}

printf("File pointer = %ld\n", ftell(fp));
printf("Pixel Data\n");

int j = 0;
BYTE bits[dim];
while( j < dim){
  fread(&bits[j],1,1,fp);
  printf("%d,", bits[j]);
  j++;
}
printf("\n");

free(x);
free(y);
free(z);

return 0;

}
