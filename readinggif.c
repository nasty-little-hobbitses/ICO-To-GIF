// Header Files
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// Function prototypes
int nth_bit(int,int);		// Returns the nth bit of a number in binary form
int three_bit(int,int);		// Returns decimal form of 3 bits starting with lsb as nth bit of a number Eg: 111 is returned as 7

// Defining 1 Byte and 2 Bytes of information
typedef unsigned char BYTE;		// 1 Byte of data
typedef unsigned short WORD;	// 2 Byte of Data

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


int main()
{
	int i;								// i = loop variable
	FILE *fp,*fr;						// fp = file pointer for reading, fr = file pointer for writing
	fp = fopen("down.gif","rb");		// source destination
	fr = fopen("test2.gif","wb");		// target destination

	if(fp == NULL || fr == NULL)		// to check for any errors in opening files
	{
		printf("Nope");
		exit(1);
	}

	// initialising the GIF Header
	GIFHEAD *header;
	header = (GIFHEAD*)malloc(sizeof(GIFHEAD));

	// reading and writing elements present in the header
	fread(&(header->Signature),sizeof(BYTE),3,fp);
	printf("\nSignature = %s",header->Signature);
	fwrite(&(header->Signature),sizeof(BYTE),3,fr);

	fread(&(header->Version),sizeof(BYTE),3,fp);
	printf("\nVersion = %s",header->Version);
	fwrite(&(header->Version),sizeof(BYTE),3,fr);

	fread(&(header->ScreenWidth),sizeof(WORD),1,fp);
	printf("\nScreen Width = %d",header->ScreenWidth);
	fwrite(&(header->ScreenWidth),sizeof(WORD),1,fr);

	fread(&(header->ScreenHeight),sizeof(WORD),1,fp);
	printf("\nScreen Height = %d",header->ScreenHeight);
	fwrite(&(header->ScreenHeight),sizeof(WORD),1,fr);

	fread(&(header->Packed),sizeof(BYTE),1,fp);
	printf("\nPacked = %d",header->Packed);
	fwrite(&(header->Packed),sizeof(BYTE),1,fr);

	// to calculate size of global color table
	int SizeOfGCT = three_bit(header->Packed,0);
	int NoOfGCTEnt = (1L << (SizeOfGCT + 1));

	// information broken down from "header->Packed"
	printf("\nGlobal Color Table Flag = %d",nth_bit(header->Packed,7));									// indicates the presence of Global Color Table
	printf("\nColor Resolution = %d",three_bit(header->Packed,4));										// used for indicating the bits per color assigned to a pixel
	printf("\nGlobal Color Sort Flag = %d (it is '0' for version 87a)",nth_bit(header->Packed,3));		// indicates whether Global Color Table is to be sorted
	printf("\nNumber of Entries of Global Color Table = %d",NoOfGCTEnt);								// indicates the number of Global Color Entries
	printf("\nNo of Bits for Color Palette = %d",three_bit(header->Packed,4) + 1);						// indicates the bit per colour assigned to a pixel

	fread(&(header->BackgroundColor),sizeof(BYTE),1,fp);
	printf("\nBackground Color = %d",header->BackgroundColor);
	fwrite(&(header->BackgroundColor),sizeof(BYTE),1,fr);

	fread(&(header->AspectRatio),sizeof(BYTE),1,fp);
	printf("\nAspect Ratio = %d",header->AspectRatio);
	fwrite(&(header->AspectRatio),sizeof(BYTE),1,fr);

	// Reads Global Color Table if present
	if(nth_bit(header->Packed,7) == 1)
	{
		// initialising Global Color Table Entries
		GIFCOLORTABLE glo[NoOfGCTEnt];

		// Reading each Entry of Global Color Table
		for(i = 0;i < NoOfGCTEnt;i++)
		{
			fread(&(glo[i].Red),sizeof(BYTE),1,fp);
			fread(&(glo[i].Green),sizeof(BYTE),1,fp);
			fread(&(glo[i].Blue),sizeof(BYTE),1,fp);
			printf("\n[%3d][%3d][%3d]%d",glo[i].Red,glo[i].Green,glo[i].Blue,i);
			fwrite(&(glo[i].Red),sizeof(BYTE),1,fr);
			fwrite(&(glo[i].Green),sizeof(BYTE),1,fr);
			fwrite(&(glo[i].Blue),sizeof(BYTE),1,fr);
		}
	}

	// Image section
	while(1)
	{
		// initialising Image Descriptor
		GIFIMGDESC *img;
		img = (GIFIMGDESC*)malloc(sizeof(GIFIMGDESC));

		fread(&(img->Separator),sizeof(BYTE),1,fp);
		printf("\nImage Separator = %d",img->Separator);	// indicates the presence of image or end of file
		fwrite(&(img->Separator),sizeof(BYTE),1,fr);

		// To check for end of file
		if(img->Separator == 59)
		{
			printf("\nEnd of File");
			free(img);
			free(header);
			fclose(fp);
			fclose(fr);
			break;
		}

		fread(&(img->Left),sizeof(WORD),1,fp);
		printf("\nImage Left Coordinate = %d",img->Left);
		fwrite(&(img->Left),sizeof(WORD),1,fr);

		fread(&(img->Top),sizeof(WORD),1,fp);
		printf("\nImage Top Coordinate = %d",img->Top);
		fwrite(&(img->Top),sizeof(WORD),1,fr);

		fread(&(img->Width),sizeof(WORD),1,fp);
		printf("\nImage Width in Pixels = %d",img->Width);
		fwrite(&(img->Width),sizeof(WORD),1,fr);

		fread(&(img->Height),sizeof(WORD),1,fp);
		printf("\nImage Height = %d",img->Height);
		fwrite(&(img->Height),sizeof(WORD),1,fr);

		fread(&(img->Packed),sizeof(BYTE),1,fp);
		printf("\nPacked = %d",img->Packed);
		fwrite(&(img->Packed),sizeof(BYTE),1,fr);

		// to calculate the size of Local Color Table
		int SizeOfLCT = three_bit(img->Packed,0);
		int NoOfLCTEnt = 1L << (SizeOfLCT + 1);

		// information broken down from "img->Packed"
		printf("\nImage Color Table Flag = %d",nth_bit(img->Packed,7));
		printf("\nInterlace Flag = %d",nth_bit(img->Packed,6));
		printf("\nSort Flag = %d",nth_bit(img->Packed,5));
		printf("\nReserved For Future Use = %d%d",nth_bit(img->Packed,5),nth_bit(img->Packed,4));
		printf("\nNumber of Entries of Local Colour Table = %d",NoOfLCTEnt);

		// Reads Local Color Table if present
		if(nth_bit(img->Packed,7) == 1)
		{
			GIFCOLORTABLE loc[NoOfLCTEnt];

			// Reading each Entry of Local Color Table
			for(i = 0;i < NoOfLCTEnt;i++)
			{
				fread(&(loc[i].Red),sizeof(BYTE),1,fp);
				fread(&(loc[i].Green),sizeof(BYTE),1,fp);
				fread(&(loc[i].Blue),sizeof(BYTE),1,fp);
				printf("\n[%3d][%3d][%3d]%d",loc[i].Red,loc[i].Green,loc[i].Blue,i);
				fwrite(&(loc[i].Red),sizeof(BYTE),1,fr);
				fwrite(&(loc[i].Green),sizeof(BYTE),1,fr);
				fwrite(&(loc[i].Blue),sizeof(BYTE),1,fr);
			}
		}

		// initialising Image Data
		IMGDATA *data;
		data = (IMGDATA*)malloc(sizeof(IMGDATA));

		fread(&data->Compression,sizeof(BYTE),1,fp);
		printf("\nCompression Factor = %x",data->Compression);
		fwrite(&data->Compression,sizeof(BYTE),1,fr);

		// Reading the image data
		do
		{
			fread(&data->Size,sizeof(BYTE),1,fp);
			printf("\nSize = %x\n",data->Size);				// indicates the size of sub-block containg pixel data
			fwrite(&data->Size,sizeof(BYTE),1,fr);
			for(i = 0;i < data->Size;i++)					// reading pixel data
			{
				fread(&data->Image,sizeof(BYTE),1,fp);
				printf("%x ",data->Image);
				fwrite(&data->Image,sizeof(BYTE),1,fr);
			}
		}while(data->Size != 0);							// image data block ends when size = 0
		free(img);
	}


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
