#include <stdio.h>
#include <stdlib.h>

#include "evaluation.c"
#include "wordle_solver.c"

#define ANSWERS_FILE "wordle_allowed_answers.txt"
#define GUESSES_FILE "wordle_allowed_guesses.txt"
#define COMBINED_FILE "wordle_combined_words.txt"

#define DEFAULT_INPUT_FILE "default_input.txt"

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

int main (int argc, char **argv) {
    char evaluation[2 * WORD_LEN + 1];

    int num_initial_guesses = solve(DEFAULT_INPUT_FILE, "initial_guesses.txt", GUESSES_FILE);
    int num_initial_answers = solve(DEFAULT_INPUT_FILE, "initial_answers.txt", ANSWERS_FILE);

    printf("Num guesses: %d\n", num_initial_guesses);
    printf("Num initial answers: %d\n", num_initial_answers);

    if (num_initial_answers <= 0) {
        fprintf(stderr, "No answers match, ensure input is valid\n");
        return -1;
    }



    return 0;
}