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

// This enum represents a movement: going forward, turning left or turning right
enum movements {
    FORWARD, LEFT, RIGHT
};
typedef enum movements movement;

bool is_action_allowed(
        action planned_action,
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        action last_action
);

movement get_last_movement(snake_list snake);

// ### Searching methods ###
bool search_bonus(item_tree item_tree);

action search_bonus_direction(
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        bool only_straight
);

int search_type_straight(
        char char_to_search,
        item_tree item_tree,
        enum actions direction,
        int initial_distance
);

// ### Actions computing methods ###
action compute_action(
        action last_action,
        movement last_movement,
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        snake_list snake
);

action skip_to_bonus(
        snake_list snake,
        int distance_bonus
);

action compute_action_forward(
        action last_action,
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        snake_list snake
);

action compute_action_left(
        action last_action,
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        snake_list snake
);

action compute_action_right(
        action last_action,
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        snake_list snake
);

/*
  snake function:
  This function returns a valid move for Snake based on its current position on the game map.
 */
action snake(
        item_tree itn,
        item_tree ite,
        item_tree its,
        item_tree itw,
        snake_list s,
        action last_action
) {
    action a;
    int snake_length = score + 1; // Use the score to get snake's length (Allowed by Pr. Goasdoué)

    if (snake_length < 3) { // Snake is too short to calculate his previous movement
        a = search_bonus_direction(itn, ite, its, itw, false); // Search for a bonus in our field of view

        if (a == -1) { // The bonus is invisible
            do {
                a = rand() % 4;
            } while (!is_action_allowed(a, itn, ite, its, itw, last_action)); // Play a random allowed action
        }
    } else {
        movement last_movement = get_last_movement(s); // Movement calculation based on snake's position

        if (DEBUG) {
            printf("LM : %s ", last_movement == FORWARD ? "FORWARD" : last_movement == LEFT ? "LEFT" : "RIGHT");
            printf("| LA : %s\n", last_action == NORTH ? "NORTH" : last_action == WEST ? "WEST" : last_action == EAST ? "EAST" : "SOUTH");
        }

        a = compute_action(last_action, last_movement, itn, ite, its, itw, s); // Calculate an action based on the last action and movement and what snake can see
    }

    if (a == -1) { // If we don't know what to do, we do the same move as before
        a = last_action;
    }

    if (a == -1) { // If the previous move was -1 (at start), choose a valid first move
        if (itw->item != WALL) {
            return WEST;
        } else if (itn->item != WALL) {
            return NORTH;
        } else if (its->item != WALL) {
            return SOUTH;
        } else {
            return EAST;
        }
    }

    bool has_exit_left = true; // This boolen represents if there is a valid action left

    if (!is_action_allowed(a, itn, ite, its, itw, last_action)) { // If the current planned action is disallowed
        if (DEBUG) {
            printf("Action disallowed! Searching for tail...\n");
        }

        if (search_type_straight(SNAKE_TAIL, itw, WEST, 0) > 0) { // Search for the tail in straight line to the WEST
            a = WEST;

            if (DEBUG) {
                printf("Tail found on WEST\n");
            }
        } else if (search_type_straight(SNAKE_TAIL, its, SOUTH, 0) > 0) { // Search for the tail in straight line to the SOUTH
            a = SOUTH;

            if (DEBUG) {
                printf("Tail found on SOUTH\n");
            }
        } else if (search_type_straight(SNAKE_TAIL, itn, NORTH, 0) > 0) { // Search for the tail in straight line to the NORTH
            a = NORTH;

            if (DEBUG) {
                printf("Tail found on NORTH\n");
            }
        }
    }

    if (!is_action_allowed(a, itn, ite, its, itw, last_action)) { // If our planned action is still disallowed (e.g. we can't find our tail)
        // Use a list representing if we tested all the possible directions, to avoid infinite loops
        int tested[4];
        for (int i = 0; i < 4; ++i) {
            tested[i] = 0; // Initialize our list at 0
        }

        bool searching = true;
        do {
            a = rand() % 4; // Get a random action between 0 and 3

            tested[a] = 1; // Mark the action as tested

            int i = 0;
            for (; i < 4 && tested[i] == 0; i++) { // Check if all values are 1
            }

            has_exit_left = i == 4; // If all values are 1, there is no exit

            searching = !has_exit_left && !is_action_allowed(a, itn, ite, its, itw, last_action); // while condition
        } while (searching);
    }

    if (DEBUG && !has_exit_left) {
        printf("There is no exit... :'(\n");
    }

    return a;
}

