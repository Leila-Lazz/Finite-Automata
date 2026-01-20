#include "library.h"
#include <stdio.h>
#include <string.h>

int main() {
    while (1) {
        int choice;
        int modified;
        printf("Enter the number of the automata between 1 and 44 : ");
        scanf("%d", &choice);
        if (choice == 0) break;

        char filename[50];
        sprintf(filename, "automates/automate_%02d.txt", choice);

        Automaton A = read_automaton(filename);
        display_automaton(A);       
        printf("\n");

        // Verify if the automata is standard
        if (!is_standard(A)) {
            char response;
            printf("The automaton is not standard. Do you want to standardize it? (y/n): ");
            scanf(" %c", &response);
            if (response == 'y' || response == 'Y') {
                A = standardize(A);
                printf("Automaton standardized.\n");
                display_automaton(A);
            } else {
                printf("Automaton kept as is.\n");
            }
        }

         // Vérifier si l'automate est déterministe
    if (is_deterministic(A)) {
        printf("The automata is deterministic!\n");
    } else {
        printf("The automata isn't deterministic!\n");
        // Effectuer la déterminisation
        A = determinize_and_complete(A);
        modified = 1;  // Marquer qu'un changement a eu lieu
    }

    // Vérifier si l'automate est complet
    if (is_complete(A)) {
        printf("The automata is complete!\n");
    } else {
        printf("The automata isn't complete!\n");
        // Effectuer la complétude
        A = determinize_and_complete(A);  // Complète si nécessaire
        modified = 1;  // Marquer qu'un changement a eu lieu
    }

    // Si l'automate a été modifié, l'afficher
    if (modified) {
        printf("The automaton has been modified. Here is the updated automaton:\n");
        display_automaton(A);  // Affiche seulement si modifié
        printf("\n");
    }

        // Apply the minimization of the automata
        A = minimize_automaton(A);
        display_minimal_automaton(A);
        printf("\n");

        // Apply the construction of the complementary automata
        A = complementary_automaton(A);
        display_automaton(A);
        printf("\n");

        // Loop to recognize words
        char word[50];
        printf("Enter a word to test : ");
        while (1) {
            scanf("%s", word);
            if (strcmp(word, "end") == 0) break;
            printf("The word %s is %s by the automata !\n", word, recognize_word(A, word) ? "recognized" : "not recognized");
        }
    }
    return 0;
}
