#include <unistd.h>
#include "wof.h"
#include "menu.h"
#include "printer.h"
#include "puzzles.h"

userInputStruct input = {'?',false};
bool globalCatchInput = false;

using namespace std;

int main() {
  int roundBank = 0;
  int bank = 0;
  int round = 1;

  pthread_t inputCatcher;

  pthread_create(&inputCatcher, NULL, catchInput, (void*)&input);
  // ^-- ignoreInput(true);

  initscr();

  WINDOW* win = stdscr;

  curs_set(0);

  noecho();

  if(!has_colors()) {
    endwin();
    printf("Oops! Your terminal doesn't support colors.\n");
    return -1;
  }

  correctScreenSize(win);

  start_color();

  init_pair(1, COLOR_WHITE, COLOR_RED);
  init_pair(2, COLOR_WHITE, COLOR_GREEN);
  init_pair(3, COLOR_WHITE, COLOR_YELLOW);
  init_pair(4, COLOR_WHITE, COLOR_BLUE);
  init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(6, COLOR_WHITE, COLOR_CYAN);
  init_pair(7, COLOR_WHITE, COLOR_BLACK); // used for bankrupt and text
  init_pair(8, COLOR_BLACK, COLOR_WHITE); // used for lose turn
  init_pair(9, COLOR_BLACK, COLOR_WHITE); // used for printing board letter
  init_pair(10, COLOR_WHITE, COLOR_WHITE); // used for printing board blank
  init_pair(11, COLOR_GREEN, COLOR_GREEN); // used for printing board background
  init_pair(12, COLOR_BLUE, COLOR_BLUE); // used for around the board

  /*
    creates puzzle linked list and returns the first node
    the nodes are circularly linked
  */
  wholePuzzleNode* puzzleHeadPtr = (wholePuzzleNode*)malloc(sizeof(wholePuzzleNode));
  puzzleHeadPtr = initializePuzzles();

  /*
    creates wheel linked list and returns the first node
    the nodes are circularly linked
  */
  wheelPiece* wheelHeadPtr = initializeWheel();

  alphabetLetter* alphabetHeadPtr = initializeAlphabet();

  int menuChoice = PLAY;
  bool showIntro = true;
  //showIntro = false;

 MENU:
  menuChoice = showMenu(win, wheelHeadPtr, menuChoice, showIntro);
  showIntro = false;

  if(menuChoice == EXIT) {
    clear();
    printInMiddle(win, SCREEN_HEIGHT / 2 - 1,
		  "Thanks for playing Wheel of Fortune!");
    refresh();
    wait(2000);
  }
  else if(menuChoice == RULES) {
    clear();
    printInMiddle(win, SCREEN_HEIGHT / 2 - 1,
		  "RULES!");
    printInMiddle(win, SCREEN_HEIGHT / 2,
		  PRESS_ENTER);
    refresh();
    while(getch() != '\n'){}
    clearAboveWheel(win);
    refresh();
    goto MENU;
  }
  else if(menuChoice == PLAY) {
    while(1) {

      ignoreInput(true);

      wholePuzzleNode* currentPuzzle = getPuzzle(puzzleHeadPtr);

      if(currentPuzzle == NULL){
	endwin();
	exit(0);
      }

      letter* guessesHeadPtr = (letter*)malloc(sizeof(letter));
      guessesHeadPtr->letter = -1;
      guessesHeadPtr->next = NULL;

      clearAboveWheel(win);

      printBoard(win);
      printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4,
		    "Here is your puzzle...");

      const char* puzzle = currentPuzzle->puzzle;
      const char* category = currentPuzzle->category;

      puzzleLetterNode* puzzleLetterHeadPtr = preparePuzzle(win, puzzle);

      int turnsLeft = 5;

      printTurns(win, turnsLeft);
      printRoundBank(win, roundBank);
      printAlphabetLeft(win, alphabetHeadPtr);
      printBank(win, bank);
      printRound(win, round);
      printPuzzle(win, puzzleLetterHeadPtr, category, true);

      bool puzzleSolved = false;

      ignoreInput(false);

      while(!puzzleSolved && turnsLeft > 0) {
	int spinValue = 0;
	char instructions[] = "Press ENTER to spin or \'s\' to solve";
	char instructionsV[] = "Press ENTER to spin, \'v\' to buy a vowel for $250, or \'s\' to solve";

	clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 4);
	if(roundBank >= 250)
	  printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3,
			instructionsV);
	else
	  printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3,
			instructions);

	printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 1, "V");
	clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 2);

	refresh();
	int input = getReadyGlobalInput();

	bool chooseVowel = false;

	if(roundBank >= 250) {
	  while(!(input == '\n' || input == 's' || input == 'S' ||
		  input == 'v' || input == 'V' )) {
	    input = getch();
	  }
	}
	else {
	  while(!(input == '\n' || input == 's' || input == 'S')) {
	    input = getch();
	  }
	}

	if(input == 's' || input == 'S') {
	  puzzleSolved = solve(win, puzzleLetterHeadPtr);

	  printPuzzle(win, puzzleLetterHeadPtr, false);
	  if(puzzleSolved)
	    break;
	  else {
	    turnsLeft--;
	    printTurns(win, turnsLeft);
	    ignoreInput(true);
	    ignoreInput(false);
	  }
	}
	else {
	  if(input == '\n') {
	    chooseVowel = false;

	    refresh();
	    int strength = getStrength(win);
	    ignoreInput(true);

	    /* returns the piece that the player landed on */
	    wheelPiece* piece = spin(win, wheelHeadPtr, strength);

	    spinValue = piece->value;

	    /* clear "spinning..." */
	    clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 2);

	    if(spinValue <= 0) {
	      if(spinValue == BANKRUPT) {
		printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4,
			      "Sorry, you went bankrupt!");
		roundBank = 0;
		printRoundBank(win, roundBank);

		turnsLeft--;
		printTurns(win, turnsLeft);
	      }
	      else if(spinValue == LOSE_TURN) {
		printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4,
			      "Sorry, you lose your turn.");

		turnsLeft--;
		printTurns(win, turnsLeft);
	      }

	      printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3,
			    PRESS_ENTER);
	      refresh();
	      ignoreInput(false);
	      if(getReadyGlobalInput() != '\n'){
		while(getch() != '\n'){}
	      }
	      ignoreInput(true);
	      ignoreInput(false);
	      continue;
	    }

	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4, piece->name);
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3,
			  "Type a consonant: ");
	    curs_set(1);
	    refresh();
	    ignoreInput(false);
	    input = getReadyGlobalInput();
	  }
	  else if(input == 'v' || input == 'V') {
	    chooseVowel = true;
	    clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 3);
	    curs_set(1);
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3,
			  "Type a vowel: ");
	    refresh();
	    input = getch();
	  }
	  refresh();

	  /* This should never happen since the thread will grab an input */
	  if(input == -1){
	    input = getch();
	  }

	  if(chooseVowel) {
	    while((!(input >= 'a' && input <= 'z') &&
		   !(input >= 'A' && input <= 'Z')) ||
		  !isVowel(input)) {
	      input = getch();
	    }
	    roundBank -= 250;
	    printRoundBank(win, roundBank);
	  }

	  else {
	    while((!(input >= 'a' && input <= 'z') &&
		   !(input >= 'A' && input <= 'Z')) ||
		  isVowel(input) ) {
	      input = getch();
	    }
	  }

	  curs_set(0);

	  if(input >= 'a' && input <= 'z')
	    input = toupper(input);

	  ignoreInput(true);

	  clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 2);

	  char playerChoice[12] = "You chose ";
	  playerChoice[10] = input;
	  playerChoice[11] = '\0';

	  clearLine(win, SCREEN_HEIGHT - SPACE_LENGTH - 3);
	  printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3, playerChoice);

	  int numberCorrect = 0;

	  int outcome = checkGuess(guessesHeadPtr, input);
	  outcome = SUCCESS;

	  bool letterGuessed = checkIfGuessed(alphabetHeadPtr, input);
	  printAlphabetLeft(win, alphabetHeadPtr);
	  if(letterGuessed)
	    outcome = ALREADY_GUESSED;
	  else
	    outcome = guessLetter(win, puzzleLetterHeadPtr, input,
				  &numberCorrect);

	  if(outcome == FAILURE) {
	    char message[23] = "Sorry, there are no ";
	    message[20] = input;
	    message[21] = 's';
	    message[22] = '\0';
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4, message);
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3, "Press ENTER to continue..?");
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3, PRESS_ENTER);
	    turnsLeft--;
	    printTurns(win, turnsLeft);
	    ignoreInput(false);
	    int pressEnter = getReadyGlobalInput();
	    while(pressEnter != '\n'){
	      pressEnter = getch();
	    }
	    ignoreInput(true);
	  }
	  else if(outcome == SUCCESS) {
	    roundBank += (spinValue * numberCorrect);
	    printRoundBank(win, roundBank);
	  }
	  else {
	    char message[28] = "Oops! You already guessed ";
	    message[26] = input;
	    message[27] = '\0';
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 4, message);
	    printInMiddle(win, SCREEN_HEIGHT - SPACE_LENGTH - 3, PRESS_ENTER);
	    turnsLeft--;
	    printTurns(win, turnsLeft);
	    ignoreInput(false);
	    int pressEnter = getReadyGlobalInput();
	    while(pressEnter != '\n'){
	      pressEnter = getch();
	    }
	    ignoreInput(true);
	  }
	  ignoreInput(false);

	  //start checking for a complete puzzle, out of turns, etc.
	  puzzleSolved = isSolved(puzzleLetterHeadPtr);
	}
      } // end while(1) inner - end of letter-guessing loop

      if(puzzleSolved) {
	bank += roundBank;
	printBank(win, bank);
	ignoreInput(false);
	ignoreInput(true);
	ignoreInput(false);
	printPuzzle(win, puzzleLetterHeadPtr, false);
	clearLine(win, 11);
	printInMiddle(win, 11, "You solved the puzzle!");
	clearLine(win, 12);
	printInMiddle(win, 12, PRESS_ENTER);
	refresh();

	int pressEnter = getReadyGlobalInput();
	while(pressEnter != '\n'){
	  pressEnter = getch();
	}
      }
      else {
	ignoreInput(false);

	printPuzzle(win, puzzleLetterHeadPtr, false);
	clearLine(win, 11);
	printInMiddle(win, 11, "You ran out of turns!");
	clearLine(win, 12);
	printInMiddle(win, 12, PRESS_ENTER);
	refresh();
	int pressEnter = getReadyGlobalInput();
	while(pressEnter != '\n'){
	  pressEnter = getch();
	}
      }

      roundBank = 0;
      freeList(puzzleLetterHeadPtr);
      resetAlphabet(alphabetHeadPtr);
      round++;
      // go to menu?

    } // end while(1) outer - end of puzzle round

    //goto MENU;
  }

  endwin();

  freeList(wheelHeadPtr);
  freeList(puzzleHeadPtr);
  return 0;
}

