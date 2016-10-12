#ifndef PUZZLES_H
#define PUZZLES_H

typedef struct puzzle
{
  const char* puzzle;
  const char* category;
  bool seen;
  struct puzzle* next;

} wholePuzzleNode;

wholePuzzleNode* initializePuzzles();
wholePuzzleNode* linkInPuzzle(wholePuzzleNode* previous, const char* puzzle,
			      const char* category);
wholePuzzleNode* getPuzzle(wholePuzzleNode* headPtr);
void freeList(wholePuzzleNode* headPtr);

#endif