/*
 * compute_action function
 * This function uses the last action and the last movement done by snake and what he can see to choose the best action
 */
action compute_action(action last_action, movement last_movement, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list snake) {
    action a = -1;

    // Search for the bonus in straight line on WEST
    int distance_bonus = search_type_straight(BONUS, itw, WEST, 1);

    // Search for any part of the snake's body in straight line on WEST
    int distance_body = search_type_straight(SNAKE_BODY, itw, WEST, 1);

    if (DEBUG) {
        printf("DB : %d | %d\n", distance_bonus, distance_body);
    }

    // If we found a bonus in our field of view, closer than the snake's body
    if (distance_bonus > 0 && (distance_body == 0 || distance_body > distance_bonus)) {
        a = skip_to_bonus(snake, distance_bonus); // Check if we can "skip" the loop and go directly to the bonus safely

        if (a != -1) { // If we can go to the bonus
            return a;
        }
    }

    switch (last_movement) {
        case FORWARD:
            a = compute_action_forward(last_action, itn, ite, its, itw, snake);
            break;

        case LEFT:
            a = compute_action_left(last_action, itn, ite, its, itw, snake);
            break;

        case RIGHT:
            a = compute_action_right(last_action, itn, ite, its, itw, snake);
            break;

        default:
            break;
    }

    return a;
}

/*
 * compute_action_forward function
 * This function focuses on what to do after snake going forward
 */