bool isVowel(char letter)
{
  letter = tolower(letter);
  if(letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o'
     || letter == 'u')
    return true;
  return false;
}

void ignoreInput(bool value)
{
  globalCatchInput = value;
  /* give the catcher time to catch the input */
  if(value)
    wait(100);
}

void* catchInput(void* trash)
{
  while(1) {
    if(globalCatchInput){
      input.uInput = getch();

      if(globalCatchInput)
	input.uInput = -1;
      else {
	input.ready = true;
      }
    }
    wait(1);
  }
  return NULL;
}

int getReadyGlobalInput(void)
{
  int temp;
  /*
    stalls the program while the last input caught by input-catcher
    thread is not ready
  */
  while(!input.ready){
    wait(1);
  }
  input.ready = false;
  temp = input.uInput;
  input.uInput = -1;
  return temp;
}


wheelPiece* initializeWheel()
{
  wheelPiece* temp;
  wheelPiece* headPtr = (wheelPiece*)malloc(sizeof(wheelPiece));

  /*
    RED 1
    GREEN 2
    YELLOW 3
    BLUE 4
    MAGENTA 5
    CYAN 6
    BLACK 7
    WHITE 8
    Because the colors don't look right use the following subs:

    - purple -> BLUE
    - blue -> CYAN
    - pink -> MAGENTA
   */
  temp = linkIn(headPtr, "$400", 400, YELLOW);
  temp = linkIn(temp, "$300", 300, MAGENTA);
  temp = linkIn(temp, "$900", 900, RED);
  temp = linkIn(temp, "$500", 500, CYAN);
  temp = linkIn(temp, "$300", 300, GREEN);
  temp = linkIn(temp, "$900", 900, MAGENTA);
  temp = linkIn(temp, "BANKRUPT", -2, BLACK);
  temp = linkIn(temp, "$600", 600, BLUE);
  temp = linkIn(temp, "$400", 400, YELLOW);
  temp = linkIn(temp, "$300", 300, CYAN);
  temp = linkIn(temp, "LOSE TURN", -1, WHITE);
  temp = linkIn(temp, "$800", 800, RED);
  temp = linkIn(temp, "$350", 350, BLUE);
  temp = linkIn(temp, "$450", 450, MAGENTA);
  temp = linkIn(temp, "$700", 700, GREEN);
  temp = linkIn(temp, "$300", 300, YELLOW);
  temp = linkIn(temp, "$600", 600, BLUE);
  temp = linkIn(temp, "$5000", 5000, WHITE);
  temp = linkIn(temp, "$600", 600, GREEN);
  temp = linkIn(temp, "$500", 500, YELLOW);
  temp = linkIn(temp, "$300", 300, RED);
  temp = linkIn(temp, "$500", 500, CYAN);
  temp = linkIn(temp, "$800", 800, YELLOW);
  temp = linkIn(temp, "$550", 550, BLUE);
  temp->next = headPtr->next; // complete circle

  return headPtr;
}

