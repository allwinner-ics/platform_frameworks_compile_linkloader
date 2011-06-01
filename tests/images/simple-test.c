#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main() {
  srand(time(NULL));

  unsigned int ans = rand() % 100;
  unsigned int user = 100;
  unsigned int left = 0;
  unsigned int right = 99;

  printf("Hello, droid!  Let's play a number guessing game!\n");

  while (user != ans) {
    printf("Please input a number [%d-%d]:\n", left, right);

    if (scanf("%u", &user) != 1) {
      break;
    }

    if (user < left || user > right) {
      /* Out of range, ignore this answer. */
      continue;
    } else if (user == ans) {
      printf("You got it!\n");
      break;
    } else if (user < ans) {
      left = user;
    } else {
      right = user;
    }
  }

  return 0;
}
