#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

void* catchInput(void* threadInput);

int main()
{
  initscr();
  noecho();

  bool ignoreInput = false;
  char temp = ' ';

  printw("Press ENTER to toggle ignore on or off.\n");
  printw("Press q to quit and get the letter that will be output.\n");
  printw("Ignore is now off.\n");

  pthread_t inputCatcher;

  pthread_create(&inputCatcher, NULL, catchInput, (void*)&temp);

  while(temp != 'q') {
    sleep(10);
  }

  pthread_join(inputCatcher, NULL);

  endwin();

  return 0;
}

void* catchInput(void* threadInput) {
  bool ignoreInput = false;
  char* temp = (char*)threadInput;
  char output = ' ';

  while(1) {
    *temp = getch();

    if(*temp == 'q')
      return 0;
      //break;

    if(*temp == '\n') {
      if(ignoreInput) {
	printw("Ignore is now off.\n");
      }
      else {
	printw("Ignore is now on.\n");
      }
      ignoreInput = !ignoreInput;
    }
    else if(!ignoreInput) {
      ungetch(*temp);
      output = getch();
      printw("[%c] accepted.\n", output);
      ignoreInput = true;
      printw("Ignore is now on.\n");
    }
    else {
      printw("[%c] ignored.\n", *temp);
    }
  }

  /*
  printw("Output is [%c]\n", output);
  printw("Press any key to exit...\n");
  getch();

  endwin();
  return 0;
  */
}