wheelPiece* linkIn(wheelPiece* previous, const char* name, int value, int colorIndex)
{
  wheelPiece* node = (wheelPiece*)malloc(sizeof(wheelPiece));
  node->name = (const char*)malloc(sizeof(char) * (strlen(name) + 1));
  node->name = name;
  node->value = value;
  node->colorIndex = colorIndex;
  previous->next = node;

  return node;
}

wheelPiece* getPiece(wheelPiece* piece, int count)
{
  for(int i = 0; i < count; i++)
    piece = piece->next;

  return piece;
}

int getNextTime(int time)
{
  return (time + (float)(time * (float)(log10(time) - 1) / 17));
}

int strengthToDelay(int strength)
{
  double delay = 100 - (6 * (strength - 1));

  srand((unsigned)time(0));
  double randomPercent = (rand() % PERCENT_CHANGE) / 100.0;
  int sign = rand() % 2;

  if(sign == 0) // subtract
    delay -= (delay * randomPercent);
  else
    delay += (delay * randomPercent);

  if(delay > MAX_START_DELAY)
    delay = MAX_START_DELAY;
  else if(delay < MIN_DELAY)
    delay = MIN_DELAY;

  return (int)delay;
}

void wait(int milliseconds)
{
  refresh();
  usleep(milliseconds * 1000);
}

