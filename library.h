#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 100
#define MAX_SYMBOLS 26
#define MAX_TRANSITIONS 200

// Définition des structures
typedef struct {
    int source;
    char symbol;
    int target;
} Transition;

typedef struct {
    int num_symbols;
    int num_states;
    int num_initial_states;
    int initial_states[MAX_STATES];
    int num_final_states;
    int final_states[MAX_STATES];
    int num_transitions;
    Transition transitions[MAX_TRANSITIONS];
} Automaton;

// Prototypes des fonctions
Automaton read_automaton(const char *filename);
void display_automaton(Automaton A);
int is_standard(Automaton A);
Automaton standardize(Automaton A);
// Déclaration de la fonction is_deterministic
int is_deterministic(Automaton A);

// Déclaration de la fonction is_complete
int is_complete(Automaton A);

Automaton determinize_and_complete(Automaton A);
Automaton minimize_automaton(Automaton A);
Automaton complementary_automaton(Automaton A);
void display_minimal_automaton(Automaton A);
int is_final(Automaton A, int state);
int recognize_word(Automaton A, const char *word);

#endif // LIBRARY_H