action compute_action_forward(action last_action, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list snake) {
    action a = -1;

    switch (last_action) {
        case NORTH:
            if (itn->item == WALL) { // Snake hit the top of the map
                a = WEST;
            } else {
                a = NORTH;
            }
            break;

        case WEST: // Skipped for bonus
            if (itw->item == WALL) { // Snake hit the left wall of the map
                a = SOUTH;
            } else if (itn->item == WALL) { // Snake is on the first playable row
                snake_list current_snake_part = snake; // Value used for iterating over snake's body parts
                int on_line_count = 0; // Count of snake pieces on the same row
                int current_y = snake->y; // The coordinate of the current row (1 everytime due to the previous condition in case of a square map)

                // Iterate through snake parts, stopping when we found either a piece on a different row, or if we reach the tail
                while (current_snake_part->next != NULL && current_snake_part->y == current_y) {
                    on_line_count++;
                    current_snake_part = current_snake_part->next;
                }

                if (DEBUG) {
                    printf("OLC %d\n", on_line_count);
                }

                // If there is less than 5 pieces on the same row (3 columns skipped, plus the first and last where snake moves)
                // and if the snake isn't fully on the same row (happens when the snake's lenght is 3 or 4)
                if (on_line_count < 5 && on_line_count < score) {
                    a = WEST;
                } else {
                    a = SOUTH;
                }
            } else {
                snake_list current_snake_part = snake->next; // Value used for iterating over snake's body parts
                snake_list before_last_snake_part = snake->next; // Value representing the before last part

                // Iterate through snake parts, stopping when we reach the tail
                while (current_snake_part->next != NULL && current_snake_part->c != SNAKE_TAIL) {
                    if (current_snake_part->next->c == SNAKE_TAIL) { // If the next part is the tail, then the current part is the last before tail
                        before_last_snake_part = current_snake_part;
                    }

                    current_snake_part = current_snake_part->next;
                }

                int tail_x = current_snake_part->x;
                int tail_y = current_snake_part->y;
                int current_x = snake->x;
                int current_y = snake->y;

                if (tail_x < current_x) { // Tail is at snake's head left
                    if (before_last_snake_part->y < tail_y) { // Tail is going up
                        if (its->item == PATH) { // If we can go to the SOUTH
                            a = SOUTH;
                        } else {
                            a = NORTH;
                        }
                    } else if (before_last_snake_part->y < tail_y) { // Tail is going down
                        if (itn->item == WALL) { // If we can't go to the NORTH
                            a = SOUTH;
                        } else {
                            a = NORTH;
                        }
                    } else { // Tail is going left or right
                        if (tail_y < current_y) { // If tail is above the head
                            a = NORTH;
                        } else if (tail_y > current_y) { // If the tail is below the head
                            a = SOUTH;
                        } else { // If tail and head are on the same row
                            if (itn->item == WALL) { // If we can't go to the NORTH
                                a = SOUTH;
                            } else {
                                a = NORTH;
                            }
                        }
                    }
                } else if (tail_x > current_x) { // If the tail is at our right
                    a = NORTH;
                } else { // If tail and head are on the same column
                    if (tail_y > current_y) { // If tail is above the head
                        if (before_last_snake_part->x == current_x) { // If the before last part is also on the same column
                            if (before_last_snake_part->y > tail_y) { // Tail is going down
                                a = SOUTH;
                            } else {
                                a = NORTH;
                            }
                        } else {
                            a = SOUTH;
                        }
                    } else if (tail_y < current_y) { // If tail is below the head
                        a = NORTH;
                    } else {
                        // Impossible, tail and head would be on the same cell
                    }
                }
            }
            break;

        case EAST:
            if (ite->item == WALL) { // Snake hit the bottom right corner of the map
                a = NORTH;
            } else {
                a = EAST;
            }
            break;

        case SOUTH:
            if (its->item == WALL) { // Snake hit the bottom of the map
                a = EAST;
            } else if (its->psn->item == WALL) { // row before the last one
                if (itw->item == WALL) { // Last column at left
                    a = SOUTH;
                } else {
                    a = WEST;
                }
            }
            break;

        default:
            break;
    }

    return a;
}

/*
 * compute_action_left function
* This function focuses on what to do after snake turned to the left
 */
action compute_action_left(action last_action, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list snake) {
    action a = -1;

    switch (last_action) {
        case NORTH:
            a = NORTH; // Snake is in the bottom right corner
            break;

        case WEST:
            if (itw->item == WALL) { // Snake hit the left wall
                a = SOUTH;
            } else if (itw->pwn->item == WALL) { // We're on an odd map
                a = WEST;
            } else {
                if (itn->item == WALL) { // Snake hit the top of the map
                    int test_snake_length = (score + 1) - 5;

                    // Here, we need to check if the snake can skip the next 3 columns or if he will go over his tail

                    if (test_snake_length < 0) { // If snake is shorter than 5, there is no risks
                        a = WEST;
                    } else {
                        snake_list current_snake_part = snake; // Used for iterate through snake's parts
                        bool has_part_above = false; // True if there is any part of the snake is at the head's left

                        for (int i = test_snake_length; i > 0; i--) { // We're checking only for the parts that will exist after the snake has moved 5 cells
                            if (current_snake_part->x < snake->x) {
                                has_part_above = true;
                            }

                            current_snake_part = current_snake_part->next;
                        }

                        // Here, current_snake_part is at the cell in with the tail will be after moving

                        if (current_snake_part->x > snake->x && has_part_above) { // We will go over the tail
                            a = SOUTH;
                        } else {
                            a = WEST;
                        }
                    }

                    // Final check to be sure that the move is allowed
                    if (a == WEST && (itw->item == WALL || itw->item == SNAKE_BODY)) {
                        a = SOUTH;
                    }
                } else {
                    a = SOUTH;
                }
            }
            break;

        case EAST:
            if (its->item != WALL && itw->pwn->item == WALL) { // Odd map zigzag
                a = SOUTH;
            } else {
                a = EAST; // Bottom of the map
            }
            break;

        case SOUTH:
            if (its->item == WALL) { // Bottom of the map
                a = EAST;
            } else if (its->psn->item == WALL) { // row before the last one
                if (itw->item == WALL) { // Last column at left
                    a = SOUTH;
                } else {
                    a = WEST;
                }
            } else if (itw->item == WALL) { // Odd map zigzag
                if (ite->item == PATH) {
                    a = EAST;
                } else {
                    a = SOUTH;
                }
            } else {
                a = SOUTH;
            }
            break;

        default:
            break;
    }

    return a;
}

