#include <unistd.h>
#include "wof.h"
#include "printer.h"
#include "menu.h"

using namespace std;

int showMenu(WINDOW* win, wheelPiece* headPtr,
	     int menuChoice, bool showTitle)
{
  introVars* iv = (introVars*)malloc(sizeof(introVars));
  pthread_t animationThread;
  pthread_t titleThread;

  iv->titleDone = true;
  iv->showTitle = showTitle;

  if(showTitle)
    iv->titleDone = false;

  iv->exitMenu = false;
  iv->headPtr = headPtr;

  pthread_create(&animationThread, NULL, introAnimation, (void*)iv);

  if(showTitle) {
    pthread_create(&titleThread, NULL, introTitle, (void*)iv);

    while(!iv->titleDone){
      while(getch() != '\n'){
	sleep(1);
      }
    }

    // title is done, now show menu (see below)
    clearAboveWheel(win);
  }

  int key = 0;

  while(key != '\n') {

    if(menuChoice == PLAY)
      attron(COLOR_PAIR(1));
    printInMiddle(win, 4, "           ");
    printInMiddle(win, 5, "   PLAY    ");
    printInMiddle(win, 6, "           ");
    if(menuChoice == PLAY)
      attroff(COLOR_PAIR(1));

    if(menuChoice == RULES)
      attron(COLOR_PAIR(1));
    printInMiddle(win, 7, "           ");
    printInMiddle(win, 8, "   RULES   ");
    printInMiddle(win, 9, "           ");
    if(menuChoice == RULES)
      attroff(COLOR_PAIR(1));

    if(menuChoice == EXIT)
      attron(COLOR_PAIR(1));
    printInMiddle(win, 10, "           ");
    printInMiddle(win, 11, "   EXIT    ");
    printInMiddle(win, 12, "           ");
    if(menuChoice == EXIT)
      attroff(COLOR_PAIR(1));

    refresh();

    keypad(win, TRUE);
    key = wgetch(win);

    if(key == KEY_DOWN)
      menuChoice += 1;
    else if(key == KEY_UP)
      menuChoice--;
    if(menuChoice < 0)
      menuChoice = 0;
    else if(menuChoice > 2)
      menuChoice = 2;
  }

  iv->exitMenu = true;

  pthread_join(animationThread, NULL);

  if(showTitle)
    pthread_join(titleThread, NULL);

  free(iv);

  return menuChoice;
}

void* introTitle(void* iv)
{
  introVars* temp = (introVars*) iv;
  WINDOW* win = stdscr;
  const char* introArray[SCREEN_HEIGHT][SCREEN_WIDTH];

  *introArray[0] =  " _ _ _ _           _ ";
  *introArray[1] =  "| | | | |_ ___ ___| |";
  *introArray[2] =  "| | | |   | -_| -_| |";
  *introArray[3] =  "|_____|_|_|___|___|_|";
  *introArray[4] =  "     ___ ";
  *introArray[5] =  " ___|  _|";
  *introArray[6] =  "| . |  _|";
  *introArray[7] =  "|___|_|  ";
  *introArray[8] =  " _____         _               ";
  *introArray[9] =  "|   __|___ ___| |_ _ _ ___ ___ ";
  *introArray[10] = "|   __| . |  _|  _| | |   | -_|";
  *introArray[11] = "|__|  |___|_| |_| |___|_|_|___|";

  *introArray[12] = "";
  *introArray[13] = "";

  for(int i = 0; i < 4; i++)
    printInMiddle(win, i, *introArray[i]);
  refresh();

  wait(850);

  for(int i = 4; i < 8; i++)
    printInMiddle(win, i, *introArray[i]);
  refresh();

  wait(850);

  for(int i = 8; i < 14; i++)
    printInMiddle(win, i, *introArray[i]);

  refresh();

  wait(850);

  printInMiddle(win, 13, PRESS_ENTER);

  refresh();

  temp->titleDone = true;
  iv = temp;

  return NULL;
}

void* introAnimation(void* iv)
{
  refresh();
  introVars* ivCopy = (introVars*)iv;
  int startX = 0;
  int startY = 0;
  wheelPiece* tempHeadPtr = ivCopy->headPtr;
  wheelPiece* nextFirst = tempHeadPtr->next;
  wheelPiece* piece = nextFirst;

  int rise = SPACE_LENGTH;

  if(!ivCopy->showTitle)
    rise = 0;

  while(!ivCopy->exitMenu || !ivCopy->titleDone) {
    startY = SCREEN_HEIGHT;
    startX = SCREEN_WIDTH;

    startX = (startX - (NUM_OF_SPACES * SPACE_WIDTH))/ 2;
    startY -= SPACE_LENGTH - rise;

    if(ivCopy->showTitle) {
      rise--;
      if(rise < 0)
	rise = 0;
    }

    piece = nextFirst;

    for(int i = 0; i < NUM_OF_SPACES; i++) {
      attron(COLOR_PAIR(piece->colorIndex));
      printSpace(stdscr, startX, startY, piece->name);
      startX += SPACE_WIDTH;
      attroff(COLOR_PAIR(piece->colorIndex));
      piece = piece->next;
    }
    refresh();

    /*
      set the head to point to the next piece so that
      when the player ends the intro, the pieces will be saved
    */
    tempHeadPtr->next = nextFirst;
    ivCopy->headPtr = tempHeadPtr;

    nextFirst = nextFirst->next;
    wait(INTRO_WHEEL_DELAY);
  }

  return NULL;
}
