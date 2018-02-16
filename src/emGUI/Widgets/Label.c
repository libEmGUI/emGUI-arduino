/*
	emGUI Library V1.0.0 - Copyright (C) 2013
	Lemeshev K <konstantin.lemeshev@gmail.com>,
	Roman Savrulin <romeo.deepmind@gmail.com>


	This file is part of the emGUI Library distribution.

	emGUI Library is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License (version 2) as published by the
	Roman Savrulin AND MODIFIED BY the emGUI Library exception.

	>>>NOTE<<< The modification to the GPL is included to allow you to
	distribute a combined work that includes emGUI Library without being obliged to
	provide the source code for proprietary components outside of the emGUI Library.
	emGUI Library is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details. You should have received a copy of the GNU General Public
	License and the emGUI Library license exception along with emGUI Library; if not it
	can be obtained by writing to Roman Savrulin <romeo.deepmind@gmail.com>.

	Created on: 24.12.2012
*/
#include "emGUI/Widgets/Label.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include "emGUI/Draw/Draw.h"

#ifndef min
#define min(a,b) ((a < b) ? a :b)
#endif

typedef struct xLabelProps_t {
	eLabelTextAlign     eTextAlign;
	eLabelVerticalAlign eVerticalAlign;
	char *pcStr;
	uint16_t usMaxLength;
	xFont xFnt;
	uint16_t usColor;
	bool bIsMultiLine;
	bool bHaveCursor;
	char *pcPrvPage;
	char *pcNextPage;
	char *pcCurrentPage;
	void(*onEditHandler)(void);
} xLabelProps;


//gets portion of line from pcLine that can fit between uXFrom and uXTo position
//if being print with pubFont and aligned with eHorAlign
//puXLinePosition - offset of aligned line to print from

static char* prvGetLine(char *pcLine, uint16_t uXFrom, uint16_t uXTo, uint16_t *puXLinePosition,
	xFont pubFont, eLabelTextAlign eHorAlign) {

	if (!pcLine || uXTo <= uXFrom)
		return NULL;

	uint16_t uMaxLineLen = (uint16_t)strlen(pcLine);
	char * pcDelimiter = NULL; //pointer to word break character
	char * pcChar = pcLine;
	uint16_t uXCursor = uXFrom;
	uint16_t uXCursorDelimiter = uXCursor;

	while (uMaxLineLen) {
		char c = *pcChar;

		if (c == '\t') //TODO: fix \t printing
			c = ' '; // treat tab as space

		uint16_t charWidth = pxDrawHDL()->ucFontGetCharW(c, pubFont);
		
		if ((uXCursor + charWidth) >= uXTo)
			break;

		if (pcChar != pcLine && ((c == ' ' || c == '\t') ) // delimiter can't be the first symbol
			|| c == '\n') { // but if this line is single \n, treat it as delimiter to allow draw empty line
			pcDelimiter = pcChar;
			uXCursorDelimiter = uXCursor; // temporary storage in case we have to 
		}                          // rewind to  pcDelimiter

		if (c == '\n') //line ended before we reached uXTo
			break;

		uXCursor += charWidth;
		++pcChar;
		--uMaxLineLen;
	}

	if (pcDelimiter && uMaxLineLen) // if we have to break line by delimiter, we have to update uXCursor too
		uXCursor = uXCursorDelimiter;

	if (puXLinePosition) { //alingn line in uXFrom -> uXTo frame
		switch (eHorAlign) {
		case LABEL_ALIGN_CENTER:
			*puXLinePosition = uXFrom + (uXTo - uXCursor) / 2;
			break;
		case LABEL_ALIGN_RIGHT:
			*puXLinePosition = uXFrom + (uXTo - uXCursor);
			break;
		default:
			*puXLinePosition = uXFrom;
			break;
		}
	}

	if (!uMaxLineLen || !(*pcChar))
		return NULL;

	if (!pcDelimiter)
		return pcChar; // just break the word

	if (pcDelimiter == '\0') //if this is the EOL, return it
		return pcDelimiter;

	return pcDelimiter + 1; //otherwise, return a character next to delimiter
}

static void prvPrintLine(char *pcLine, uint16_t uCharCount, uint16_t uXFrom, uint16_t uY,
	xFont pubFont, uint16_t usColor, uint16_t usBackground) {
	char *pcCharacter = pcLine;

	while (*pcCharacter && uCharCount) {
		if (*pcCharacter != '\n')
			pxDrawHDL()->vPutChar(uXFrom, uY, *pcCharacter, pubFont, usColor, usBackground, false);
		uXFrom += pxDrawHDL()->ucFontGetCharW(*pcCharacter, pubFont);
		++pcCharacter;
		--uCharCount;
	}
}

