#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WORD_LEN 5
#define LINE_LEN 11
#define ALPHABET_SIZE 26

#define GREEN 'g'
#define YELLOW 'y'

struct letter_counts {
    int min;
    int max;
}

int min(int i, int j);
int max(int i, int j);

int main (int argc, char *argv[]) {
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE];
    struct letter_counts counts[ALPHABET_SIZE];

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        counts.min = 0;
        counts.max = 3;

        for (int j = 0; j < WORD_LEN; j++) {
            letter_possibilities[j][i] = true;
        }
    }

    return 0;
}

void scan_guesses(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE],
    struct letter_counts counts[ALPHABET_SIZE]
) {
    printf("Enter your guesses: \n");

    char type;
    char letter;
    char line[LINE_LEN];

    char current_word[WORD_LEN];
    int word_counts[ALPHABET_SIZE];
    int confirmed_word_counts[ALPHABET_SIZE];

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        word_counts[i] = 0;
        confirmed_word_counts[i] = 0;
    }

    while (fgets(line, LINE_LEN, stdin) != NULL) {
        //Loop over guess and check yellows and greens
        for (int i = 0; i < WORD_LEN; i++) {
            type = line[2 * i];
            letter = line[2 * i + 1];

            current_word[i] = letter;
            word_counts[letter - 'a']++;

            if (type == GREEN) {
                confirmed_word_counts[letter - 'a']++;
                confirm_letter(letter_possibilities, i, letter);
            } else if (type == YELLOW) {
                confirmed_word_counts[letter - 'a']++;
                letter_possibilities[i][letter - 'a'] = false;
            }
        }
        //Loop over guess again and check greys
        for (int i = 0; i < WORD_LEN; i++) {
            type = line[2 * i];
            letter = line[2 * i + 1];

            if (type != GREEN && type != YELLOW) {
                if (confirmed_word_counts[letter - 'a'] == 0) {
                    //None of this char in the word anywhere in any amount
                    for (int j = 0; j < WORD_LEN; j++) {
                        letter_possibilities[j][letter - 'a'] = false;
                    }
                } else {
                    //This char appears elsewhere in the word, but not as many as have appeared in your guess
                    letter_possibilities[i][]
                }
            }
        }
    }
}

void confirm_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], int letter_pos, char letter) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        letter_possibilities[letter_pos][i] = false;
    }

    letter_possibilities[letter_pos][letter - 'a'] = true;
}

int max(int i, int j) {
    if (i >= j) return i;
    return j;
}

int min(int i, int j) {
    if (i <= j) return i;
    return j;
}

