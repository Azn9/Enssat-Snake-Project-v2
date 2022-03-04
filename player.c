// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the paths
extern const char BONUS; // ascii used for the bonuses

extern bool DEBUG; // indicates whether the game runs in DEBUG mode

char *binome = "AzidMouad"; // student names here

// prototypes of the local functions/procedures
void printAction(action);

void printBoolean(bool);

void item_tree_printer(item_tree);

void lire_arbre(item_tree);

action choix(item_tree, item_tree, item_tree, item_tree);


/*
  snake function:
  This function randomly select a valid move for Snake based on its current position on the game map.
 */
action snake(
        item_tree itn, // what Snake is seeing when looking at north, up to a distance of 3
        item_tree ite, // what Snake is seeing when looking at east, up to a distance of 3
        item_tree its, // what Snake is seeing when looking at south, up to a distance of 3
        item_tree itw, // what Snake is seeing when looking at west, up to a distance of 3
        snake_list s, // snake
        action last_action // last action made, -1 in the beginning
) {
    action a; // action to choose and return

    bool ok = false; // ok will be set to true as soon as a randomly selected action is valid

    if (DEBUG) { // print what Snake is seeing when looking at north, east, south and west
        /* printf("North vision tree:\n");
         item_tree_printer(itn);
         printf("\nEast vision tree:\n");
         item_tree_printer(ite);
         printf("\nSouth vision tree:\n");
         item_tree_printer(its);
         printf("\nWest vision tree:\n");
         item_tree_printer(itw);
         printf("\n");*/

    }

    do {
        //a = rand() % 4; // randomly select an action: 0=NORTH, 1=EAST...
        a = choix(itn,ite,its,itw);


        if (DEBUG) { // print the randomly selected action, only in DEBUG mode
            /*printf("Candidate action is: ");
            printAction(a);
            printf("\n");*/
            printf("nord");
            lire_arbre(itn);
            printf("est");
            lire_arbre(ite);
            printf("sud");
            lire_arbre(its);
            printf("ouest");
            lire_arbre(itw);

        }

        switch (a) { // check whether the randomly selected action is valid, i.e., if its preconditions are satisfied
            case NORTH:
                if (itn->item != WALL
                    && itn->item != SNAKE_BODY
                    && itn->item != SNAKE_TAIL)
                    ok = true;
                break;
            case EAST:
                if (ite->item != WALL
                    && ite->item != SNAKE_BODY
                    && ite->item != SNAKE_TAIL)
                    ok = true;
                break;
            case SOUTH:
                if (its->item != WALL
                    && its->item != SNAKE_BODY
                    && its->item != SNAKE_TAIL)
                    ok = true;
                break;
            case WEST:
                if (itw->item != WALL
                    && itw->item != SNAKE_BODY
                    && itw->item != SNAKE_TAIL)
                    ok = true;
                break;
        }

        if (DEBUG) { // print whether the randomly selected action is valid, only in DEBUG mode
            /*printf("Is this candidate action valid? ");
            printBoolean(ok);
            printf("\n");*/
        }
    } while (!ok && (itn->item == PATH || itn->item == BONUS
                     || ite->item == PATH || ite->item == BONUS
                     || its->item == PATH || its->item == BONUS
                     || itw->item == PATH || itw->item == BONUS));

    return a; // answer to the game engine
}

/*
  printAction procedure:
  This procedure prints the input action name on screen.
 */
void printAction(action a) {
    switch (a) {
        case NORTH:
            printf("NORTH");
            break;
        case EAST:
            printf("EAST");
            break;
        case SOUTH:
            printf("SOUTH");
            break;
        case WEST:
            printf("WEST");
            break;
    }
}


/*
  printBoolean procedure:
  This procedure prints the input boolean value on screen.
 */
void printBoolean(bool b) {
    if (b) {
        printf("true");
    } else {
        printf("false");
    }
}

void lire_arbre(item_tree a) {
    printf("\nval = %c\n", a->item);
    //printf("\nval2 = %c\n", a->pnn->item);

}

bool test_path(item_tree a) {

    if (a->item == BONUS) {
        return true;
    } else return false;
}

action choix(item_tree n, item_tree e, item_tree s, item_tree w) {
    if (test_path(n)) {
        return NORTH;
    } else if (test_path(e)) {
        return EAST;
    } else if (test_path(s)) {
        return SOUTH;
    } else if (test_path(w)) {
        return WEST;
    } else {
        return rand() % 4;
    }
}