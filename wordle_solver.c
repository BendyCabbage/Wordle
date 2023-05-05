#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "trie.c"

#define WORD_LEN 5
#define LINE_LEN 11
#define ALPHABET_SIZE 26

#define GREEN 'g'
#define YELLOW 'y'

struct letter_counts {
    int min;
    int max;
};

int min(int i, int j);
int max(int i, int j);

void init(
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
);
void scan_guesses(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE],
    struct letter_counts counts[ALPHABET_SIZE]
);

void confirm_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], int letter_pos, char letter);
void remove_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], char letter);

void find_possible_words(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
);

void print_matching_words(
    Trie t, int depth, char *current_word,     
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
);

int main (int argc, char *argv[]) {
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE];
    struct letter_counts counts[ALPHABET_SIZE];

    init(letter_possibilities, counts);
    scan_guesses(letter_possibilities, counts);
    find_possible_words(letter_possibilities, counts);
    return 0;
}

void init(
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        counts[i].min = 0;
        counts[i].max = 3;

        for (int j = 0; j < WORD_LEN; j++) {
            letter_possibilities[j][i] = true;
        }
    }
}

/*
    Scans in the current guesses from stdin
    Input should be styled like: 
        - in front of grey characters
        y in front of yellow characters
        g in front of green characters
        \n to separate different words
        ctr + d to stop inputting

    Does not need to run efficiently as max 6 inputted words
*/
void scan_guesses(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE],
    struct letter_counts counts[ALPHABET_SIZE]
) {
    printf("Enter your guesses: \n");

    char type, letter;
    char line[LINE_LEN];
    int index;

    int word_counts[ALPHABET_SIZE];

    while (fgets(line, LINE_LEN, stdin) != NULL) {
        if (line[0] == '\n') continue;

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            word_counts[i] = 0;
        }

        //Loop over guess and check yellows and greens
        for (int i = 0; i < WORD_LEN; i++) {
            type = line[2 * i];
            letter = line[2 * i + 1]; 
            index = letter - 'a';

            if (type == GREEN) {
                word_counts[index]++;
                confirm_letter(letter_possibilities, i, letter);
            } else if (type == YELLOW) {
                word_counts[index]++;
                letter_possibilities[i][index] = false;
            }
        }
        //Loop over guess again and check greys
        for (int i = 0; i < WORD_LEN; i++) {
            type = line[2 * i];
            letter = line[2 * i + 1]; 
            index = letter - 'a';

            if (type != GREEN && type != YELLOW) {
                // We know there are exactly word_counts occurances of that character in final word
                counts[index].max = word_counts[index];
                counts[index].min = word_counts[index];

                if (word_counts[index] == 0) {
                    remove_letter(letter_possibilities, letter);
                } else {
                    letter_possibilities[i][index] = false;
                }
            }
        }

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (counts[i].min < word_counts[i]) {
                counts[i].min = word_counts[i];
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

void remove_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], char letter) {
    for (int i = 0; i < WORD_LEN; i++) {
        letter_possibilities[i][letter - 'a'] = false;
    }
}

void find_possible_words(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
) {
    Trie allowed_guesses = create_node();
    Trie allowed_answers = create_node();

    load_file(allowed_guesses, "wordle_allowed_guesses.txt", WORD_LEN);
    load_file(allowed_answers, "wordle_allowed_answers.txt", WORD_LEN);

    char current_word[WORD_LEN + 1];

    printf("Possible guesses: \n");
    print_matching_words(allowed_guesses, 0, current_word, letter_possibilities, counts);

    printf("Possible answers: \n");
    print_matching_words(allowed_answers, 0, current_word, letter_possibilities, counts);
}

void print_matching_words(
    Trie t, int depth, char *current_word,     
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
) {
    if (t == NULL) return;

    if (t->is_end_of_word) {
        //Need to check it meets counts requirements still
        int word_counts[ALPHABET_SIZE] = {0};
        for (int i = 0; i < WORD_LEN; i++) {
            word_counts[current_word[i] - 'a']++;
        }
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (word_counts[i] < counts[i].min || word_counts[i] > counts[i].max) {
                return;
            }
        }
        current_word[WORD_LEN] = '\0';
        printf("%s\n", current_word);
        return;
    }

    char letter;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        letter = i + 'a';
        if (letter_possibilities[depth][i] && t->letters[i] != NULL) {
            current_word[depth] = letter;
            print_matching_words(
                t->letters[i], depth + 1, current_word, 
                letter_possibilities, counts
            );
        }
    }
    return;
}


int max(int i, int j) {
    if (i >= j) return i;
    return j;
}

int min(int i, int j) {
    if (i <= j) return i;
    return j;
}

