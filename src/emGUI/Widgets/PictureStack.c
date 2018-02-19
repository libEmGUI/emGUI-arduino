/*
	emGUI Library V1.0.0 - Copyright (C) 2013 
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

#include "PictureStack.h"
#include "StatusBar.h"
#include "emGUI/Draw/Draw.h"
#include <stdlib.h> 

bool static prvDraw(xPictureStack *pxW){
  xPictureStackProps *xP;
  
  if(!pxW)
    return false;
  
  if(pxW->eType != WidgetPictureStack)
    return false;
  
  if(pxW->bValid)
    return false;
  
  xP = pxW->pvProp;
  
  pxDrawHDL()->bPicture(pxW->usX0, pxW->usY0, xPictureStackGetItem(pxW, xP->cItemNumber));
  return true;
}

static bool prvDispose(xPictureStack * pxW) {
	xPictureStackProps *xP;
	xPictureStackItem *xI;
	xPictureStackItem *xNext;

	if (!pxW)
		return false;

	if (pxW->eType != WidgetPictureStack)
		return false;

	xP = pxW->pvProp;


	//Находим последний элемент в списке
	xNext = xP->xItems;
	xI = xNext;
	while (xNext) {
		xI = xNext;
		xNext = xI->pxNext;
		free(xI);
	}

	/*if (xP->pxOnDispose)
		xP->pxOnDispose(pxW);*/

	xI->pxNext = NULL;
	xP->cItemCount = 0;

	return true;
}

xPictureStack *pxPictureStackCreate(uint16_t usX, uint16_t usY, xPicture xPic, xWidget *pxWidParent){
  xPictureStack *pxW;
  xPictureStackProps *xP;
  xPictureStackItem *xI;
  
  pxW = malloc(sizeof(xWidget));
  memset(pxW, 0, sizeof(xWidget));
  if(bWidgetInit(pxW, usX, usY, 1, 1, pxWidParent, true)){
    
	bWidgetSetBgPicture(pxW, xPic);

	xI = malloc(sizeof(xPictureStackItem));
	
    if(!xI)
      return NULL;
	memset(xI, 0, sizeof(xPictureStackItem));
    xI->xPic = xPic;
    xI->pxNext = NULL;

    xP = malloc(sizeof(xPictureStackProps));
	
    if(!xP)
      return NULL;
	memset(xP, 0, sizeof(xPictureStackProps));
    xP->cItemNumber = 0;
    xP->cItemCount = 1;
	pxW->pxOnDispose = prvDispose;
	xP->bDisposable = true;
    xP->xItems = xI;
    
    pxW->pvProp = xP;
        
    pxW->eType = WidgetPictureStack;
      
    pxW->pxDrawHandler = prvDraw;
    
    return pxW;
  }
  else{
    free(pxW);
    return NULL;
  }
}

bool bPictureStackSelect(xWidget *pxW, char cItemNumber){
  xPictureStackProps *xP;
  
  if(!pxW)
    return false;
  
  if(pxW->eType != WidgetPictureStack)
    return false;
  
  xP = pxW->pvProp;
  
  if(cItemNumber >= xP->cItemCount)
    return false;
  
  if(cItemNumber == xP->cItemNumber)
    return true;
  
  xP->cItemNumber = cItemNumber;
  vWidgetInvalidate(pxW);
  
  return true;
}

bool bPictureStackAddItem(xWidget *pxW, xPicture xPic){
  xPictureStackProps *xP;
  xPictureStackItem *xI;
  xPictureStackItem *xNext;
  
  if(!pxW)
    return false;
  
  if(pxW->eType != WidgetPictureStack)
    return false;
  
  xP = pxW->pvProp;
  
  if(xP->cItemCount == PICTURE_STACK_MAX_SIZE)
    return false;
  
  //Находим последний элемент в списке
  xNext = xP->xItems;
  xI = xNext;
  while(xNext){
    xI = xNext;
    xNext = xI->pxNext;
  }

  //Allocate memory for new Item
  xNext = malloc(sizeof(xPictureStackItem));

  if(!xNext)
    return false;

  xNext->xPic = xPic;
  xNext->pxNext = NULL;

  //TODO: Check Picture Dimensions here
  xI->pxNext = xNext;
  xP->cItemCount++;

  return true;
}

xPicture xPictureStackGetItem(xWidget *pxW, char cItemNumber){
  xPictureStackProps *xP;
  xPictureStackItem *xI;
  xPictureStackItem *xNext;
  
  if(!pxW)
    return NULL;
  
  if(pxW->eType != WidgetPictureStack)
    return NULL;
  
  xP = pxW->pvProp;
  
  if(cItemNumber >= xP->cItemCount)
    return NULL;
  
  xNext = xP->xItems;

  do{
    xI = xNext;
    xNext = xI->pxNext;
  }while(cItemNumber-- && xNext);

  return xI->xPic;
}

bool bPictureStackClose(xPictureStack *pxW) {
	xPictureStackProps *xP;
	if (!(xP = (xPictureStackProps*)pxWidgetGetProps(pxW, WidgetPictureStack)))
		return false;


	vWidgetHide(pxW);

	if (xP->bDisposable)
		vWidgetDispose(pxW);

	return true;
}