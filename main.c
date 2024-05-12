#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

typedef enum { X, O, N, C } type_e;
typedef struct node {
  struct node *parent_node;
  type_e node_state[9];
  int score;
} node_t;

type_e game_state[9] = {N, N, N, N, N, N, N, N, N};
type_e player_type;
type_e bot_type;
int turn;
int checks;

char to_lower_case(char c) {
  if (c >= 65 && c <= 90) {
    c += 32;
  }

  return c;
}

type_e game_done(type_e *state) {

  bool is_board_full = true;
  for (int i = 0; i < 9; i++) {
    if (state[i] == N) {
      is_board_full = false;
    }
  }

  // vertical LEFT
  if ((state[0] == state[3]) && (state[3] == state[6]) && (state[0] != N)) {
    return state[0];
  }

  // vertical MIDDLE
  if ((state[1] == state[4]) && (state[4] == state[7]) && (state[1] != N)) {
    return state[1];
  }

  // vetical RIGHT
  if ((state[2] == state[5]) && (state[5] == state[8]) && (state[2] != N)) {
    return state[2];
  }

  // horizontal UP
  if ((state[0] == state[1]) && (state[1] == state[2]) && (state[0] != N)) {
    return state[0];
  }

  // horizontal MIDDLE
  if ((state[3] == state[4]) && (state[4] == state[5]) && (state[3] != N)) {
    return state[3];
  }

  // horizontal DOWN
  if ((state[6] == state[7]) && (state[7] == state[8]) && (state[6] != N)) {
    return state[6];
  }

  // diagonal LEFT
  if ((state[0] == state[4]) && (state[4] == state[8]) && (state[0] != N)) {
    return state[0];
  }

  // diagonal RIGHT
  if ((state[2] == state[4]) && (state[4] == state[6]) && (state[2] != N)) {
    return state[2];
  }

  if (is_board_full == true) {
    return N;
  }

  return C;
}

void display_board(type_e *state) {
  for (int i = 0; i < 9; i++) {
    if (i == 0 || i == 3 || i == 6) {
      printf("|");
    }

    if (state[i] == X) {
      printf("%c", 'X');
    } else if (state[i] == O) {
      printf("%c", 'O');
    } else {
      printf("%c", ' ');
    }

    printf("|");

    if (i == 2 || i == 5 || i == 8) {
      printf("\n");
      printf("_______");
      printf("\n");
    }
  }

  printf("\n\n");
}

int node_search(node_t *node, type_e play_type, int depth) {

  node_t *results[9];
  int length = 0;
  for (int i = 0; i < 9; i++) {
    if (node->node_state[i] == N) {
      node_t *new_node = (node_t *)malloc(sizeof(node_t));
      new_node->parent_node = node;
      memcpy(new_node->node_state, node->node_state, sizeof(type_e) * 9);
      new_node->node_state[i] = play_type;
      type_e result = game_done(new_node->node_state);
      if (result == X) {
        new_node->score = 1;
      } else if (result == O) {
        new_node->score = -1;
      } else if (result == C) {
        new_node->score = 0;
      }

      if (new_node->score == 0 && (depth > 0)) {
        type_e new_type;
        if (play_type == O) {
          new_type = X;
        } else if (play_type == X) {
          new_type = O;
        }
        new_node->score = node_search(new_node, new_type, depth - 1);
      }

      results[length] = new_node;
      length += 1;
    }
  }

  if (length == 0) {
    return node->score;
  }

  srand(time(NULL));

  int best_node_index = 0;
  for (int i = 0; i < length; i++) {

    if (play_type == X) {
      if (results[i]->score > results[best_node_index]->score) {
        best_node_index = i;
      }

    } else if (play_type == O) {
      if (results[best_node_index]->score > results[i]->score) {
        best_node_index = i;
      }
    }
  }

  for (int i = 0; i < length; i++) {
    if (results[best_node_index]->score == results[i]->score) {
      int max = 100;
      int random_value = rand() % max;
      if (random_value > 20) {
        best_node_index = i;
      }
    }
  }

  if (node->parent_node == NULL) {
    memcpy(node->node_state, results[best_node_index]->node_state,
           sizeof(type_e) * 9);
  }

  int score = results[best_node_index]->score;

  for (int i = 0; i < length; i++) {
    free(results[i]);
  }

  return score;
}

void bot_logic() {

  node_t root_node;

  root_node.score = 0;
  root_node.parent_node = NULL;
  memcpy(root_node.node_state, game_state, sizeof(type_e) * 9);

  int score = node_search(&root_node, bot_type, 1);
  memcpy(game_state, root_node.node_state, sizeof(type_e) * 9);
}

void play() {

  if (turn == 0) {

    int move;
    printf("What's your move -> ");
    if (scanf("%d", &move) != 1 || move > 9 || move < 1) {
      while (getchar() != '\n')
        ;
      printf("You entered an invalid position. Try again.\n");
      return;
    }
    printf("\n");

    if (game_state[move - 1] != N) {
      printf("The position that you entered is already used. Please choose and "
             "empty position\n");
      return;
    } else {
      game_state[move - 1] = player_type;
    }

    turn = 1;
  } else if (turn == 1) {

    bot_logic();

    turn = 0;

    sleep(1);
  }

  display_board(game_state);
}

int main(int argc, char *argv[]) {
  printf("************* Welcome to tic tac to bot ! *************\n\n");
  char choice;

  printf("Please choose a side (x or o) -> ");
  scanf("%c", &choice);
  printf("\n");

  choice = to_lower_case(choice);

  if (choice == 'x') {
    player_type = X;
    bot_type = O;
    turn = 0;

    printf("You chose X, so you will play first.\n\n");

  } else if (choice == 'o') {
    player_type = O;
    bot_type = X;
    turn = 1;

    printf("You chose O, so the bot will play first.\n\n");
  } else {
    printf("You made an invalid choice. Please try again.\n");
    return -1;
  }

  display_board(game_state);
  int won = -1;

  while (true) {
    type_e result = game_done(game_state);
    if (result != C) {
      switch (result) {
      case X:
        if (player_type == X)
          won = 1;
        break;
      case O:
        if (player_type == O)
          won = 1;
        break;
      case N:
        won = 0;
        break;
      default:
        break;
      }
      break;
    }
    play();
  }

  if (won == 1) {
    printf("You Won !\n");
  } else if (won == -1) {
    printf("You Lost :( \n");
  } else {
    printf("It's a draw.\n");
  }

  return 0;
}
