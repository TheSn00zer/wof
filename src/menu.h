#ifndef INTRO_H
#define INTRO_H

typedef struct vars
{
  bool titleDone;
  bool exitMenu;
  bool showTitle;
  wheelPiece* headPtr;
} introVars;

int showMenu(WINDOW* win, wheelPiece* headPtr, int menuChoice,
	     bool showTitle);
void* introTitle(void* winIn);
void* introAnimation(void* headPtr);

#endif
