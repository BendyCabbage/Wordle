#define WORD_LEN 5
#define LINE_LEN 2 * WORD_LEN + 1
#define ALPHABET_SIZE 26

#define GREEN 'g'
#define YELLOW 'y'

#define DEFAULT_OUTPUT_FILE "possible_words.txt"
#define DEFAULT_WORD_LIST "wordle_allowed_answers.txt"

struct letter_counts {
    int min;
    int max;
};

int min(int i, int j);
int max(int i, int j);

int init(
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE],
    char *output_filename
);

void scan_guesses(  
    int num_guesses, char guesses[num_guesses][LINE_LEN],  
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE],
    struct letter_counts counts[ALPHABET_SIZE]
);

void confirm_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], int letter_pos, char letter);
void remove_letter(bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], char letter);

int find_possible_words(    
    Trie allowed_words,
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE]
);

int output_matching_words(
    Trie t, int depth, char *current_word,    
    bool letter_possibilities[WORD_LEN][ALPHABET_SIZE], 
    struct letter_counts counts[ALPHABET_SIZE], 
    FILE *output_file
);

int solve(
    int num_guesses, char guesses[num_guesses][LINE_LEN], 
    char *output_filename,
    Trie allowed_words
);