alphabetLetter* initializeAlphabet()
{
  alphabetLetter* headPtr = (alphabetLetter*)malloc(sizeof(alphabetLetter));
  headPtr->next = (alphabetLetter*)malloc(sizeof(alphabetLetter));
  alphabetLetter* temp = headPtr->next;

  for(int i = 'A'; i <= 'Z'; i++){
    temp->letter = i;
    temp->next = (alphabetLetter*)malloc(sizeof(alphabetLetter));
    temp = temp->next;
  }

  return headPtr;
}

void resetAlphabet(alphabetLetter* headPtr)
{
  alphabetLetter* temp = headPtr->next;
  while(temp) {
    temp->chosen = false;
    temp = temp->next;
  }
}

bool checkIfGuessed(alphabetLetter* headPtr, int input) {
  alphabetLetter* temp = headPtr->next;

  while(temp) {
    if(temp->letter == input) {
      if(temp->chosen)
	return true;
      else {
	temp->chosen = true;
	return false;
      }
    }
    temp = temp->next;
  }
  return false;
}

int checkGuess(letter* headPtr, int input)
{
  letter* temp = headPtr->next;

  /* no guesses yet! link in the first guess */
  if(!temp) {
    headPtr->next = (letter*)malloc(sizeof(letter));
    headPtr->next->letter = input;
    headPtr->next->next = NULL;
    return SUCCESS;
  }

  while(temp) {
    if(temp->letter == input)
      return ALREADY_GUESSED;
    if(temp->next)
      temp = temp->next;
    else {
      temp->next = (letter*)malloc(sizeof(letter));
      temp->next->letter = input;
      temp->next->next = NULL;
      return SUCCESS;
    }
  }

  return SUCCESS;
}

void freeList(wheelPiece* headPtr)
{
  if(!headPtr)
    return;

  wheelPiece* nodeToFree = headPtr->next;
  wheelPiece* temp;
  wheelPiece* holder = nodeToFree;

  free(headPtr);

  while(nodeToFree) {
    temp = nodeToFree->next;
    free(nodeToFree);
    nodeToFree = temp;
    if(nodeToFree == holder)
      break;
  }

  headPtr = NULL;
  nodeToFree = NULL;
}
