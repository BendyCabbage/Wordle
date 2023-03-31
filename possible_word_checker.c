#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define GUESS_LEN 11
#define WORD_LEN 5

void check_answers_file(bool letter_possibilities[WORD_LEN][26]);
void scan_guesses(bool letter_possibilities[WORD_LEN][26], int *num_yellows, char *yellows);

void confirm_letter(bool letter_possibilities[WORD_LEN][26], int letter_pos, char confirmed_letter);
void append_yellows(int *num_yellows, char *yellows, char new_yellow);
void remove_yellows(int *num_yellows, char *yellows, char yellow_to_remove);

void find_matches(bool letter_possibilities[WORD_LEN][26], int num_yellows, char *yellows, FILE *word_file);
bool word_contains_char(char *word, char c);

int main (int argc, char *argv[]) {
    //Initialising letter possibilities array
    bool letter_possibilities[WORD_LEN][26];
    for (int i = 0; i < WORD_LEN; i++) {
        for (int j = 0; j < 26; j++) {
            letter_possibilities[i][j] = true;
        }
    }

    int num_yellows = 0;
    char yellows[5];

    scan_guesses(letter_possibilities, &num_yellows, yellows);

    FILE *guesses_file = fopen("wordle_allowed_guesses.txt", "r");
    FILE *answers_file = fopen("wordle_allowed_answers.txt", "r");

    printf("Possible guesses: \n");
    find_matches(letter_possibilities, num_yellows, yellows, guesses_file);
    
    printf("Possible answers: \n");
    find_matches(letter_possibilities, num_yellows, yellows, answers_file);

    return 0;
}

/*Guesses scanned in as:
- for green letters, type g and then the letter
- for yellow letters, type y and then the letter
- for grey letters, type - and then the letter
If the correct word was sport and you guessed spray, enter:
gsgpyr-a-y
*/
void scan_guesses(bool letter_possibilities[WORD_LEN][26], int *num_yellows, char *yellows) {
    printf("Enter your guesses: \n");
    char guess_type;
    char guess_char;

    char guess_line[11];
    while (fgets(guess_line, 11, stdin) != NULL) {
        for (int i = 0; i < WORD_LEN; i++) {
            guess_type = guess_line[2 * i];
            guess_char = guess_line[2 * i + 1];

            if (guess_type == 'g') {
                confirm_letter(letter_possibilities, i, guess_char);
                remove_yellows(num_yellows, yellows, guess_char);
            } else if (guess_type == 'y') {
                //Character is not in this position, add to yellows
                letter_possibilities[i][guess_char - 'a'] = false;
                append_yellows(num_yellows, yellows, guess_char);
            } else {
                //Setting all positions of the letter to false
                for (int j = 0; j < WORD_LEN; j++) {
                    letter_possibilities[j][guess_char - 'a'] = false;
                }
            }
        }
        //Removing trailing newline character
        scanf("%c", &guess_char);
    }
}

void confirm_letter(bool letter_possibilities[WORD_LEN][26], int letter_pos, char confirmed_letter) {
    for (int i = 0; i < 26; i++) {
        letter_possibilities[letter_pos][i] = false;
    }
    letter_possibilities[letter_pos][confirmed_letter - 'a'] = true;
}

void append_yellows(int *num_yellows, char *yellows, char new_yellow) {
    for (int i = 0; i < *num_yellows; i++) {
        if (yellows[i] == new_yellow) return;
    }
    yellows[*num_yellows] = new_yellow;
    (*num_yellows)++;
}

void remove_yellows(int *num_yellows, char *yellows, char yellow_to_remove) {
    int i;
    for (i = 0; i < *num_yellows; i++) {
        if (yellows[i] == yellow_to_remove) break;
    }
    if (i == *num_yellows) return;

    //Move all entries after this yellow down, reduce num_yellows by 1
    for (i = i; i < *num_yellows - 1; i++) {
        yellows[i] = yellows[i + 1];
    }
    (*num_yellows)--;
}

void find_matches(bool letter_possibilities[WORD_LEN][26], int num_yellows, char *yellows, FILE *word_file) {
    if (word_file == NULL) {
        fprintf(stderr, "Cannot open file\n");
        exit(EXIT_FAILURE);
    }

    int total_matches = 0;

    char current_word[6];
    char word_copy[6];
    char buffer;
    while (fgets(current_word, 6, word_file) != NULL) {
        strcpy(word_copy, current_word);

        int i;
        for (i = 0; i < WORD_LEN; i++) {
            if (!letter_possibilities[i][current_word[i] - 'a']) break;
        }
        if (i == WORD_LEN) {
            //Need to check if word contains all yellows
            int j;
            for (j = 0; j < num_yellows; j++) {
                if (!word_contains_char(word_copy, yellows[j])) break;
            }
            if (j == num_yellows) {
                printf("%s\n", current_word);
                total_matches++;
            }
        }
        //Removing trailing newline
        fscanf(word_file, "%c", &buffer);
    }
    printf("Total number of matches: %d\n", total_matches);
}

bool word_contains_char(char *word, char c) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] == c) {
            word[i] = '_';
            return true;
        }
    }
    return false;
}

void check_answers_file(bool letter_possibilities[WORD_LEN][26]) {
    FILE *answers_file = fopen("wordle_allowed_answers.txt", "r");
    if (answers_file == NULL) {
        fprintf(stderr, "Cannot open answers file\n");
        exit(EXIT_FAILURE);
    }
    char current_word[6];
    char buffer;

    //Checking each word in answers_file
    while (fgets(current_word, 6, answers_file) != NULL) {
        int i;
        for (i = 0; i < WORD_LEN; i++) {
            if (!letter_possibilities[i][current_word[i] - 'a']) break;
        }
        if (i == WORD_LEN) {
            printf("%s\n", current_word);
        }
        //Removing trailing newline
        fscanf(answers_file, "%c", &buffer);
    }
}
