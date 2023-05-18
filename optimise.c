#include <stdio.h>
#include <stdlib.h>

#include "evaluation.c"
#include "wordle_solver.c"

#define ANSWERS_FILE "wordle_allowed_answers.txt"
#define GUESSES_FILE "wordle_allowed_guesses.txt"
#define COMBINED_FILE "wordle_combined_words.txt"

#define DEFAULT_INPUT_FILE "input.txt"

#define MAX_GUESSES 6
/*
int solve(char *input_filename, char *output_filename, char *word_list_name);

Overall steps for finding best possible next guess:
1. Find all possible remaining answers + guesses
2. 
    For each possible remaining answer:
        For each possible guess:
            evaluate(answer, guess) and strcat(existing guesses, evaluation)
            solve(added guesses), count number of remaining answers
    sort all (guess, count of remaining answers) in descending order
    pick first one

*/

struct guess_score {
    char guess[WORD_LEN + 1];
    int sum_remaining_answers;
};

void init_guesses(char *filename, struct guess_score *guesses, int offset);

int main (int argc, char **argv) {
    int num_guesses = argc - 1;
    char guesses[num_guesses][LINE_LEN];

    for (int i = 0; i < num_guesses; i++) {
        strcpy(guesses[i], argv[i + 1]);
    }

    int num_initial_guesses = solve(num_guesses, guesses, "initial_guesses.txt", GUESSES_FILE);
    int num_initial_answers = solve(num_guesses, guesses, "initial_answers.txt", ANSWERS_FILE);

    printf("Num guesses: %d\n", num_initial_guesses);
    printf("Num initial answers: %d\n", num_initial_answers);

    if (num_initial_answers <= 0) {
        fprintf(stderr, "No answers match, ensure input is valid\n");
        return -1;
    }

    struct guess_score gs[num_initial_answers + num_initial_guesses];
    init_guesses("initial_answers.txt", gs, 0);
    init_guesses("initial_guesses.txt", gs, num_initial_answers);

    return 0;
}

void init_guesses(char *filename, struct guess_score *guesses, int offset) {
    FILE *words = fopen(filename, "r");
    char line[WORD_LEN + 1];
    int index = offset;
    while (fread(line, sizeof(char), WORD_LEN + 1, words) > 0) {
        line[WORD_LEN] = '\0';
        strcpy(guesses[index].guess, line);
        guesses[index].sum_remaining_answers = 0;
        index++;
    }

    fclose(words);
}