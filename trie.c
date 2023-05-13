#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "trie.h"

#define MAX_WORD_LENGTH 128

Trie create_node() {
    Trie new_node = malloc(sizeof(struct trie_node));
    new_node->is_end_of_word = false;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        new_node->letters[i] = NULL;
    }
    return new_node;
}

int insert(Trie root, char *word) {
    Trie current = root;

    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';

        if (current->letters[index] == NULL) {
            current->letters[index] = create_node();
        }
        current = current->letters[index];
    }
    current->is_end_of_word = true;
    return 0;
}

bool search(Trie root, const char *word) {
    Trie current = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';

        if (current->letters[index] == NULL) {
            return false;
        }
        current = current->letters[index];
    }
    return current != NULL && current->is_end_of_word;
}

void print_all_words(Trie current, char *current_word) {
    if (current == NULL) return;
    if (current->is_end_of_word) {
        printf("%s\n", current_word);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (current->letters[i] != NULL) {
            char *new_word = malloc((strlen(current_word) + 2) * sizeof(char));
            char letter = i + 'a';
            strcpy(new_word, current_word);

            new_word[strlen(current_word)] = letter;
            new_word[strlen(current_word) + 1] = '\0';
            
            print_all_words(current->letters[i], new_word);
            free(new_word);
        }
    }
    return;
}

int load_file(Trie t, char *file_name, int max_word_length) {
    FILE *words_file = fopen(file_name, "r");
    char *word = malloc(sizeof(max_word_length + 1) * sizeof(char));

    if (words_file == NULL) {
        fprintf(stderr, "Unable to open words file\n");
        return -1;
    }
    if (word == NULL) {
        fprintf(stderr, "No memory\n");
        return -1;
    }

    while (fgets(word, max_word_length + 1, words_file) != NULL) {
        if (word[0] != '\n') {
            insert(t, word);
        }
    }
    fclose(words_file);
    return 0;
}

void destroy(Trie t) {
    if (t == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        destroy(t->letters[i]);
    }
    free(t);
}


