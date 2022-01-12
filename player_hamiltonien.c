// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdio.h>   // printf
#include <stdlib.h>  // rand

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL;       // ascii used for the walls
extern const char PATH;       // ascii used for the paths
extern const char BONUS;      // ascii used for the bonuses

extern bool DEBUG; // indicates whether the game runs in DEBUG mode
extern int score; // the current score

char *binome = "Axel JOLY"; // student names here

enum movements {
    FORWARD, LEFT, RIGHT
};
typedef enum movements movement;

bool is_allowed(action action, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list s);

movement action_to_movement(action action, snake_list snake);

bool search_bonus(item_tree item_tree);

/*
  snake function:
  This function randomly select a valid move for Snake based on its current
  position on the game map.
 */
action snake(item_tree itn, // what Snake is seeing when looking at north, up to
        // a distance of 3
             item_tree ite, // what Snake is seeing when looking at east, up to
        // a distance of 3
             item_tree its, // what Snake is seeing when looking at south, up to
        // a distance of 3
             item_tree itw, // what Snake is seeing when looking at west, up to
        // a distance of 3
             snake_list s,  // snake
             action last_action // last action made, -1 in the beginning
) {
    action a = last_action;
    movement last_movement = action_to_movement(last_action, s);
    int snake_length = score + 1;

    if (snake_length < 3) {
        if (search_bonus(itn)) {
            if (is_allowed(NORTH, itn, ite, its, itw, s)) {
                a = NORTH;
            }
        } else if (search_bonus(ite)) {
            if (is_allowed(EAST, itn, ite, its, itw, s)) {
                a = EAST;
            }
        } else if (search_bonus(itw)) {
            if (is_allowed(WEST, itn, ite, its, itw, s)) {
                a = WEST;
            }
        } else if (search_bonus(its)) {
            if (is_allowed(SOUTH, itn, ite, its, itw, s)) {
                a = SOUTH;
            }
        } else {
            do {
                a = rand() % 4;
            } while (!is_allowed(a, itn, ite, its, itw, s));
        }

        return a;
    }

    if (DEBUG) {
        printf("LM : %s ", last_movement == FORWARD ? "FORWARD" : last_movement == LEFT ? "LEFT" : "RIGHT");
        printf("| LA : %s\n", last_action == NORTH ? "NORTH" : last_action == WEST ? "WEST" : last_action == EAST ? "EAST" : "SOUTH");
    }

    if (last_movement == FORWARD) {
        if (last_action == WEST) {
            if (itw->item == WALL) {
                a = SOUTH;
            } else {
                a = WEST;
            }
        } else if (last_action == SOUTH) {
            if (its->item == WALL) {
                a = EAST;
            } else if (its->psn->item == WALL) {
                if (itw->item == WALL) {
                    a = SOUTH;
                } else {
                    a = WEST;
                }
            }
        } else if (last_action == NORTH) {
            if (itn->item == WALL) {
                a = WEST;
            } else {
                a = NORTH;
            }
        } else if (last_action == EAST) {
            if (its->item == WALL) {
                if (ite->item == WALL) {
                    a = NORTH;
                }
            } else {
                a = SOUTH;
            }
        }
    } else if (last_movement == LEFT) {
        if (last_action == WEST) {
            if (itn->item == WALL) {
                a = SOUTH;
            }
        } else if (last_action == EAST) {
            if (ite->item == WALL) {
                a = NORTH;
            }
        } else if (last_action == SOUTH) {
            a = SOUTH;
        } else if (last_action == NORTH) {
            a = NORTH;
        }
    } else {
        if (last_action == WEST) {
            if (its->psn->item == WALL) {
                a = NORTH;
            }
        } else if (last_action == EAST) {
            if (itw->item == WALL) {
                a = SOUTH;
            }
        } else if (last_action == SOUTH) {
            if (its->item == WALL) {
                a = WEST;
            } else if (its->psn->item == WALL) {
                a = WEST;
            } else {
                a = SOUTH;
            }
        } else if (last_action == NORTH) {
            a = NORTH;
        }
    }

    if (!is_allowed(a, itn, ite, its, itw, s)) {
        do {
            a = rand() % 4;
        } while (!is_allowed(a, itn, ite, its, itw, s));
    }

    if (a == -1) {
        if (itw->item != WALL) {
            a = WEST;
        } else if (itn->item != WALL) {
            a = NORTH;
        } else if (its->item != WALL) {
            a = SOUTH;
        } else {
            a = EAST;
        }
    }

    return a;
}

movement action_to_movement(action action, snake_list snake) {
    if (snake->next == NULL) { // Snake is only a head
        return FORWARD;
    } else {
        if (snake->next->next == NULL) { // Snake is only a and a tail
            return FORWARD;
        } else {
            snake_list third_piece = snake->next->next;

            if (snake->x > third_piece->x) {
                if (snake->y > third_piece->y) {
                    return RIGHT;
                } else if (snake->y < third_piece->y) {
                    return LEFT;
                } else {
                    return FORWARD;
                }
            } else if (snake->x < third_piece->x) {
                if (snake->y > third_piece->y) {
                    return RIGHT;
                } else if (snake->y < third_piece->y) {
                    return LEFT;
                } else {
                    return FORWARD;
                }
            } else {
                return FORWARD;
            }
        }
    }
}

bool is_allowed(action action, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list s) {
    bool ok = false;

    switch (action) { // check whether the randomly selected action is valid, i.e.,
        // if its preconditions are satisfied
        case NORTH:
            if (itn->item != WALL && itn->item != SNAKE_BODY &&
                itn->item != SNAKE_TAIL)
                ok = true;
            break;
        case EAST:
            if (ite->item != WALL && ite->item != SNAKE_BODY &&
                ite->item != SNAKE_TAIL)
                ok = true;
            break;
        case SOUTH:
            if (its->item != WALL && its->item != SNAKE_BODY &&
                its->item != SNAKE_TAIL)
                ok = true;
            break;
        case WEST:
            if (itw->item != WALL && itw->item != SNAKE_BODY &&
                itw->item != SNAKE_TAIL)
                ok = true;
            break;

        default:
            break;
    }

    return ok &&
           (itn->item == PATH || itn->item == BONUS || ite->item == PATH ||
            ite->item == BONUS || its->item == PATH || its->item == BONUS ||
            itw->item == PATH || itw->item == BONUS);
}

bool search_bonus(item_tree item_tree) {
    if (item_tree == NULL) {
        return false;
    }

    if (item_tree->item == BONUS) {
        return true;
    } else {
        if (item_tree->psn != NULL && search_bonus(item_tree->psn)) {
            return true;
        }
        if (item_tree->pen != NULL && search_bonus(item_tree->pen)) {
            return true;
        }
        if (item_tree->pwn != NULL && search_bonus(item_tree->pwn)) {
            return true;
        }
        if (item_tree->psn != NULL && search_bonus(item_tree->psn)) {
            return true;
        }
    }

    return false;
}