/*
 * compute_action_right function
 * This function focuses on what to do after snake turned to the right
 */
action compute_action_right(action last_action, item_tree itn, item_tree ite, item_tree its, item_tree itw, snake_list snake) {
    action a = -1;

    switch (last_action) {
        case NORTH:
            if (itn->item == WALL) {
                a = WEST;
            } else {
                a = NORTH;
            }
            break;

        case WEST:
            if (itw->item == WALL) {
                a = SOUTH;
            } else {
                a = NORTH;
            }
            break;

        case EAST:
            // Impossible, snake is not allowed to do that
            break;

        case SOUTH:
            if (its->item == WALL) {
                a = EAST;
            } else if (itw->pwn->item == WALL) { //Odd map zigzag
                a = WEST;
            }
            break;

        default:
            break;
    }

    return a;
}

/*
 * skip_to_bonus function
 * This function calculate the action to follow the bonus, and check if the move is allowed
 */
action skip_to_bonus(snake_list snake, int distance_bonus) {
    action a = WEST;

    int bonus_x = snake->x - distance_bonus;

    snake_list current_snake_part = snake->next;
    while (current_snake_part->c != SNAKE_TAIL) {
        if (current_snake_part->y == snake->y - 1 && current_snake_part->x == bonus_x) { // Head is following the tail, we can't skip
            bonus_x = 0;
        }

        current_snake_part = current_snake_part->next;
    }

    if (bonus_x == 0) {
        a = -1;
    } else {
        int current_x = snake->x;
        int current_y = snake->y;
        int tail_x = current_snake_part->x;
        int tail_y = current_snake_part->y;

        if (tail_x < current_x) { // If tail is at left of head
            if (current_x - tail_x == distance_bonus) {
                if (tail_y < current_y) {
                    a = -1;
                }
            } else if (current_x - tail_x < distance_bonus) {
                a = -1;
            } else {
                int test_snake_length = score - distance_bonus;

                current_snake_part = snake;
                while (test_snake_length > 0) {
                    test_snake_length--;
                    current_snake_part = current_snake_part->next;
                }

                if (current_snake_part->x > bonus_x) { // We will go over the tail
                    current_snake_part = snake->next;
                    bool has_part_above = false;
                    while (current_snake_part->c != SNAKE_TAIL) {
                        if (current_snake_part->x < current_x) {
                            has_part_above = true;
                        }

                        current_snake_part = current_snake_part->next;
                    }

                    if (has_part_above) {
                        a = -1;
                    }
                }
            }
        }
    }

    return a;
}

/*
 * get_last_movement Function
 * This function calculate the last movement done from the snake's position
 */
