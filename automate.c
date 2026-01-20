#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STATES 100
#define MAX_SYMBOLS 26
#define MAX_TRANSITIONS 200

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

// Lire un automate depuis un fichier
Automaton read_automaton(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    Automaton A;
    fscanf(file, "%d", &A.num_symbols);
    fscanf(file, "%d", &A.num_states);

    fscanf(file, "%d", &A.num_initial_states);
    for (int i = 0; i < A.num_initial_states; i++)
        fscanf(file, "%d", &A.initial_states[i]);

    fscanf(file, "%d", &A.num_final_states);
    for (int i = 0; i < A.num_final_states; i++)
        fscanf(file, "%d", &A.final_states[i]);

    fscanf(file, "%d", &A.num_transitions);
    for (int i = 0; i < A.num_transitions; i++) {
        fscanf(file, "%d %c %d", &A.transitions[i].source, &A.transitions[i].symbol, &A.transitions[i].target);
    }

    fclose(file);
    return A;
}

// Affichage d'un automate
void display_automaton(Automaton A) {
    printf("Nombre de symboles: %d\n", A.num_symbols);
    printf("Nombre d'etats: %d\n", A.num_states);
    printf("Etats initiaux: ");
    for (int i = 0; i < A.num_initial_states; i++)
        printf("%d ", A.initial_states[i]);
    printf("\nEtats finaux: ");
    for (int i = 0; i < A.num_final_states; i++)
        printf("%d ", A.final_states[i]);
    printf("\nTransitions:\n");
    for (int i = 0; i < A.num_transitions; i++)
        printf("%d --%c--> %d\n", A.transitions[i].source, A.transitions[i].symbol, A.transitions[i].target);
}

// Déterminisation de l'automate
Automaton determinize(Automaton A) {
    // Implémentation simplifiée pour la déterminisation
    Automaton D = A; // Ici, une vraie implémentation de déterminisation serait faite
    return D;
}

// Minimisation de l'automate
Automaton minimize(Automaton A) {
    // Implémentation simplifiée pour la minimisation
    Automaton M = A; // Ici, une vraie implémentation de minimisation serait faite
    return M;
}

// Reconnaissance d'un mot
int recognize_word(Automaton A, const char *word) {
    int current_state = A.initial_states[0];
    for (int i = 0; word[i] != '\0'; i++) {
        char symbol = word[i];
        int found = 0;
        for (int j = 0; j < A.num_transitions; j++) {
            if (A.transitions[j].source == current_state && A.transitions[j].symbol == symbol) {
                current_state = A.transitions[j].target;
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    for (int i = 0; i < A.num_final_states; i++) {
        if (current_state == A.final_states[i]) return 1;
    }
    return 0;
}

// Génération de l'automate complémentaire
Automaton complement(Automaton A) {
    Automaton CompA = A;
    int new_final_states[MAX_STATES];
    int count = 0;
    for (int i = 0; i < A.num_states; i++) {
        int is_final = 0;
        for (int j = 0; j < A.num_final_states; j++) {
            if (i == A.final_states[j]) {
                is_final = 1;
                break;
            }
        }
        if (!is_final) new_final_states[count++] = i;
    }
    CompA.num_final_states = count;
    memcpy(CompA.final_states, new_final_states, count * sizeof(int));
    return CompA;
}

int is_deterministic(Automaton A);
int is_complete(Automaton A);

// Vérifie si un automate est déterministe
int is_deterministic(Automaton A) {
    for (int i = 0; i < A.num_transitions - 1; i++) {
        for (int j = i + 1; j < A.num_transitions; j++) {
            if (A.transitions[i].source == A.transitions[j].source && A.transitions[i].symbol == A.transitions[j].symbol) {
                return 0; // Pas déterministe
            }
        }
    }
    return 1; // Déterministe
}

// Vérifie si un automate est complet
int is_complete(Automaton A) {
    int transitions[A.num_states][A.num_symbols];
    memset(transitions, 0, sizeof(transitions));

    for (int i = 0; i < A.num_transitions; i++) {
        transitions[A.transitions[i].source][A.transitions[i].symbol - 'a'] = 1;
    }

    for (int i = 0; i < A.num_states; i++) {
        for (int j = 0; j < A.num_symbols; j++) {
            if (!transitions[i][j]) return 0; // Pas complet
        }
    }
    return 1; // Complet
}

int main() {
    char filename[50];
    printf("Entrez le nom du fichier contenant l'automate: ");
    scanf("%s", filename);
    
    Automaton A = read_automaton(filename);
    display_automaton(A);
    
    if (is_deterministic(A))
        printf("L'automate est deterministe.\n");
    else
        printf("L'automate n'est pas deterministe.\n");
    
    if (is_complete(A))
        printf("L'automate est complet.\n");
    else
        printf("L'automate n'est pas complet.\n");
    
    A = determinize(A);
    A = minimize(A);
    
    char word[50];
    printf("Entrez un mot a tester: ");
    scanf("%s", word);
    if (recognize_word(A, word))
        printf("Le mot est reconnu.\n");
    else
        printf("Le mot n'est pas reconnu.\n");
    
    A = complement(A);
    printf("Automate complementaire :\n");
    display_automaton(A);
    
    return 0;
}