static char *prvGetPage(char *pcPage, uint16_t uXFrom, uint16_t uXTo, uint16_t uYFrom, uint16_t uYTo, uint16_t *puYPosition, uint16_t *puLineCount,
	xFont pubFont, eLabelTextAlign eHorAlign, eLabelVerticalAlign eVerAlign) {
	char *pcNxtLine;
	uint16_t uYBefor;
	uYBefor = uYFrom;
	pcNxtLine = pcPage;
	*puLineCount = 0;
	while ((uYBefor + pxDrawHDL()->usFontGetH(pubFont)) <= uYTo && pcNxtLine) {
		pcNxtLine = prvGetLine(pcNxtLine, uXFrom, uXTo, NULL, pubFont, eHorAlign);
		uYBefor += pxDrawHDL()->usFontGetH(pubFont);
		if (puLineCount)
			(*puLineCount)++;
	}

	if (puYPosition)
		switch (eVerAlign) {
		case LABEL_ALIGN_MIDDLE:
			*puYPosition = uYFrom + (uYTo - uYBefor) / 2;
			break;
		case LABEL_ALIGN_BOTTOM:
			*puYPosition = uYFrom + (uYTo - uYBefor);
			break;
		default:
			*puYPosition = uYFrom;
			break;
		}
	return pcNxtLine;
}

static void prvPrintPage(char *pcPage, uint16_t uXFrom, uint16_t uXTo, uint16_t uYFrom, uint16_t uLineCount, eLabelTextAlign eHorAlign,
	xFont pubFont, uint16_t usColor, uint16_t usBackground) {
	char *pcNxtLine, *pcCrntLine;
	uint16_t uLineLen, uXPutLine;
	pcCrntLine = pcPage;
	while (pcCrntLine && uLineCount) {
		pcNxtLine = prvGetLine(pcCrntLine, uXFrom, uXTo, &uXPutLine, pubFont, eHorAlign);
		uLineLen = pcNxtLine - pcCrntLine;
		prvPrintLine(pcCrntLine, uLineLen, uXPutLine, uYFrom, pubFont, usColor, usBackground);
		uYFrom += pxDrawHDL()->usFontGetH(pubFont);
		pcCrntLine = pcNxtLine;
		--uLineCount;
	}
}

static bool prvDraw(xWidget *pxW) {
	xLabelProps *xP;
	uint16_t uY, uLineCount;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return false;

	if (!xP->pcStr)
		return false;

	//if (strlen(xP->pcStr) > xP->usMaxLength)
	  //return false;

	if (pxW->bValid)
		return false;

	bWidgetDrawHandler(pxW);

	xP->pcNextPage = prvGetPage(xP->pcCurrentPage, pxW->usX0, pxW->usX1 + 1, pxW->usY0, pxW->usY1 + 1, &uY, &uLineCount,
		xP->xFnt, xP->eTextAlign, xP->eVerticalAlign);
	prvPrintPage(xP->pcCurrentPage, pxW->usX0, pxW->usX1 + 1, uY, uLineCount, xP->eTextAlign,
		xP->xFnt, xP->usColor, pxW->usBgColor);

	return true;
}

static void prvSetPrevPagePointer(xLabel *pxW) {
	char *pcPageCount;
	xLabelProps *xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel);
	if (!xP)
		return;
	
	xP->pcPrvPage = NULL;
	pcPageCount = xP->pcStr;
	while (pcPageCount != xP->pcCurrentPage) {
		xP->pcPrvPage = pcPageCount;
		pcPageCount = prvGetPage(pcPageCount, pxW->usX0, pxW->usX1 + 1, pxW->usY0, pxW->usY1 + 1, NULL, NULL,
			xP->xFnt, xP->eTextAlign, xP->eVerticalAlign);
	}
}

bool bLabelDrawNextPage(xLabel *pxW) {
	xLabelProps *xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel);
	if (!xP)
		return false;
	if (xP->pcNextPage) {
		xP->pcCurrentPage = xP->pcNextPage;
		vWidgetInvalidate(pxW);
		return true;
	}
	return false;
}

bool bLabelDrawPrevPage(xLabel *pxW) {
	xLabelProps *xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel);
	if (!xP)
		return false;
	prvSetPrevPagePointer(pxW);
	if (xP->pcPrvPage) {
		xP->pcCurrentPage = xP->pcPrvPage;
		vWidgetInvalidate(pxW);
		return true;
	}
	return false;
}

static bool prvDispose(xWidget * pxW) {
	xLabelProps *xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel);
	if (!xP)
		return false;

	if (xP->usMaxLength && xP->pcStr)
		free(xP->pcStr);

	return true;
}

