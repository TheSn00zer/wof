#ifndef SCREEN_H
#define SCREEN_H

typedef struct node
{
  char letter;
  bool chosen;
  int X;
  int Y;
  struct node* next;
  struct node* previous;

} puzzleLetterNode;

void correctScreenSize(WINDOW* win);
int getPuzzleSpaceCount(const char* puzzle);
char* getPuzzleWord(const char* puzzle, int* placeInPuzzle);
void printInMiddle(WINDOW* win, int y, const char* stringToPrint);
void clearLine(WINDOW* win, int y);
void printSpace(WINDOW* win, int startX, int startY, std::string spaceString);
void printStrengthBar(WINDOW* win, int y, int x, int strength, int middle);
void printBoard(WINDOW* win);

puzzleLetterNode* preparePuzzle(WINDOW* win, const char* puzzle);
puzzleLetterNode* linkInLetter(puzzleLetterNode* previous, char letter,
			       bool chosen, int X,
			int Y);
void printPuzzle(WINDOW* win, puzzleLetterNode* headPtr, bool stall);
void printPuzzle(WINDOW* win, puzzleLetterNode* headPtr, const char* category,
		 bool stall);
void printRoundBank(WINDOW* win, int money);
void printBank(WINDOW* win, int money);
void printRound(WINDOW* win, int round);
void printAlphabetLeft(WINDOW* win, alphabetLetter* headPtr);
void printTurns(WINDOW* win, int turnsLeft);
int guessLetter(WINDOW* win, puzzleLetterNode* headPtr, char letter,
		int* count);
void printBoardAndPuzzle(WINDOW* win, puzzleLetterNode* headPtr,
			 const char* category);
wheelPiece* spin(WINDOW* win, wheelPiece* headPtr, int strength);
int getStrength(WINDOW* win);
bool solve(WINDOW* win, puzzleLetterNode* headPtr);
void solveAllLetters(puzzleLetterNode* headPtr);
bool isSolved(puzzleLetterNode* headPtr);
void clearAboveWheel(WINDOW* win);
void freeList(puzzleLetterNode* headPtr);

#endif
