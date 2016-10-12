#include "wof.h"
#include "printer.h"
#include "puzzles.h"

using namespace std;

void correctScreenSize(WINDOW* win)
{
  int initialTermWidth = 0;
  int initialTermHeight = 0;
  getmaxyx(win, initialTermHeight, initialTermWidth);

  if(initialTermHeight == SCREEN_HEIGHT
     && initialTermWidth == SCREEN_WIDTH) {}
  else if(initialTermHeight < SCREEN_HEIGHT
     || initialTermWidth < SCREEN_WIDTH) {
    printw("Wheel of Fortune is meant for a terminal of size %dx%d, ",
	      SCREEN_WIDTH, SCREEN_HEIGHT);
    printw("but yours is %dx%d.\n", initialTermWidth, initialTermHeight);
    printw("Please resize the terminal to %dx%d.\n",
	   SCREEN_WIDTH, SCREEN_HEIGHT);
    printw("Press ENTER to exit...\n");
    while(getch() != '\n'){}
    endwin();
    exit(-1);
  }
  else {
    printw("Wheel of Fortune is meant for a terminal of size %dx%d, ",
	   SCREEN_WIDTH, SCREEN_HEIGHT);
    printw("but yours is %dx%d.\n", initialTermWidth, initialTermHeight);
    printw("There will be blank space on the ");
    if(initialTermWidth > SCREEN_WIDTH && initialTermHeight > SCREEN_HEIGHT)
      printw("right and bottom");
    else if(initialTermWidth > SCREEN_WIDTH)
      printw("right");
    else if(initialTermHeight > SCREEN_HEIGHT)
      printw("bottom");
    printw(" of the terminal.\n");
    mvwprintw(win, 3, 0, "Continue anyway? (y or n): ");
    noecho();
    char input = (char)getch();
    input = tolower(input);
    while(!(input == 'y' || input == 'n')) {
      mvwprintw(win, 3, 0, "Continue anyway? (y or n): ");
      input = (char)getch();
    }
    if(input == 'n') {
      endwin();
      printf("\nGoodbye!\n\n");
      exit(-1);
    }
    clear();
    refresh();
  }

  noecho();
  clear();
}

void printBoardAndPuzzle(WINDOW* win, puzzleLetterNode* headPtr,
			 const char* category)
{
  printBoard(win);
  printPuzzle(win, headPtr, category, false);
}

void printBoard(WINDOW* win)
{
  /*
    12,14,14,12
   */

  int board[7];
  board[0] = PUZZLE_LINE_1 * 2 - 1;
  board[1] = PUZZLE_LINE_1 * 2 - 1;
  board[2] = PUZZLE_LINE_2 * 2 - 1;
  board[3] = PUZZLE_LINE_2 * 2 - 1;
  board[4] = PUZZLE_LINE_2 * 2 - 1;
  board[5] = PUZZLE_LINE_3 * 2 - 1;
  board[6] = PUZZLE_LINE_3 * 2 - 1;

  /*
  9 -> (COLOR_BLACK, COLOR_WHITE) used for printing board letter
  10-> (COLOR_WHITE, COLOR_WHITE) used for printing board blank
  11-> (COLOR_GREEN, COLOR_GREEN) used for printing board background
  12-> (COLOR_BLUE, COLOR_BLUE) used for outside of board
  */
  attron(COLOR_PAIR(12));
  for(int i = 0; i < BOARD_HEIGHT; i++)
    for(int j = 0; j < SCREEN_WIDTH; j++)
      mvwprintw(win, i, j, " ");
  attroff(COLOR_PAIR(12));

  attron(COLOR_PAIR(11));
  for(int i = 0; i < BOARD_HEIGHT - 2; i++) {
    for(int j = 0; j < board[i]; j++){
      mvwprintw(win, i + 1, j + ((SCREEN_WIDTH - board[i]) / 2), " ");
    }
  }
  attroff(COLOR_PAIR(11));
}