// if usMaxLength = 0, internal label logic uses external pointer to string,
// i.e. cStr contents is not copied to internal storage, 
// because internal storage is not allocated at all
//
// This ideally fits case when string is in read-only segment. But be careful with
// PROGMEM on ARDUINO and ESP8266!
xLabel * pxLabelCreate(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, char const * cStr, xFont xFnt, uint16_t usMaxLength, xWidget *pxWidParent) {
	xLabel *pxW;
	xLabelProps *xP;

	if (!cStr)
		return NULL;

	pxW = malloc(sizeof(xWidget));
	memset(pxW, 0, sizeof(xWidget)); //TODO: add check logic!

	if (!pxW)
		return NULL;

	if (!xFnt)
		xFnt = pxDrawHDL()->xGetDefaultFont();

	if (usH < pxDrawHDL()->usFontGetH(xFnt))
		usH = pxDrawHDL()->usFontGetH(xFnt);

	if (bWidgetInit(pxW, usX, usY, usW, usH, pxWidParent, true)) {

		pxW->eType = WidgetLabel;
		pxW->pxOnDispose = prvDispose;

		vWidgetSetBgColor(pxW, EMGUI_COLOR_WHITE, false);

		xP = malloc(sizeof(xLabelProps));
		
		if (!xP) {
			free(pxW);
			return NULL;
		}

		memset(xP, 0, sizeof(xLabelProps));
		pxW->pvProp = xP;

		usMaxLength = (usMaxLength > EMGUI_LABEL_MAX_LENGTH) ? EMGUI_LABEL_MAX_LENGTH : usMaxLength;

		if (0 == usMaxLength) { //use external pointer to const char * directly
			xP->pcStr = (char *)cStr; ///WARNING!
		}
		else { // allocating memory for internal storage
			//+ 1 //for '\0' char in the end
			xP->pcStr = malloc(usMaxLength);
		}

		//memory is not allocated! Aborting!
		if (!xP->pcStr) {
			vWidgetDispose(pxW);
			return NULL;
		}

		xP->usMaxLength = usMaxLength;

		xP->eTextAlign = LABEL_ALIGN_LEFT;
		xP->eVerticalAlign = LABEL_ALIGN_TOP;

		xP->usColor = EMGUI_COLOR_BLACK;
		xP->xFnt = xFnt;

		if (usMaxLength) {
			memcpy(xP->pcStr, cStr, min(strlen(cStr) + 1, usMaxLength));
			//Terminating string
			xP->pcStr[usMaxLength - 1] = '\0';
		}

		xP->pcCurrentPage = xP->pcStr;
		pxW->pxDrawHandler = prvDraw;

		return pxW;
	}
	else {
		free(pxW);
		return NULL;
	}
}

char * pcLabelSetText(xWidget *pxW, const char * pcStr) {
	xLabelProps *xP;
	char *pcLine;
	uint16_t  usMaxLength;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)) || !pcStr)
		return NULL;

	pcLine = xP->pcStr;
	usMaxLength = xP->usMaxLength;

	// if we can copy to usMaxLength
	if (usMaxLength) {
		if (!strcmp(pcStr, pcLine))
			return NULL;

		memcpy(pcLine, pcStr, min(strlen(pcStr) + 1 , usMaxLength));
		pcLine[usMaxLength - 1] = '\0';
	}
	else
		xP->pcStr = (char*)pcStr;

	xP->pcCurrentPage = xP->pcStr;
	xP->pcNextPage = xP->pcPrvPage = NULL;

	vWidgetInvalidate(pxW);
	return NULL;
}

void pcLabelSetTextAdaptWidth(xLabel *pxW, const char * pcStr) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return;

	pcLabelSetText(pxW, pcStr);
	uint16_t X0 = usWidgetGetX0(pxW, false);
	uint16_t X1 = X0 + pxDrawHDL()->usFontGetStrW(pcStr, xP->xFnt);
	
	if (X1 > EMGUI_LCD_WIDTH)
		X1 = EMGUI_LCD_WIDTH;
	
	bWidgetSetCoords(pxW, 
		X0,
		usWidgetGetY0(pxW, false),
		X1,
		usWidgetGetY1(pxW, false),
		false
	);
	vWidgetInvalidate(pxW);
}

void vLabelSetTextColor(xWidget *pxW, uint16_t usColor) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return;

	xP->usColor = usColor;
	vWidgetInvalidate(pxW);
}

void vLabelSetTextAlign(xWidget *pxW, eLabelTextAlign eAlign) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return;

	xP->eTextAlign = eAlign;
	vWidgetInvalidate(pxW);
}

void vLabelSetVerticalAlign(xWidget *pxW, eLabelVerticalAlign eAlign) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return;

	xP->eVerticalAlign = eAlign;
	vWidgetInvalidate(pxW);
}

