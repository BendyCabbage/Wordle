#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "trie.c"
#include "wordle_solver.h"

char OUTPUT_GUESSES_FILE[] = "guesses.txt";
char OUTPUT_ANSWERS_FILE[] = "answers.txt";

FILE *output_file;
char *word_list;

/*
Current todo list:
-   Modify scan guesses to have 2 separate functions, process_guesses() which calls scan_guesses()
    Idea is to be able to input an already scanned in string into process_guesses() so that this file never needs to scan input

-   Input to solve being if answers and/or guesses files are being checked and appended to.
*/

int solve(int num_guesses, char **guesses, char *output_filename, char *word_list_name) {
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE];
    struct letter_counts counts[ALPHABET_SIZE];

    printf("NUM GUESSES: %d\n", num_guesses);

    for (int i = 0; i < num_guesses; i++) {
        printf("Guess %d: %s\n", i, guesses[i]);
    }
    printf("WHAT\n");

    if (init(letter_possibilities, counts, output_filename, word_list_name) != 0) {
        fprintf(stderr, "Initialisation failed. Check input, output and word list names are correct\n");
        return 0;
    }
    printf("Finished init\n");

    scan_guesses(num_guesses, guesses, letter_possibilities, counts);
    int num_possible_words = find_possible_words(letter_possibilities, counts);

    fclose(output_file);

    return num_possible_words;
}

int init(
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE],
    char *output_filename,
    char *word_list_name
) {
    printf("Start of init\n");
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        counts[i].min = 0;
        counts[i].max = 3;

        for (int j = 0; j < WORD_LEN; j++) {
            letter_possibilities[j][i] = true;
        }
    }

    if (output_filename == NULL) {
        output_file = fopen(DEFAULT_OUTPUT_FILE, "w");
    } else {
        output_file = fopen(output_filename, "w");
    }

    if (word_list_name == NULL) {
        word_list = DEFAULT_WORD_LIST;
    } else {
        word_list = word_list_name;
    }

    if (output_file == NULL || word_list == NULL) {
        return -1;
    }
    printf("End of init\n");
    return 0;
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
    int num_guesses, char **guesses,
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE],
    struct letter_counts counts[ALPHABET_SIZE]
) {
    if (num_guesses <= 0) return;

    char type, letter;
    char line[LINE_LEN];
    int index;

    int word_counts[ALPHABET_SIZE];

    for (int i = 0; i < num_guesses; i++) {
        printf("Processing %s\n", guesses[i]);

        for (int i = 0; i < ALPHABET_SIZE; i++) {
            word_counts[i] = 0;
        }

        //Loop over guess and check yellows and greens
        for (int i = 0; i < WORD_LEN; i++) {
            type = guesses[i][2 * i];
            letter = guesses[i][2 * i + 1]; 
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
            type = guesses[i][2 * i];
            letter = guesses[i][2 * i + 1]; 
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

int find_possible_words(    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
) {
    Trie allowed_words = create_node();
    load_file(allowed_words, word_list, WORD_LEN);

    char current_word[WORD_LEN + 1];
    int num_words = output_matching_words(
        allowed_words, 0, current_word, 
        letter_possibilities, counts, 
        output_file
    );
    destroy(allowed_words);
    return num_words;
}

int output_matching_words(
    Trie t, int depth, char *current_word,    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE], FILE *output_file
) {
    if (t == NULL) return 0;

    if (t->is_end_of_word) {
        int word_counts[ALPHABET_SIZE] = {0};
        for (int i = 0; i < WORD_LEN; i++) {
            word_counts[current_word[i] - 'a']++;
        }
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (word_counts[i] < counts[i].min || word_counts[i] > counts[i].max) {
                return 0;
            }
        }
        current_word[WORD_LEN] = '\0';
        fprintf(output_file, "%s\n", current_word);
        return 1;
    }
    int word_count = 0;
    char letter;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        letter = i + 'a';
        if (letter_possibilities[depth][i] && t->letters[i] != NULL) {
            current_word[depth] = letter;
            word_count += output_matching_words(
                t->letters[i], depth + 1, current_word, 
                letter_possibilities, counts, output_file
            );
        }
    }
    return word_count;
}


int max(int i, int j) {
    if (i >= j) return i;
    return j;
}

int min(int i, int j) {
    if (i <= j) return i;
    return j;
}

