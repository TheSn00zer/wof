#include "wof.h"
#include "puzzles.h"

#define NUM_OF_PUZZLES 32

wholePuzzleNode* initializePuzzles()
{
  wholePuzzleNode* temp;
  wholePuzzleNode* headPtr = (wholePuzzleNode*)malloc(sizeof(wholePuzzleNode));

  temp = linkInPuzzle(headPtr, "BETTY WHITE HOUSE", "Before & After");
  temp = linkInPuzzle(temp, "Carpet and vacuum cleaner", "Same Name");
  temp = linkInPuzzle(temp, "THE FAMILY DOG", "Living Thing");
  temp = linkInPuzzle(temp, "PUBLIC SPEAKER", "Person");
  temp = linkInPuzzle(temp, "ROMANTIC GETWAWAY","Thing");
  temp = linkInPuzzle(temp, "HERBAL TEA","Food & Drink");
  temp = linkInPuzzle(temp, "THAT\'s WHAT I\'M TALKING ABOUT","Phrase");
  temp = linkInPuzzle(temp, "FORGING AHEAD", "Phrase");
  temp = linkInPuzzle(temp, "YOU MAY NOW KISS THE BRIDE OF FRANKENSTEIN","Before & After");
  temp = linkInPuzzle(temp, "MIXING BOWL", "Around the House");
  temp = linkInPuzzle(temp, "WEDDING MINISTER","Occupation");
  temp = linkInPuzzle(temp, "Underwater Camera","Thing");
  temp = linkInPuzzle(temp, "Fifth Grader","Person");
  temp = linkInPuzzle(temp, "SALT & PEPPER SHAKERS", "Things");
  temp = linkInPuzzle(temp, "HOTEL DOORMAN", "Occumpation");
  temp = linkInPuzzle(temp, "SAN FRANCISCO, CALIFORNIA","On the Map");
  temp = linkInPuzzle(temp, "RECYCLED PLASTIC SURGEON", "Before & After");
  temp = linkInPuzzle(temp, "UNIDENTIFIED FLYING OBJECTS", "Things");
  temp = linkInPuzzle(temp, "SINGAPORE", "On the Map");
  temp = linkInPuzzle(temp, "The GATEWAY ARCH IN ST. LOUIS", "Place");
  temp = linkInPuzzle(temp, "ORANGE GROVE", "Place");
  temp = linkInPuzzle(temp, "WINDOW DISPLAY", "Thing");
  temp = linkInPuzzle(temp, "CANDIED YAMS","Things");
  temp = linkInPuzzle(temp, "PAYING YOUR DUES", "Phrase");
  temp = linkInPuzzle(temp, "DRIVING TO WORK", "Event");
  temp = linkInPuzzle(temp, "MAID MARIAN", "Fictional Character");
  temp = linkInPuzzle(temp, "CHEWY LICORICE", "Food & Drink");
  temp = linkInPuzzle(temp, "ABOVE AVERAGE", "Phrase");
  temp = linkInPuzzle(temp, "HUMBLE ABODE", "Place");
  temp = linkInPuzzle(temp, "SLOWPOKE", "Person");
  temp = linkInPuzzle(temp, "FAIR ENOUGHT", "Phrase");
  temp = linkInPuzzle(temp, "YOUNG PUP", "Living Thing");
  temp->next = headPtr->next;

  return headPtr;
}

wholePuzzleNode* linkInPuzzle(wholePuzzleNode* previous, const char* puzzle,
			      const char* category)
{
  wholePuzzleNode* node = (wholePuzzleNode*)malloc(sizeof(wholePuzzleNode));
  node->puzzle = (const char*)malloc(sizeof(char) * (strlen(puzzle) + 1));
  node->puzzle = puzzle;
  node->category = (const char*)malloc(sizeof(char) * (strlen(category) + 1));
  node->category = category;
  node->seen = false;
  previous->next = node;

  return node;
}

wholePuzzleNode* getPuzzle(wholePuzzleNode* headPtr)
{
  srand((unsigned)time(0));
  int random = rand() % NUM_OF_PUZZLES;
  wholePuzzleNode* node = headPtr->next;
  for(int i = 0; i < random; i++)
    node = node->next;

  wholePuzzleNode* placeHolder = node;

  while(node->seen) {
    node = node->next;
    if(node == placeHolder)
      return NULL;
  }
  node->seen = true;
  return node;
}

void freeList(wholePuzzleNode* headPtr)
{
  if(!headPtr)
    return;

  wholePuzzleNode* nodeToFree = headPtr->next;
  wholePuzzleNode* temp;
  wholePuzzleNode* holder = nodeToFree;

  free(headPtr);

  while(nodeToFree) {
    temp = nodeToFree->next;
    //free(nodeToFree->puzzle);
    //free(nodeToFree->category);
    free(nodeToFree);
    nodeToFree = temp;
    if(nodeToFree == holder)
      break;
  }

  headPtr = NULL;
  nodeToFree = NULL;
}