puzzleLetterNode* preparePuzzle(WINDOW* win, const char* puzzle)
{
  puzzleLetterNode* headPtr = (puzzleLetterNode*)malloc(sizeof(puzzleLetterNode));
  puzzleLetterNode* tempNode = (puzzleLetterNode*)malloc(sizeof(puzzleLetterNode));

  const int puzzleWordCount = getPuzzleSpaceCount(puzzle) + 1;

  char* puzzleWords[puzzleWordCount];
  int placeInPuzzle = 0;

  for(int i = 0; i < puzzleWordCount; i++) {
    char* tempWord = getPuzzleWord(puzzle, &placeInPuzzle);
    puzzleWords[i] = (char*)malloc(sizeof(char) * (strlen(tempWord) + 1));
    strncpy(puzzleWords[i], tempWord, strlen(tempWord));
    free(tempWord);
  }


  int puzzleRow = 1;
  int puzzleSpace = ((SCREEN_WIDTH - (PUZZLE_LINE_1 * 2 - 1)) / 2) + 1;
  int realPuzzleRow;

  int maxLineWidths[] = {PUZZLE_LINE_1, PUZZLE_LINE_2, PUZZLE_LINE_3};

  /*
  headPtr->next = tempNode;
  */
  headPtr = tempNode;

  for(int i = 0; i < puzzleWordCount; i++) {
    if(puzzleSpace + (int)strlen(puzzleWords[i]) * 2 - 1
       >= ((maxLineWidths[puzzleRow - 1] * 2 - 1)
	   + (SCREEN_WIDTH - (maxLineWidths[puzzleRow - 1] * 2 - 1)) / 2)) {

      puzzleRow += 1;
      puzzleSpace = ((SCREEN_WIDTH -
		      (maxLineWidths[puzzleRow - 1] * 2 - 1)) / 2) + 1;
    }

    realPuzzleRow = (puzzleRow - 1) * 2 + 2;

    for(int j = 0; j < (int)strlen(puzzleWords[i]); j++) {
      tempNode = linkInLetter(tempNode, toupper(puzzleWords[i][j]),
			     false, puzzleSpace, realPuzzleRow);
      puzzleSpace += 2;
    }
    puzzleSpace += 2;
  }

  headPtr->next->previous = NULL;
  return headPtr;
}

