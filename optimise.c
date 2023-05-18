#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int is_answer;
};

void init_guesses(char *filename, struct guess_score *guesses, int offset, int is_answer);
int sort_compare(const void *guess1, const void *guess2);

int main (int argc, char **argv) {
    int num_guesses = argc - 1;
    char guesses[num_guesses + 1][LINE_LEN];

    for (int i = 0; i < num_guesses; i++) {
        strcpy(guesses[i], argv[i + 1]);
    }

    Trie allowed_guesses = create_node();
    Trie allowed_answers = create_node();

    load_file(allowed_guesses, GUESSES_FILE, WORD_LEN);
    load_file(allowed_answers, ANSWERS_FILE, WORD_LEN);

    int num_initial_guesses = solve(num_guesses, guesses, "initial_guesses.txt", allowed_guesses);
    int num_initial_answers = solve(num_guesses, guesses, "initial_answers.txt", allowed_answers);

    printf("Num guesses: %d\n", num_initial_guesses);
    printf("Num initial answers: %d\n", num_initial_answers);

    if (num_initial_answers <= 0) {
        fprintf(stderr, "No answers match, ensure input is valid\n");
        return -1;
    }

    struct guess_score gs[num_initial_answers + num_initial_guesses];
    init_guesses("initial_answers.txt", gs, 0, true);
    init_guesses("initial_guesses.txt", gs, num_initial_answers, false);

    //List of answers = gs[0..num_initial_answers - 1].guess
    //List of guesses = gs[num_initial_answers..num_initial_answers + num_initial_guesses - 1].guess


    //For every answer
    for (int i = 0; i < num_initial_answers; i++) {
        //For every possible guess
        for (int j = 0; j < num_initial_answers + num_initial_guesses; j++) {
            //Answer = gs[i].guess
            //Guess = gs[j].guess
            char evaluation[2 * WORD_LEN + 1];
            evaluate(gs[i].guess, gs[j].guess, evaluation);
            strcpy(guesses[num_guesses], evaluation);
            int remaining_answers = solve(num_guesses + 1, guesses, "answers.txt", allowed_answers);
            gs[j].sum_remaining_answers += remaining_answers;
        }
    }
    qsort(gs, num_initial_answers + num_initial_guesses, sizeof(struct guess_score), sort_compare);
    for (int i = 0; i < num_initial_answers + num_initial_guesses; i++) {
        if (gs[i].sum_remaining_answers <= 0) continue;
        printf("Guess: %s, sum: %d, is answer: %d\n", gs[i].guess, gs[i].sum_remaining_answers, gs[i].is_answer);
    }

    return 0;
}

void init_guesses(char *filename, struct guess_score *guesses, int offset, int is_answer) {
    FILE *words = fopen(filename, "r");
    char line[WORD_LEN + 1];
    int index = offset;
    while (fread(line, sizeof(char), WORD_LEN + 1, words) > 0) {
        line[WORD_LEN] = '\0';
        strcpy(guesses[index].guess, line);
        guesses[index].sum_remaining_answers = 0;
        guesses[index].is_answer = is_answer;
        index++;
    }
    fclose(words);
}

int sort_compare(const void *guess1, const void *guess2) {
    struct guess_score g1 = *(struct guess_score *)guess1;
    struct guess_score g2 = *(struct guess_score *)guess2;

    if (g1.sum_remaining_answers != g2.sum_remaining_answers) {
        return g1.sum_remaining_answers - g2.sum_remaining_answers;
    }
    if (g1.is_answer != g2.is_answer) {
        return g2.is_answer - g1.is_answer;
    }

    return strcmp(g1.guess, g2.guess);
}