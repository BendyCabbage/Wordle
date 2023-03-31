#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "trie.h"

int main(int argc, char *argv) {
    Trie t = create_node();

    insert(t, "hi");
    insert(t, "hello");
    insert(t, "hey");
    printf("Search for dog: %d\n", search(t, "dog"));

    print_all_words(t, "");

    return 0;
}

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

        if (current->letters[i] == NULL) {
            return false;
        }
        current = current->letters[i];
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