puzzleLetterNode* linkInLetter(puzzleLetterNode* previous, char letter,
			      bool chosen, int X, int Y)
{
  puzzleLetterNode* node = (puzzleLetterNode*)malloc(sizeof(puzzleLetterNode));
  node->letter = letter;
  if(!((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z')))
    node->chosen = true;
  else
   node->chosen = chosen;
  node->X = X;
  node->Y = Y;
  node->next = NULL;
  node->previous = previous;
  previous->next = node;
  return node;
}

void printPuzzle(WINDOW* win, puzzleLetterNode* headPtr, const char* category,
		 bool stall)
{
  attron(COLOR_PAIR(4));
  mvwprintw(win, 0, 1, "Category:");
  mvwprintw(win, 1, 1, "              ");
  mvwprintw(win, 1, 1, "%s", category);
  /*
  mvwprintw(win, 2, SCREEN_WIDTH - strlen(temp) - 4, "A B C D E F G");
  mvwprintw(win, 3, SCREEN_WIDTH - strlen(temp) - 4, "H   J   L M N");
  mvwprintw(win, 4, SCREEN_WIDTH - strlen(temp) - 4, "O P Q   S T U");
  mvwprintw(win, 5, SCREEN_WIDTH - strlen(temp) - 4, "V W X Y Z");
  */
  attroff(COLOR_PAIR(4));

  printPuzzle(win, headPtr, stall);
}

void printRoundBank(WINDOW* win, int money)
{
  attron(COLOR_PAIR(4));
  mvwprintw(win, 3, 1, "Money this round:");
  mvwprintw(win, 4, 1, "              ");
  mvwprintw(win, 4, 1, "$%d", money);
  attroff(COLOR_PAIR(4));
}

void printBank(WINDOW* win, int money)
{
  mvwprintw(win, SCREEN_HEIGHT - 3, 1, "Money won:");
  mvwprintw(win, SCREEN_HEIGHT - 2, 1, "              ");
  mvwprintw(win, SCREEN_HEIGHT - 2, 1, "$%d", money);
}

void printRound(WINDOW* win, int round)
{
  mvwprintw(win, SCREEN_HEIGHT - 6, 1, "Round:");
  mvwprintw(win, SCREEN_HEIGHT - 5, 1, "%d", round);
}

void printTurns(WINDOW* win, int turnsLeft)
{
  attron(COLOR_PAIR(4));
  mvwprintw(win, 6, 1, "Turns left:", turnsLeft);
  mvwprintw(win, 7, 1, "              ");
  mvwprintw(win, 7, 1, "%d", turnsLeft);
  attroff(COLOR_PAIR(4));
}

void printPuzzle(WINDOW* win, puzzleLetterNode* headPtr, bool stall)
{
  puzzleLetterNode* current = headPtr->next;

  /*
    9 -> (COLOR_BLACK, COLOR_WHITE) used for printing board letter
    10-> (COLOR_WHITE, COLOR_WHITE) used for printing board blank
    11-> (COLOR_GREEN, COLOR_GREEN) used for printing board background
    12-> (COLOR_BLUE, COLOR_BLUE) used for outside of board
  */

  attron(COLOR_PAIR(9));
  while(current != NULL) {
    if(current->chosen)
      mvwprintw(win, current->Y, current->X, "%c", current->letter);
    else
      mvwprintw(win, current->Y, current->X, " ");

    if(stall)
      wait(REVEAL_PUZZLE_DELAY);
    current = current->next;
  }
  attroff(COLOR_PAIR(9));
}

void printAlphabetLeft(WINDOW* win, alphabetLetter* headPtr)
{
  int row = 0;
  int space = SCREEN_WIDTH - strlen(LETTERS_LEFT) - 6;
  alphabetLetter* temp = headPtr->next;
  attron(COLOR_PAIR(4));
  mvwprintw(win, row, space, LETTERS_LEFT);
  row++;
  while(temp) {
    if(space >= SCREEN_WIDTH - 2) {
      space = SCREEN_WIDTH - strlen(LETTERS_LEFT) - 6;
      row++;
    }
    if(!temp->chosen)
      mvwprintw(win, row, space, "%c", temp->letter);
    else
      mvwprintw(win, row, space, " ", temp->letter);

    space += 2;

    temp = temp->next;
  }
  attroff(COLOR_PAIR(4));
  refresh();
}

 /*
   1 -> correct
   0 -> incorrect
   -1 -> guessed letter already
 */
int guessLetter(WINDOW* win, puzzleLetterNode* headPtr, char letter,
		int* numberCorrect)
{
  *numberCorrect = 0;
  int outcome = FAILURE;
  puzzleLetterNode* current = (puzzleLetterNode*)malloc(sizeof(puzzleLetterNode));
  current = headPtr->next;

  attron(COLOR_PAIR(12));
  while(current != NULL) {
    if(current->letter == letter) {
      /* letter found */
      outcome = SUCCESS;

      if(current->chosen) {
	wait(300);
	attroff(COLOR_PAIR(12));
	return ALREADY_GUESSED;
      }

      while(current != NULL) {
	if(current->letter == letter) {
	  current->chosen = true;
	  *numberCorrect += 1;
	  mvwprintw(win, current->Y, current->X, " ");
	  refresh();
	  wait(500);
	}
	current = current->next;
      }
      break;
    }
    current = current->next;
  }
  attroff(COLOR_PAIR(12));

  /*
    9 -> (COLOR_BLACK, COLOR_WHITE) used for printing board letter
    10-> (COLOR_WHITE, COLOR_WHITE) used for printing board blank
    11-> (COLOR_GREEN, COLOR_GREEN) used for printing board background
    12-> (COLOR_BLUE, COLOR_BLUE) used for outside of board
  */

  current = headPtr->next;

  wait(800);

  attron(COLOR_PAIR(9));
  while(current != NULL) {
    if(current->letter == letter) {
      mvwprintw(win, current->Y, current->X, "%c", current->letter);
      refresh();
      wait(500);
    }
    current = current->next;
  }
  attroff(COLOR_PAIR(9));

  free(current);
  return outcome;
}

int getPuzzleSpaceCount(const char* puzzle)
{
  int spaces = 0;
  for(int i = 0; i < (int)strlen(puzzle); i++)
    if((char)puzzle[i] == ' ')
      spaces++;
  return spaces;
}

char* getPuzzleWord(const char* puzzle, int* placeInPuzzle)
{
  int wordSize = 0;
  int start;
  char* word;

  /* skip spaces */
  while(puzzle[*placeInPuzzle] == ' ') {
    *placeInPuzzle += 1;
  }

  start = *placeInPuzzle;

  while(*placeInPuzzle < (int)strlen(puzzle)) {
    if(puzzle[*placeInPuzzle] == ' ')
      break;
    wordSize += 1;
    *placeInPuzzle += 1;
  }

  word = (char*)malloc(sizeof(char) * (wordSize + 1));
  for(int i = 0; i < wordSize; i++) {
    word[i] = puzzle[start + i];
  }
  word[wordSize] = '\0';

  return word;
}

void clearAboveWheel(WINDOW* win)
{
  for(int i = 0; i < SCREEN_HEIGHT - SPACE_LENGTH - 1; i++) {
    clearLine(win, i);
  }
}

wheelPiece* spin(WINDOW* win, wheelPiece* headPtr, int strength)
{
  int spinCount = 0;
  int delay = strengthToDelay(strength);
  int startX = 0;
  int startY = 0;
  wheelPiece* piece = headPtr->next;
  wheelPiece* nextFirst = piece;

  printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 2, "spinning...");
  printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 1, "V");

  while(delay < MAX_DELAY) {
    startX = SCREEN_WIDTH;
    startY = SCREEN_HEIGHT;

    startX = (startX - (NUM_OF_SPACES * SPACE_WIDTH))/ 2;
    startY -= SPACE_LENGTH;

    piece = nextFirst;

    for(int i = 0; i < NUM_OF_SPACES; i++) {
      attron(COLOR_PAIR(piece->colorIndex));
      printSpace(win, startX, startY, piece->name);
      startX += SPACE_WIDTH;
      attroff(COLOR_PAIR(piece->colorIndex));
      piece = piece->next;
    }

    refresh();
    headPtr->next = nextFirst; // i want the wheel to keep its position
    nextFirst = nextFirst->next;
    spinCount += 1;
    wait(delay);
    delay = getNextTime(delay);
  }

  /* the head is the first shown piece. get piece in middle of shown pieces */
  int pieceToGet = (NUM_OF_SPACES / 2) + ((NUM_OF_SPACES) % 2);
  piece = getPiece(headPtr, pieceToGet);
  spinCount = 0;

  return piece;
}

void clearLine(WINDOW* win, int y)
{
  for(int i = 0; i < SCREEN_WIDTH; i++)
    mvwprintw(win, y, i, " ");
}

void printInMiddle(WINDOW* win, int y, const char* stringToPrint)
{
  if(y > SCREEN_HEIGHT) {
    printw("Error printing [%s] in function printInMiddle", stringToPrint);
    exit(-3);
  }
  mvwprintw(win, y, (SCREEN_WIDTH - strlen(stringToPrint)) / 2, stringToPrint);
  refresh();
}

int getStrength(WINDOW* win)
{
  int strength = 1;
  int input = -1;

  int startX = SCREEN_WIDTH / 2;

  /*
    SPACE_LENGTH lines from bottom, 5 more lines for text,
    1 more for gap
  */
  int startY = SCREEN_HEIGHT - SPACE_LENGTH - 6;
  char instr1[] = "Please choose the spin strength";
  char instr2[] = "Use the right and left arrow keys and press ENTER when finished";
  char instr3[] = "------------";
  char instr4[] = "|          |";

  attron(COLOR_PAIR(7));

  clearLine(win, startY);
  printInMiddle(win, startY, instr1);

  clearLine(win, startY + 1);
  printInMiddle(win, startY + 1, instr2);

  clearLine(win, startY + 2);
  printInMiddle(win, startY + 2, instr3);

  clearLine(win, startY + 3);
  printInMiddle(win, startY + 3, instr4);

  clearLine(win, startY + 4);
  printInMiddle(win, startY + 4, instr3);

  startX = (SCREEN_WIDTH - MAX_STRENGTH) / 2;
  printStrengthBar(win, startY + 3, startX, strength, SCREEN_WIDTH / 2);
  

  while(1) {
    keypad(win, TRUE);
    input = wgetch(win);

    if(input == KEY_ENTER || input == '\n') {
      clearLine(win, startY);
      clearLine(win, startY + 1);
      clearLine(win, startY + 2);
      clearLine(win, startY + 3);
      clearLine(win, startY + 4);
      refresh();
      attroff(COLOR_PAIR(7));
      return strength;
    }

    else if(input == KEY_LEFT) {
      strength -= 1;
      if(strength < MIN_STRENGTH)
	strength = MIN_STRENGTH;
      printStrengthBar(win, startY + 3, startX, strength, SCREEN_WIDTH / 2);
    }

    else if(input == KEY_RIGHT) {
      strength += 1;
      if(strength > MAX_STRENGTH)
	strength = MAX_STRENGTH;
      printStrengthBar(win, startY + 3, startX, strength, SCREEN_WIDTH / 2);
    }
  }

  return -1;
}

void printStrengthBar(WINDOW* win, int y, int x, int strength, int middle) {

  for(int i = 0; i < MAX_STRENGTH; i++)
    mvwprintw(win, y, x + i, " ");
  mvwprintw(win, y + 1, middle + 1, "-");
  for(int i = 0; i < strength; i++){
    mvwprintw(win, y, x + i, ">");
    mvwprintw(win, y + 1, middle - 2, "(%d)", strength);
  }
}

bool solve(WINDOW* win, puzzleLetterNode* headPtr)
{
  bool solved = true;
  puzzleLetterNode* node = headPtr->next;

  while(node != NULL) {
    if(node->chosen)
      node = node->next;
    else {
      attron(COLOR_PAIR(1));
      mvwprintw(win, node->Y, node->X, " ");

      int input = toupper(getch());

      while(!(input >= 'A' && input <= 'Z')) {
	input = toupper(getch());
      }

      if(input != node->letter)
	solved = false;

      mvwprintw(win, node->Y, node->X, "%c", input);
      attroff(COLOR_PAIR(1));
      node = node->next;
    }
  }

  if(solved)
    solveAllLetters(headPtr);
  return solved;
}

void solveAllLetters(puzzleLetterNode* headPtr)
{
  puzzleLetterNode* node = headPtr->next;
  while(node != NULL) {
    node->chosen = true;
    node = node->next;
  }
}

bool isSolved(puzzleLetterNode* headPtr)
{
  puzzleLetterNode* node = headPtr->next;

  while(node != NULL) {
    if(!node->chosen)
      return false;
    node = node->next;
  }
  return true;
}

void printSpace(WINDOW* win, int startX, int startY, string spaceString)
{
  int spaceStringLength = spaceString.length();
  int stringPlace = 0;
  const char* spaceString_c = spaceString.c_str();

  for(int i = startY; i < startY + SPACE_LENGTH; i++) {
    mvwprintw(win, i, startX, " ");
    if(stringPlace < spaceStringLength)
      mvwprintw(win, i, startX + 1, "%c", spaceString_c[stringPlace]);
    else
      mvwprintw(win, i, startX + 1, " ");
    mvwprintw(win, i, startX + 2, " ");
    stringPlace += 1;
  }
}

void freeList(puzzleLetterNode* headPtr)
{
  if(!headPtr)
    return;

  puzzleLetterNode* nodeToFree = headPtr->next;
  puzzleLetterNode* temp;

  free(headPtr);

  while(nodeToFree) {
    temp = nodeToFree->next;
    free(nodeToFree);
    nodeToFree = temp;
  }

  headPtr = NULL;
  nodeToFree = NULL;
}
