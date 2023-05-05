#define ALPHABET_SIZE 26

typedef struct trie_node *Trie;
struct trie_node {
    Trie letters[ALPHABET_SIZE];
    bool is_end_of_word;
};

Trie create_node();
int insert(Trie root, char *word);
bool search(Trie root, const char *word);
void print_all_words(Trie current, char *current_word);
int load_file(Trie t, char *file_name, int max_word_length);