#include "library.h"

// Read an automata from a file
Automaton read_automaton(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error when opening the file");
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

// Display the automata / Cette fonction n'a pas de bug
void display_automaton(Automaton A) {
    printf("Number of symbols : %d\n", A.num_symbols);
    printf("Number of states : %d\n", A.num_states);
    printf("Initial state(s) : ");
    for (int i = 0; i < A.num_initial_states; i++)
        printf("%d ", A.initial_states[i]);
    printf("\nNumber corresponding to the final state(s) : ");
    for (int i = 0; i < A.num_final_states; i++)
        printf("%d ", A.final_states[i]);
    printf("\nTransitions:\n");
    for (int i = 0; i < A.num_transitions; i++)
        printf("%d --%c--> %d\n", A.transitions[i].source, A.transitions[i].symbol, A.transitions[i].target);
}

// Standardize
int is_standard(Automaton A) {
    for (int i = 0; i < A.num_initial_states; i++) {
        if (A.initial_states[i] != 0) return 0;
    }
    return 1;
}

Automaton standardize(Automaton A) {
    if (is_standard(A)) return A;
    printf("Standardizing...\n");
    Automaton SFA = A;
    SFA.initial_states[0] = 0;
    SFA.num_initial_states = 1;
    return SFA;
}

// Verify is the automata is determinsitic
int is_deterministic(Automaton A) {
    for (int i = 0; i < A.num_transitions; i++) {
        for (int j = i + 1; j < A.num_transitions; j++) {
            if (A.transitions[i].source == A.transitions[j].source && A.transitions[i].symbol == A.transitions[j].symbol) {
                return 0;  // ~no
            }
        }
    }
    return 1; // ~yes
}

// Verify ig the automata is complete or not / revoir la deuxieme partie de cette focntion
int is_complete(Automaton A) {
    int transitions[A.num_states][A.num_symbols];
    memset(transitions, 0, sizeof(transitions));

    for (int i = 0; i < A.num_transitions; i++) {
        transitions[A.transitions[i].source][A.transitions[i].symbol - 'a'] = 1;
    }

    // verify id each state has a transition for each symbol of the alphabet / pas sur si c'est la bonne condition
    for (int i = 0; i < A.num_states; i++) {
        for (int j = 0; j < A.num_symbols; j++) {
            if (!transitions[i][j]) {
                return 0; // Not complete
            }
        }
    }
    return 1; // Complete
}

Automaton determinize_and_complete(Automaton A) {
    printf("Determinizing and completing the automaton...\n");

    Automaton CDFA;
    CDFA.num_symbols = A.num_symbols;
    CDFA.num_states = 0; // Will be set dynamically
    CDFA.num_initial_states = 1;
    CDFA.initial_states[0] = 0; // Start with the initial state of the NFA
    CDFA.num_final_states = 0;

    // 1. Powerset construction
    // Map NFA states to DFA states (powerset of NFA states)
    int state_map[MAX_STATES] = {0}; // To keep track of new DFA states
    int state_set[MAX_STATES][MAX_STATES]; // Each element holds a set of NFA states
    int new_state_count = 1;
    
    // Initially, the DFA's initial state corresponds to the NFA's initial state
    state_set[0][0] = A.initial_states[0]; // Start with the initial state of NFA
    state_map[0] = 1;

    // Create new transitions for the DFA
    for (int i = 0; i < new_state_count; i++) {
        for (int symbol = 0; symbol < A.num_symbols; symbol++) {
            int target_set[MAX_STATES] = {0}; // This stores the new set of states after the transition
            int target_count = 0;

            // For each state in the current set of NFA states
            for (int j = 0; j < MAX_STATES && state_set[i][j] != 0; j++) {
                int current_state = state_set[i][j];
                
                // Find all transitions from this state for the given symbol
                for (int t = 0; t < A.num_transitions; t++) {
                    if (A.transitions[t].source == current_state && A.transitions[t].symbol == 'a' + symbol) {
                        int target_state = A.transitions[t].target;
                        if (!target_set[target_count]) {
                            target_set[target_count++] = target_state;
                        }
                    }
                }
            }

            if (target_count > 0) {
                // Add the new state to the DFA if it's not already present
                int target_state = target_set[0];
                for (int k = 1; k < target_count; k++) {
                    target_state |= target_set[k]; // Combine states (set union)
                }

                int new_state_index = -1;
                for (int k = 0; k < new_state_count; k++) {
                    if (state_set[k][0] == target_state) {
                        new_state_index = k;
                        break;
                    }
                }

                if (new_state_index == -1) {
                    state_set[new_state_count][0] = target_state;
                    new_state_index = new_state_count++;
                }

                // Add the transition from the current state to the new state
                CDFA.transitions[CDFA.num_transitions].source = i;
                CDFA.transitions[CDFA.num_transitions].symbol = 'a' + symbol;
                CDFA.transitions[CDFA.num_transitions].target = new_state_index;
                CDFA.num_transitions++;
            }
        }
    }

    // 2. Completeness
    // Ensure all states in the DFA have transitions for each symbol
    for (int i = 0; i < new_state_count; i++) {
        for (int symbol = 0; symbol < A.num_symbols; symbol++) {
            int found = 0;
            for (int t = 0; t < CDFA.num_transitions; t++) {
                if (CDFA.transitions[t].source == i && CDFA.transitions[t].symbol == 'a' + symbol) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                // Add a transition to the "sink" state if no transition is found
                int sink_state = CDFA.num_states;
                CDFA.transitions[CDFA.num_transitions].source = i;
                CDFA.transitions[CDFA.num_transitions].symbol = 'a' + symbol;
                CDFA.transitions[CDFA.num_transitions].target = sink_state;
                CDFA.num_transitions++;
            }
        }
    }

    // 3. Final States (all NFA final states should be final in the DFA)
    for (int i = 0; i < new_state_count; i++) {
        for (int j = 0; j < A.num_final_states; j++) {
            if (state_set[i][0] == A.final_states[j]) {
                CDFA.final_states[CDFA.num_final_states++] = i;
                break;
            }
        }
    }

    // 4. Set the number of states in the DFA
    CDFA.num_states = new_state_count;

    return CDFA;
}

// Minimization of an automata
Automaton minimize_automaton(Automaton A) {
    printf("Minimisation of the automata...\n");

    // Étape 1 : Créer une table de partition
    int partition[A.num_states][A.num_states];
    for (int i = 0; i < A.num_states; i++) {
        for (int j = 0; j < A.num_states; j++) {
            partition[i][j] = 0;  // Initialisation à 0
        }
    }

    // Étape 2 : Initialiser les partitions en fonction des états finaux et non finaux
    for (int i = 0; i < A.num_states; i++) {
        for (int j = i + 1; j < A.num_states; j++) {
            if ((is_final(A, i) && !is_final(A, j)) || (!is_final(A, i) && is_final(A, j))) {
                partition[i][j] = 1;
                partition[j][i] = 1;
            }
        }
    }

    // Étape 3 : Refiner la partition en fonction des transitions
    int refined = 1;
    while (refined) {
        refined = 0;
        for (int i = 0; i < A.num_states; i++) {
            for (int j = i + 1; j < A.num_states; j++) {
                if (partition[i][j] == 0) {
                    // Vérifier si les transitions de i et j sont différentes pour tous les symboles
                    for (int symbol = 0; symbol < A.num_symbols; symbol++) {
                        int target_i = -1, target_j = -1;
                        for (int t = 0; t < A.num_transitions; t++) {
                            if (A.transitions[t].source == i && A.transitions[t].symbol == 'a' + symbol) {
                                target_i = A.transitions[t].target;
                            }
                            if (A.transitions[t].source == j && A.transitions[t].symbol == 'a' + symbol) {
                                target_j = A.transitions[t].target;
                            }
                        }

                        if (target_i != target_j) {
                            partition[i][j] = 1;
                            partition[j][i] = 1;
                            refined = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Étape 4 : Créer les nouveaux états minimisés
    Automaton minimized_automaton;
    minimized_automaton.num_states = 0;
    minimized_automaton.num_symbols = A.num_symbols;
    minimized_automaton.num_transitions = 0;
    minimized_automaton.num_final_states = 0;
    minimized_automaton.num_initial_states = 1;
    minimized_automaton.initial_states[0] = 0;

    int state_map[A.num_states];
    for (int i = 0; i < A.num_states; i++) {
        state_map[i] = -1;
    }

    // Créer les nouveaux états minimisés
    for (int i = 0; i < A.num_states; i++) {
        if (state_map[i] == -1) {
            state_map[i] = minimized_automaton.num_states++;
        }

        // Ajouter les transitions minimisées
        for (int symbol = 0; symbol < A.num_symbols; symbol++) {
            int target = -1;
            for (int t = 0; t < A.num_transitions; t++) {
                if (A.transitions[t].source == i && A.transitions[t].symbol == 'a' + symbol) {
                    target = A.transitions[t].target;
                }
            }

            if (target != -1) {
                minimized_automaton.transitions[minimized_automaton.num_transitions].source = state_map[i];
                minimized_automaton.transitions[minimized_automaton.num_transitions].symbol = 'a' + symbol;
                minimized_automaton.transitions[minimized_automaton.num_transitions].target = state_map[target];
                minimized_automaton.num_transitions++;
            }
        }
    }

    // Mettre à jour les états finaux
    for (int i = 0; i < A.num_states; i++) {
        if (is_final(A, i)) {
            minimized_automaton.final_states[minimized_automaton.num_final_states++] = state_map[i];
        }
    }

    return minimized_automaton;
}

// Fonction utilitaire pour vérifier si un état est final
int is_final(Automaton A, int state) {
    for (int i = 0; i < A.num_final_states; i++) {
        if (A.final_states[i] == state) {
            return 1;
        }
    }
    return 0;
}

// Génération of the complementary automata
Automaton complementary_automaton(Automaton A) {
    printf("Creation of the complementary automata...\n");

    // Créer une copie de l'automate d'origine
    Automaton AComp;
    AComp.num_symbols = A.num_symbols;
    AComp.num_states = A.num_states;
    AComp.num_transitions = A.num_transitions;
    AComp.num_initial_states = 1;
    AComp.initial_states[0] = 0;  // Initial state remains the same

    // Créer un état "puit" pour gérer les transitions manquantes
    int sink_state = A.num_states;
    AComp.num_states++;

    // Copier les transitions de l'automate original
    for (int i = 0; i < A.num_transitions; i++) {
        AComp.transitions[i] = A.transitions[i];
    }

    // Ajouter des transitions manquantes pour chaque état
    for (int i = 0; i < A.num_states; i++) {
        for (int j = 0; j < A.num_symbols; j++) {
            int found = 0;
            for (int t = 0; t < A.num_transitions; t++) {
                if (A.transitions[t].source == i && A.transitions[t].symbol == 'a' + j) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                // Si aucune transition n'existe, ajouter une transition vers l'état "puit"
                AComp.transitions[AComp.num_transitions].source = i;
                AComp.transitions[AComp.num_transitions].symbol = 'a' + j;
                AComp.transitions[AComp.num_transitions].target = sink_state;
                AComp.num_transitions++;
            }
        }
    }

    // Inverser les états finaux
    for (int i = 0; i < A.num_states; i++) {
        int is_final_state = 0;
        for (int j = 0; j < A.num_final_states; j++) {
            if (A.final_states[j] == i) {
                is_final_state = 1;
                break;
            }
        }

        // Si l'état est final dans l'automate original, il devient non final dans le complémentaire
        // Si l'état n'est pas final dans l'automate original, il devient final dans le complémentaire
        if (is_final_state) {
            // Ne pas ajouter cet état dans les états finaux du complémentaire
        } else {
            // Ajouter l'état comme final dans l'automate complémentaire
            AComp.final_states[AComp.num_final_states++] = i;
        }
    }

    // L'état "puit" ne doit pas être final
    return AComp;
}

// Display of the minimal automata with corresponding of states
void display_minimal_automaton(Automaton A) {
    printf("Minimal automata :\n");
    display_automaton(A);
    //printf("Correspondance des états avec l'automate original : (à implémenter)\n");
}

// Reconnaissance de mots
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
        if (!found) {
            return 0;  // Si aucune transition n'est trouvée pour le symbole, le mot est rejeté
        }
    }

    // Vérifier si l'état actuel est un état final
    for (int i = 0; i < A.num_final_states; i++) {
        if (current_state == A.final_states[i]) {
            return 1;  // Le mot est accepté
        }
    }
    return 0;  // Le mot n'est pas accepté
}

