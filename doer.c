#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_LEN 5
#define ALPHABET_SIZE 26

char *evaluate(char *wordle, char *guess, char evaluation[2 * WORD_LEN + 1]);

int main(int argc, char **argv) {
    char wordle[WORD_LEN + 1];
    char guess[WORD_LEN + 1];
    char evaluation[2 * WORD_LEN + 1];

    if (argc == 2) {
        strcpy(wordle, argv[1]);
    } else if (argc > 2) {
        strcpy(wordle, argv[1]);
        for (int i = 2; i < argc; i++) {
            printf("%s\n", evaluate(wordle, argv[i], evaluation));
        }
    } else {
        printf("Enter the wordle: ");
        fgets(wordle, WORD_LEN + 1, stdin);
    }

    while (fgets(guess, WORD_LEN + 1, stdin) != NULL) {
        if (guess[0] == '\n') continue;
        printf("%s\n", evaluate(wordle, guess, evaluation));
    }
    return 0;
}

//Given a guess and a wordle outputs an evaluation string
//Dictating the yellows, greens etc. for the guess.
char *evaluate(char *wordle, char *guess, char evaluation[2 * WORD_LEN + 1]) {
    int wordle_letter_count[ALPHABET_SIZE] = {0};

    for (int i = 0; i < WORD_LEN; i++) {
        evaluation[2 * i + 1] = guess[i];
        
        wordle_letter_count[wordle[i] - 'a']++;

        if (guess[i] == wordle[i]) {
            evaluation[2 * i] = 'g';
        } else {
            evaluation[2 * i] = '-';
        }
    }
    
    int current_word_count[ALPHABET_SIZE] = {0};

    //Need to do greys and yellows while not overwriting greens
    for (int i = 0; i < WORD_LEN; i++) {
        int index = guess[i] - 'a';
        current_word_count[index]++;

        if (evaluation[2 * i] == 'g') continue;

        if (current_word_count[index] <= wordle_letter_count[index]) {
            evaluation[2 * i] = 'y';
        } else {
            evaluation[2 * i] = '-';
        }
    }
    evaluation[2 * WORD_LEN + 1] = '\0';
    return evaluation;
}