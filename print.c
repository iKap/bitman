//#ifdef _WIN32
//#include <windows.h>
//#endif
#include <stdio.h>
#include <string.h>
#include "print.h"
#include "esc_colors.h"

int GetNonEmptyNibbleHighBit(__INT64 Val)
{
	int i,p = 63;
	__INT64 FMask;

	MY_NAME;

	for (i=0; i < 60; i+=4)
	{
		FMask = (__INT64)0xF << (60-i);
		if (Val & FMask)
		{
			return p;
		}
		p = 60-i-1;
	}
	return p;
}

void PrintUsage(char *name)
{
	MY_NAME;

	printf(COLOR(IYellow, "%s version %s\n"),APP_NAME, VERSION);
	printf(COLOR(Green, "(c) by Igor Kaplinsky (igor.kaplinsky@gmail.com)\n"));
	printf("USAGE:" COLOR(BIWhite, "%s")" [0xHHHHHHHH | DDDDDDDD] {:XX | XX:XX ....}\n", name);
	printf(COLOR(BIBlue,"Example:\n"));
	printf(COLOR(BIWhite," %s")" 0x0102030405060708 :40 43:32 :10\n\n", name);
	printf(COLOR(BIWhite," %s")" 72623859790382856 :40 43:32 :10\n\n", name);
	printf(COLOR(BIWhite," %s")" 0x76534987 03:08'FIELD_A'\n", name);
	printf(COLOR(BIWhite," %s")" 0x76534987 03:08'FIELD_A' 07:15'FIELD_B'=0xA5\n\n", name);
}

// Print bit string 
void PrintoutValue(__INT64 Value)
{
	int i, NibbleHighBit;  
	int Zeros = 0, Ones = 0;
	char BitStr[100],BitHexStr[100],BitNum[100],ULineStr[100],strTmp[10];
	__INT64 FMask,Mask;
	
	MY_NAME;

	NibbleHighBit = GetNonEmptyNibbleHighBit(Value);
	memset(BitStr,0,sizeof(BitStr));
	memset(BitHexStr,0,sizeof(BitHexStr));
	memset(BitNum,0,sizeof(BitNum));
	memset(ULineStr,0,sizeof(ULineStr));

	for (i=63-NibbleHighBit; i<64; i++)
	{
		Mask = ((__INT64)1 << (63-i));
		if (Value & Mask)
		{
			strcat(BitStr,"1");
			Ones +=1;
		}
		else
		{
			strcat(BitStr,"0");
			Zeros +=1;
		}

		if ((i % 4) == 3) // Nibble hex value
		{
			strcat(BitStr,    " ");
			FMask = (__INT64)0xF << (63-i);
			//Four bits Hex value
			memset(strTmp,0,sizeof(strTmp));
			sprintf(strTmp,"   %X ", (unsigned int) ( (Value & FMask) >> (63-i) ) & 0xF );
			strcat(BitHexStr,strTmp);
			// Underline forming
			strcat(ULineStr,"---- ");
			//Number of bits
			memset(strTmp,0,sizeof(strTmp));
			sprintf(strTmp,"  %2d ", (63-i) );
			strcat(BitNum,strTmp);
		}
	}//for 0 to 64
	printf("\n #| %s|\n",BitNum);
	printf("  | %s| \n",ULineStr);
	printf("%2d| %s|0\n",NibbleHighBit,BitStr);
	printf("  | %s| \n",ULineStr);
	printf(" =| %s|\n",BitHexStr);
	printf("\n Zeros:(%d), Ones:(%d): ", Zeros, Ones);

	// Number of setup bits
	for (i=0; i<64; i++)
	{
		Mask = ((__INT64)1 << (63-i));
		if (Value & Mask)
			printf(":%02d ",63-i);

	}
	printf("\n\n");

}//PrintoutValue()

int PrintoutHeader(PLISTCNTRBLOCK pLCB)
{
	char HeaderStr[100];
	int  HeaderPos = 1;

	memset(HeaderStr,'-',sizeof(HeaderStr));
	/* Range */
	strncpy(HeaderStr+HeaderPos,"Range",5);
	HeaderPos += 5; /* Length of "Range" */
	HeaderPos += 3;
	/* Name */
	if (pLCB->MaxNameLength > 4 /* "Name" */)
	{
		int indent = HeaderPos + (pLCB->MaxNameLength - 4) / 2;
		strncpy(HeaderStr+indent,"Name",4);
	}
	HeaderPos += pLCB->MaxNameLength;
	HeaderPos += 3;
	/* Value */
	if (pLCB->MaxValuesLength > 5 /* "Value" */)
	{
		int indent = HeaderPos + (pLCB->MaxValuesLength - 5) / 2;
		strncpy(HeaderStr+indent,"Value",5);
	}
	HeaderPos += pLCB->MaxValuesLength;
	HeaderPos += 1;
	/*Mask*/
	if (pLCB->MaxMaskLength > 4 /* "Mask" */)
	{
		int indent = HeaderPos + (pLCB->MaxMaskLength - 4) / 2;
		strncpy(HeaderStr+indent,"Mask",4);
	}
	HeaderPos += pLCB->MaxMaskLength;
	HeaderPos += 1;
	
	/*Masked*/
	strncpy(HeaderStr+HeaderPos,"Masked",6);
	HeaderPos += 6 + pLCB->MaxValuesLength;
	HeaderStr[HeaderPos] = 0;
	/* Output the string */
	printf("%s\n\n",HeaderStr);

	return HeaderPos;

}


void PrintoutFooter(int Length)
{
	char FooterStr[100];
	int  FooterPos = 1;

	/*Underline*/		
	memset(FooterStr,'-', Length);
	FooterStr[Length+1] = 0;
	printf("%s\n\n",FooterStr);

}

void PrintoutItem(PItem pItem, PLISTCNTRBLOCK pLCB)
{
	char SpacePlace[100];

	//Bit range
	if (pItem->From == pItem->To) 
		printf("[  :%.02d]",pItem->From);
	else
		printf("[%.02d:%.02d]",pItem->From,pItem->To);
	// Name
	memset(SpacePlace,' ',sizeof(SpacePlace));
	SpacePlace[pLCB->MaxNameLength] = 0; //The end of Name string
	if (pItem->isName)
		sprintf(SpacePlace+(pLCB->MaxNameLength - pItem->NameLength),"%s",pItem->pName);
	printf("  %s :",SpacePlace);
	// pItem Value (regarding to bit range (or single bit)
	printf(" %s",pItem->sValues);
	memset(SpacePlace,' ',sizeof(SpacePlace));
	SpacePlace[pLCB->MaxValuesLength - pItem->ValuesLength] = 0;
	printf("%s",SpacePlace);
	// Mask
	printf(" %s",pItem->sMask);
	memset(SpacePlace,' ',sizeof(SpacePlace));
	SpacePlace[pLCB->MaxMaskLength - pItem->MaskLength] = 0;
	printf("%s",SpacePlace);
	//Masked Value
	printf(" 0x%"LONG_PR"X", pLCB->MainValue & pItem->Mask);
	
	printf("\n");
		
}


