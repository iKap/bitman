/*****************************************************************************/
/*                       Bit extractor main file                             */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "global.h"
#include "print.h"
#include "types.h"
#include "esc_colors.h"

unsigned char HexMap[256];

void HexMapInit()
{
	MY_NAME;

	memset(&HexMap,0,sizeof(HexMap));

	HexMap['0'] = 0x0; HexMap['1'] = 0x1; HexMap['2'] = 0x2; HexMap['3'] = 0x3;
	HexMap['4'] = 0x4; HexMap['5'] = 0x5; HexMap['6'] = 0x6; HexMap['7'] = 0x7;
	HexMap['8'] = 0x8; HexMap['9'] = 0x9; HexMap['A'] = 0xA; HexMap['B'] = 0xB;
	HexMap['C'] = 0xC; HexMap['D'] = 0xD; HexMap['E'] = 0xE; HexMap['F'] = 0xF;
	HexMap['a'] = 0xa; HexMap['b'] = 0xb; HexMap['c'] = 0xc; HexMap['d'] = 0xd;
	HexMap['e'] = 0xe; HexMap['f'] = 0xf;
}

__INT64 _htoi64(const char *hexstr)
{
	__INT64 val = 0;
	int   i;
	for (i = 0; ((i < 18) && (*hexstr)); i++)
	{
		val <<= 4;
		val += HexMap[*hexstr];
		hexstr++;
	}
	return val;
}

void AssignmentCheck(char *pParamStr, PLISTCNTRBLOCK pLCB)
{
	char *pAssValue;
	PItem pItem = pLCB->pCurr;

	MY_NAME;

	if (pAssValue = strchr(pParamStr,'='))
	{
		*pAssValue = 0;
		pAssValue += 1;
		pItem->isAssignment = TRUE;

		if ((pAssValue[0] == '0') && (pAssValue[1] == 'x'))// Hex representation of Assignment value
		{
			pAssValue+=2;
			pItem->AssValue = _htoi64(pAssValue);
		}
		else // Decimal representation
		{
			pItem->AssValue = ATOI(pAssValue);
		}
		pLCB->isGlobalAssignment |= TRUE;
	}

	pLCB->isGlobalAssignment |= pItem->isAssignment;
}


void NameCheck(char *pParamStr, PLISTCNTRBLOCK pLCB)
{
	char *pNextStr, *pName;
	PItem pItem = pLCB->pCurr;

	MY_NAME;

	if (pName = strchr(pParamStr,'\'')) // Open bracket
	{
		*pName = 0;
		pName += 1;
		pItem->pName = pName;
		if (pNextStr = strchr(pName,'\'')) // Next, closing bracket
		{
			*pNextStr = 0;
			pItem->isName = TRUE;
			pItem->NameLength = pNextStr - pName; // Name Length

			if(pLCB->MaxNameLength < pItem->NameLength )
			{
				pLCB->MaxNameLength = pItem->NameLength;
			}

			return;
		}
		else
		{
			pItem->isValid = FALSE;
			printf(COLOR(IRed,"Name invalid (%s)\n"),pParamStr);
			return;
		}
	}
	return;
}

void ReadBitRange(char *pParamStr, PLISTCNTRBLOCK pLCB)
{
	char *pNextStr;
	int   tmp;
	PItem pItem = pLCB->pCurr;

	MY_NAME;

	if (pParamStr[0] == ':') // One bit
	{
		pParamStr+=1;
		pItem->From = pItem->To = atoi(pParamStr);
		pItem->isValid = TRUE;
	}
	else // Bit interval
	{
		if ( pNextStr = strchr(pParamStr,':') )
		{
			*pNextStr = 0;
			pNextStr +=1;
			pItem->From = atoi(pParamStr);
			pItem->To   = atoi(pNextStr);
			if (pItem->From > pItem->To)
			{
				tmp         = pItem->From;
				pItem->From = pItem->To;
				pItem->To   = tmp;
			}
			pItem->isValid  = TRUE;
		}
		else
		{
			pItem->isValid = FALSE;
			printf(COLOR(IRed,"Bit range invalid (%s)\n"),pParamStr);
		}
	}//Bit interval
}//ReadBitRange

void ProcessParam(PLISTCNTRBLOCK pLCB )
{
	int i;
	__INT64 LocalMask;
	PItem pItem = pLCB->pCurr;

	MY_NAME;

	// Performing the Masks
	pItem->Mask   = 0;
	LocalMask     = 0;
	for (i = pItem->From; i <= pItem->To; i++)
	{
		pItem->Mask    |= ( (__INT64)1 << i );
		LocalMask     <<= 1;
		LocalMask      |= (__INT64)1 ;
	}

	pItem->ItemValue = (pLCB->MainValue & pItem->Mask) >> pItem->From;

	if (pItem->isAssignment)
	{
		pLCB->ResultValue &= ~pItem->Mask;
		pItem->AssValue   &= LocalMask;
		pLCB->ResultValue |= (pItem->AssValue << pItem->From );
	}

}