movement get_last_movement(snake_list snake) {
    if (snake->next == NULL) { // Snake is only a head
        return FORWARD;
    } else {
        if (snake->next->next == NULL) { // Snake is only a head and a tail
            return FORWARD;
        } else {
            snake_list second_piece = snake->next;
            snake_list third_piece = second_piece->next;

            if (snake->x > third_piece->x) {
                if (snake->y > third_piece->y) {
                    if (snake->x == second_piece->x) {
                        return RIGHT;
                    } else {
                        return LEFT;
                    }
                } else if (snake->y < third_piece->y) {
                    if (snake->x == second_piece->x) {
                        return LEFT;
                    } else {
                        return RIGHT;
                    }
                } else {
                    return FORWARD;
                }
            } else if (snake->x < third_piece->x) {
                if (snake->y > third_piece->y) {
                    if (snake->x == second_piece->x) {
                        return LEFT;
                    } else {
                        return RIGHT;
                    }
                } else if (snake->y < third_piece->y) {
                    if (snake->x == second_piece->x) {
                        return RIGHT;
                    } else {
                        return LEFT;
                    }
                } else {
                    return FORWARD;
                }
            } else {
                return FORWARD;
            }
        }
    }
}

/*
 * is_action_allowed function
 * This function returns true if the planned_action is allowed and don't kill the snake
 */
bool is_action_allowed(action planned_action, item_tree itn, item_tree ite, item_tree its, item_tree itw, action last_action) {
    bool ok = false;

    if ((planned_action + 2) % 4 == last_action) { // Test to avoid going backward
        if (DEBUG) {
            printf("Trying to go backward!\n");
        }

        return false;
    }

    switch (planned_action) { // check whether the planned action is valid
        case NORTH:
            if (itn->item != WALL && itn->item != SNAKE_BODY)
                ok = true;
            break;
        case EAST:
            if (ite->item != WALL && ite->item != SNAKE_BODY)
                ok = true;
            break;
        case SOUTH:
            if (its->item != WALL && its->item != SNAKE_BODY)
                ok = true;
            break;
        case WEST:
            if (itw->item != WALL && itw->item != SNAKE_BODY)
                ok = true;
            break;

        default:
            break;
    }

    return ok;
}

/*
 * search_bonus_direction function
 * This function returns an action leading to the bonus (if found)
 */
action search_bonus_direction(item_tree itn, item_tree ite, item_tree its, item_tree itw, bool only_straight) {
    if (only_straight) {
        if (search_type_straight(BONUS, itn, NORTH, 1) > 0) {
            return NORTH;
        } else if (search_type_straight(BONUS, ite, EAST, 1) > 0) {
            return EAST;
        } else if (search_type_straight(BONUS, itw, WEST, 1) > 0) {
            return WEST;
        } else if (search_type_straight(BONUS, its, SOUTH, 1) > 0) {
            return SOUTH;
        }
    } else {
        if (search_bonus(itn)) {
            return NORTH;
        } else if (search_bonus(ite)) {
            return EAST;
        } else if (search_bonus(itw)) {
            return WEST;
        } else if (search_bonus(its)) {
            return SOUTH;
        }
    }

    return -1;
}

/*
 * search_bonus function
 * this function returns true if the bonus is found by browsing the specified item_tree recursively
 */
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

/*
 * search_type_straight function
 * this function returns the distance between the current position and the bonus position (if found, 0 else) recursively, by going only one direction
 */
int search_type_straight(char char_to_search, item_tree item_tree, enum actions direction, int initial_distance) {
    if (item_tree == NULL) {
        return 0;
    }

    if (item_tree->item == char_to_search) {
        return initial_distance;
    } else {
        if (direction == NORTH && item_tree->psn != NULL) {
            return search_type_straight(char_to_search, item_tree->psn, NORTH, initial_distance + 1);
        }
        if (direction == EAST && item_tree->pen != NULL) {
            return search_type_straight(char_to_search, item_tree->pen, EAST, initial_distance + 1);
        }
        if (direction == WEST && item_tree->pwn != NULL) {
            return search_type_straight(char_to_search, item_tree->pwn, WEST, initial_distance + 1);
        }
        if (direction == SOUTH && item_tree->psn != NULL) {
            return search_type_straight(char_to_search, item_tree->psn, SOUTH, initial_distance + 1);
        }
    }

    return 0;
}
