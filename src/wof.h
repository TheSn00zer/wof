#ifndef WOF_H
#define WOF_H

#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>

#define PRESS_ENTER "Press ENTER to continue..."
#define LETTERS_LEFT "Letters left:"

#define PLAY 0
#define RULES 1
#define EXIT 2

#define NUM_OF_COLORS 6
#define SPACE_WIDTH 3
#define SPACE_LENGTH 9
#define NUM_OF_SPACES 13
#define SPACE_WIDTH 3
#define MAX_STRENGTH 10
#define MIN_STRENGTH 1
#define MIN_DELAY 40
#define MAX_START_DELAY 100
#define MAX_DELAY 350
#define PERCENT_CHANGE 20
#define INTRO_WHEEL_DELAY 120
#define BOARD_ROWS 3
#define BOARD_HEIGHT (BOARD_ROWS * 2 + 1) + 2

#define PUZZLE_LINE_1 16
#define PUZZLE_LINE_2 19
#define PUZZLE_LINE_3 16
#define REVEAL_PUZZLE_DELAY 90

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24

#define BANKRUPT -2
#define LOSE_TURN -1

#define SUCCESS 1
#define FAILURE 0
#define ALREADY_GUESSED -1

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define BLACK 7
#define WHITE 8

typedef struct piece
{
  const char* name;
  int value; // lose a turn = -1, bankrupt = -2
  int colorIndex;
  struct piece* next;
} wheelPiece;

typedef struct input
{
  char uInput;
  bool ready;
} userInputStruct;

typedef struct letterStruct
{
  int letter;
  struct letterStruct* next;
} letter;

typedef struct alphabetStruct
{
  int letter;
  int chosen;
  struct alphabetStruct* next;
} alphabetLetter;

wheelPiece* initializeWheel(void);
wheelPiece* linkIn(wheelPiece*, const char* name, int value, int colorIndex);
void wait(int seconds);
int getNextTime(int time);
int strengthToDelay(int strength);
wheelPiece* getPiece(wheelPiece* piece, int index);
void* catchInput(void* trash);
void ignoreInput(bool value);
int getReadyGlobalInput(void);
bool isVowel(char letter);
bool checkIfGuessed(alphabetLetter* headPtr, int input);
int checkGuess(letter* guessHeadPtr, int input);
alphabetLetter* initializeAlphabet();
void resetAlphabet(alphabetLetter* headPtr);
void freeList(wheelPiece* headPtr);

#endif