void PerformStrings(PLISTCNTRBLOCK pLCB)
{
	PItem pItem = pLCB->pCurr;

	// Perform strings
	if (pItem->isValid)
	{
		//ItemValue and AssignedValue (if present)
		memset(pItem->sValues, 0, sizeof(pItem->sValues));
		if (pItem->isAssignment)
		{
			sprintf(pItem->sValues,"0x%"LONG_PR"X <= 0x%"LONG_PR"X%n",
											  pItem->ItemValue,
											  pItem->AssValue,
											  &(pItem->ValuesLength));
		}
		else
		{
			sprintf(pItem->sValues,"0x%"LONG_PR"X%n",
											  pItem->ItemValue,
											  &(pItem->ValuesLength));
		}

		// Finding maximum value length
		if (pLCB->MaxValuesLength < pItem->ValuesLength)
			pLCB->MaxValuesLength = pItem->ValuesLength;
		
		//Mask
		memset(pItem->sMask,0,sizeof(pItem->sMask));
		sprintf(pItem->sMask,"0x%"LONG_PR"X%n",
										  pItem->Mask,
										  &(pItem->MaskLength));
		// Finding maximum mask length
		if (pLCB->MaxMaskLength < pItem->MaskLength)
			pLCB->MaxMaskLength = pItem->MaskLength;
		
	}

}
	
__INT64 ReadMainValue(char *pStrVal)
{
__INT64 MainValue;

	if (pStrVal[0]=='0' && pStrVal[1]=='x')
	{ // This is a HEX representation
		pStrVal+=2;
		MainValue = _htoi64(pStrVal);
#if 0
		printf("in HEX: ");
		printf(" 0x%"LONG_PR"X (%"LONG_PR"u)\n\n", MainValue, MainValue);  
#endif		
	}
	else // Decimal Representation
	{
		MainValue = ATOI(pStrVal);
#if 0
		printf("in DEC: ");
		printf(" %"LONG_PR"u (0x%"LONG_PR"X)\n\n", MainValue, MainValue);  
#endif		
	}

}

void InitListCntrBlock(PLISTCNTRBLOCK pLCB)
{
	memset(pLCB, 0, sizeof(TLISTCNTRBLOCK));
	// Initial values
	pLCB->MaxValuesLength  = 6;
	pLCB->MaxMaskLength    = 5;
}

TLISTCNTRBLOCK LCB; // List Control Block

#ifdef _WIN32
void InitConsole()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; //ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void InitConsole() {}
#endif

int main(int argc, char *argv[])
{
	PLISTCNTRBLOCK pLCB = &LCB;
	PItem pItem = 0;
	int HeaderLen;

	InitConsole();
	InitListCntrBlock(pLCB);
	
	if (argc < 2)
	{
		PrintUsage(APP_NAME_SHORT); // TODO: need to separate name from path basename
		// TODO: Write good description
		return -1;
	}

	HexMapInit();
	pLCB->ResultValue = pLCB->MainValue = ReadMainValue(argv[1]);

	printf("\n");
	printf(COLOR(BIWhite,"[Input]:"));
	printf(" 0x%"LONG_PR"X (%"LONG_PR"u)\n\n", pLCB->MainValue, pLCB->MainValue);  


	PrintoutValue(pLCB->MainValue);

	if (argc > 2) // if more then Value parameter present.
	{
		int arg_cnt;

		pItem = (PItem)malloc(sizeof(TItem)*argc);
		memset( pItem, 0, sizeof(TItem)*argc);

		// 1.Prepare
		for (arg_cnt=2; arg_cnt < argc; arg_cnt++)
		{
			char *pParamStr  =  argv[arg_cnt];

			pItem = (PItem)malloc(sizeof(TItem));
			memset(  pItem, 0,    sizeof(TItem));

			if (pLCB->pHead == NULL)
				pLCB->pHead = pItem;

			if (pLCB->pCurr)
				pLCB->pCurr->pNext = pItem;

			pLCB->pCurr = pItem;

			//Check for Assignment is present
			AssignmentCheck(pParamStr, pLCB);
			//Check for Name is present and calculate a maximum length
			NameCheck(pParamStr, pLCB);
			//Read bit-range 
			ReadBitRange(pParamStr, pLCB);
			//Calculating Item value and make assignments if present
			ProcessParam(pLCB);
			//Preparing string representation of item value and mask
			PerformStrings(pLCB);

		}//for

		// Output

		// Header
		HeaderLen = PrintoutHeader(pLCB);

		// Items
		pItem = pLCB->pHead;
		while (pItem)
		{
				PrintoutItem(pItem, pLCB);
				pItem = pItem->pNext;
		}

		PrintoutFooter(HeaderLen);

		/*Result Value if some assignment exist*/
		if (pLCB->isGlobalAssignment && pLCB->ResultValue)
		{
			PrintoutValue(pLCB->ResultValue);
			printf(COLOR(BIWhite,"\n[Result]: "));
			printf("0x%"LONG_PR"X (%"LONG_PR"u)\n", pLCB->ResultValue, pLCB->ResultValue);
		}
	}//argc
	printf("\n");

	//system("pause");
	return 0;
}

