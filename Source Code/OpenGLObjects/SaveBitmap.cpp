// SaveBitmap.cpp - Output a bitmap to a .BMP file.
//

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <time.h> 

#include <shellapi.h>
#include <commdlg.h>

#define BITSPERPIXEL		32

void	SaveBitmap( const char *szFilename, HBITMAP hBitmap )
{
	HDC					hdc=NULL;
	FILE*				fp=NULL;
	LPVOID				pBuf=NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader;

	do{

		hdc=GetDC(NULL);
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

		if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBox(NULL,_T("Unable to Allocate Bitmap Memory"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}
		
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf,&bmpInfo,DIB_RGB_COLORS);	

		if((fp=fopen(szFilename,"wb"))==NULL)
		{
			MessageBox(NULL,_T("Unable to Create Bitmap File"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}

		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);

	}while(false);

		if(hdc)
			ReleaseDC(NULL,hdc);

		if(pBuf)
			free(pBuf);

		if(fp)
			fclose(fp);
}