/*void vLabelSetTextExt(xWidget *pxW, char * pStr, int usMaxLength) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return;

	//Для сброса режима
	if (!usMaxLength) {
		pcLabelSetText(pxW, pStr);
		xP->usMaxLength = 0;
	}

	//Внешнюю строку можно устанавливать только если не было создано внутр. хранилище!
	if (xP->usMaxLength)
		return;

	if (xP->pcStr == pStr)
		return;

	xP->pcStr = pStr;
	xP->usMaxLength = usMaxLength;

	xP->pcCurrentPage = xP->pcStr;
	xP->pcPrvPage = xP->pcNextPage = NULL;

	vWidgetInvalidate(pxW);
}*/

const char * pcLabelGetText(xWidget *pxW) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return NULL;

	return xP->pcStr;
}

bool bLabelSetMultiline(xWidget *pxW, bool bMultiLine) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return false;

	xP->bIsMultiLine = bMultiLine;

	return true;
}

bool bLabelSetMaxLength(xLabel *pxW, size_t uiMaxLength, eLabelMaxLenModificator eType) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		return 0;

	size_t currentML = iLabelGetMaxLength(pxW);

	switch (eType)
	{
	case LABEL_MAXLEN_EXTEND:
		if (currentML >= uiMaxLength)
			return true;
		break;
	case LABEL_MAXLEN_SHRINK:
		if (currentML <= uiMaxLength)
			return true;
		break;
	default:
		break;
	}

	if (uiMaxLength > 0 && currentML == uiMaxLength || uiMaxLength > EMGUI_LABEL_MAX_LENGTH)
		return false;

	if (currentML && xP->pcStr) {
		free(xP->pcStr);
		xP->pcStr = NULL;
	}

	xP->pcStr = malloc(uiMaxLength);

	xP->pcCurrentPage = xP->pcStr;
	xP->pcNextPage = xP->pcPrvPage = NULL;

	if (!xP->pcStr) {
		xP->usMaxLength = 0;
		return false;
	}

	xP->pcStr[uiMaxLength - 1] = 0;
	xP->pcStr[0] = 0;

	xP->usMaxLength = (uint16_t) uiMaxLength;

	return true;
}

int iLabelGetMaxLength(xLabel *pxL) {
	xLabelProps *xP;

	if (!(xP = (xLabelProps *)pxWidgetGetProps(pxL, WidgetLabel)))
		return 0;

	return xP->usMaxLength;
}

bool bLabelAppendChar(xWidget *pxW, char cChar, bool bSetInvalidate) {
	xLabelProps *xP;
	if ((xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel))) {
		uint16_t usLen = (uint16_t)strlen(xP->pcStr);
		if (usLen + 1 < xP->usMaxLength) {
			xP->pcStr[usLen] = cChar;
			xP->pcStr[usLen + 1] = '\0';
			if (bSetInvalidate)
				vWidgetInvalidate(pxW);
			return true;
		}
	}
	return false;
}

bool bLabelBackSpace(xWidget *pxW, bool bSetInvalidate) {
	xLabelProps *xP;
	if ((xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel))) {
		uint16_t usLen = (uint16_t)strlen(xP->pcStr);
		if (usLen > 0) {
			xP->pcStr[usLen - 1] = '\0';
			if (bSetInvalidate)
				vWidgetInvalidate(pxW);
			return true;
		}
	}
	return false;
}

void vLabelSetOnEditHandler(xWidget *pxW, void(*callback)(void)) {
	xLabelProps *xP;
	if ((xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		xP->onEditHandler = callback;
}

void vLabelSetHaveCursor(xWidget *pxW, bool bLabelHaveCursor) {
	xLabelProps *xP;
	if ((xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel)))
		xP->bHaveCursor = bLabelHaveCursor;
}

void vLabelClear(xWidget *pxW, bool bSetInvalidate) {
	xLabelProps *xP;
	if ((xP = (xLabelProps *)pxWidgetGetProps(pxW, WidgetLabel))) {
		xP->pcStr[0] = '\0';
		if (bSetInvalidate)
			vWidgetInvalidate(pxW);
	}
}

int iLabelPrintf(xWidget *pxW, char const *pcFmt, ...) {
	xLabelProps *xP;
	int iRet = 0;

	if (!(xP = (xLabelProps*)pxWidgetGetProps(pxW, WidgetLabel)))
		return 0;

	if (!xP->usMaxLength)
		return 0;

	va_list args;
	va_start(args, pcFmt);

	iRet = vsnprintf(xP->pcStr, xP->usMaxLength, pcFmt, args);

	vWidgetInvalidate(pxW);
	return iRet;
}