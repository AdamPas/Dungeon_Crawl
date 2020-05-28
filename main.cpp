/*
Make a program that outputs a simple grid based gameboard to
the screen using either numbers or characters.
Allow the user  to move either up, down, left, or right
each turn. If the player steps on a trap then they lose.
If the make it to the treasure 'X' then they win.

★★ Add enemies that move randomly in any direction once per turn. (enemies just like traps cause the player to lose if touched)

 Extra ideas:
 TODO: Place the exit randomly in one of the corners!
 TODO: Also place the player randomly on one of the corners

 1) Have different player classes with special powers: The knight has 50% chance of killing the orc.
                                                       The monk can eliminate the first orc encounter.
 2) Have events occur, like summoning enemies and traps
 3) Different types of enemies, with different behavior (e.g. random, chasing the player, moving away from the player or other enemies)
 4) Have the player move more than one characters successively (and win when they meet or have more to spare)
 6) It can serve as basis for a funny adventure, where there are dialogues and interaction with the user
 7) Connected rooms
 */

#include <iostream>
#include <cstdlib>
#include <time.h>

#include <exception>

#define GRID_SIZE 20

#define EMPTY '-'
#define PLAYER '@'
#define TRAP '#'
#define ENEMY '%'
#define GOAL 'X'
#define VISIBILITY 4
#define FOG '?'
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

using namespace std;

char grid[GRID_SIZE][GRID_SIZE];
int player_position[2];
bool stop = false;                      // if true, the game is finished

void print_intro(void);
void set_difficulty(int&,int&);
void show_grid(void);
void place_traps(int);
void place_enemies(int,int**);
void move_enemies(int,int**);
void move_player(void);
void check_move(void);

int main() {

    // game variables
    int num_traps;
    int num_enemies;

    // print intro message and instructions
    print_intro();

    // Choose difficulty
    set_difficulty(num_traps,num_enemies);

    // Allocate memory for the enemies' positions array
    int** enemies_positions;    // one row for each enemy
    try {
        try {
            enemies_positions = new int* [num_enemies];
        }
        catch(exception& e1){
            throw;
        }

        try {
            for (int i=0; i<num_enemies; i++){
                enemies_positions[i] = new int [2];
            }
        }
        catch(exception& e1){
            throw;
        }
    }
    catch(exception& e){
        cout << "Runtime error: " << e.what() << endl;
        return -1;
    }
    
    //........Initialization...........
    int turn = 0;
    srand(time(NULL));

    for(int i=0;i<GRID_SIZE;i++){
        for(int j=0;j<GRID_SIZE;j++){
            grid[i][j] = EMPTY;
        }
    }

    // place player
    player_position[0] = 0; player_position[1] = 0;
    grid[0][0] = PLAYER;

    // place goal, traps, enemies
    grid[GRID_SIZE-1][GRID_SIZE-1] = GOAL;
    place_traps(num_traps);
    place_enemies(num_enemies,enemies_positions);

    //.........................

    //............Game Loop.......................
    while(!stop){
        turn++;

        cout << "Turn " << turn << endl;
        show_grid();

        move_player();
        //show_grid();

        // if the player didn't lose or win
        if(!stop){
            move_enemies(num_enemies,enemies_positions);
        }

        show_grid();
    }
    // ..........................

    // release enemies position array
    for (int i=0; i<num_enemies; i++){
        delete[] enemies_positions[i];
    }
    delete[] enemies_positions;

    return 0;
}

void print_intro(void){
/*
 * Prints introduction and instructions
 */

    cout << "Cave Survival ver.1.1.\nAuthor: adam\n\n";
    cout << "You were having a nice walk in the sun, when you saw the cave entrance between the bushes. " << endl;
    cout << "Once your curiosity forced you to walk just 5 meters in, the entrance closed shut behind you!" << endl;
    cout << "You light your torch, knowing your only chance out is the dim light you see in the distance..." << endl;
    cout << "You soon realize you are not alone in the cave...\n" << endl;

    cout << "Instructions: Get to the light " << GOAL << " ,avoiding deadly traps "
         << TRAP << " and blind troglodytes " << ENEMY <<" !" << endl;
    cout << "Your flickering torch flame only lets you see 3 steps around you...";
    cout << "Your hero is " << PLAYER << "." << " Move in each turn with {a,w,s,d}.\nGood luck!\n";

}

void set_difficulty(int& num_traps, int& num_enemies){
/*
 * Asks the player for difficulty setting and adjusts traps and enemies
 */

    char difficulty;
    cout << "Choose level of difficulty {e,n,h}: ";
    cin >> difficulty;

    switch (difficulty){
        case 'h':
            cout << "Hard mode. \n";
            num_traps = 40;
            num_enemies = 27;
            break;
        case 'n':
            cout << "Normal mode. \n";
            num_traps = 38;
            num_enemies = 23;
            break;
        default:
            cout << "Easy mode. \n";
            num_traps = 35;
            num_enemies = 15;
    }
}

void show_grid(void){
/*
 * Prints the grid on the screen
 */

    // Print the actual grid only around the player. The rest is fog -> ?
    for(int i=0; i<GRID_SIZE; i++){
        for(int j=0; j<GRID_SIZE; j++){
            if (((i>(player_position[0]-VISIBILITY)) && (i<(player_position[0]+VISIBILITY)) &&
            (j>(player_position[1]-VISIBILITY)) && (j<(player_position[1]+VISIBILITY))) ||
            (grid[i][j] == GOAL))
                {cout << grid[i][j] << " ";}
            else
                {cout << FOG << " ";}
        }
        cout << endl;
    }


    // Empty screen
    cout << "\n\n";
}

void place_traps(int num_traps){
/*
 * Initializes trap locations randomly (except for the right and low borders)
 */

    int rand_x, rand_y;
    for(int i=0;i<num_traps;i++){
        // Make sure that each trap is placed in an empty place
        do{
            rand_x = rand() % (GRID_SIZE-2) + 1;
            rand_y = rand() % (GRID_SIZE-2) + 1;
        }while(grid[rand_x][rand_y]!=EMPTY);

        grid[rand_x][rand_y] = TRAP;
    }
}

void place_enemies(int num_enemies,int** enemies_positions){
/*
 * Places enemies randomly
 */
    int rand_x, rand_y;
    for(int i=0;i<num_enemies;i++){
        // Make sure that each enemy is placed in an empty place
        do{
            rand_x = rand() % (GRID_SIZE-2) + 1;
            rand_y = rand() % (GRID_SIZE-2) + 1;
        }while(grid[rand_x][rand_y]!=EMPTY);

        enemies_positions[i][0] = rand_x;       // store enemy's position and update grid
        enemies_positions[i][1] = rand_y;
        grid[rand_x][rand_y] = ENEMY;

    }
}

void move_enemies(int num_enemies,int** enemies_positions){
/*
 * Moves the enemies randomly
 */

    int direction, enemy_x, enemy_y;

    // move each enemy
    for(int i=0;i<num_enemies;i++){

        // if the game is over, don't move the rest of the enemies
        if(!stop) {
            // current enemy's position on the board
            enemy_x = enemies_positions[i][0];
            enemy_y = enemies_positions[i][1];

            direction = rand() % 4;             // choose a random direction

            switch (direction) {
                // UP
                case 0:
                    // Don't move on a trap or another enemy!
                    if ((enemy_x - 1) < 0 or grid[enemy_x - 1][enemy_y] == TRAP or grid[enemy_x - 1][enemy_y] == ENEMY)
                        break;

                    grid[enemy_x][enemy_y] = EMPTY;                  // update current grid cell
                    enemy_x--;
                    enemies_positions[i][0]--;                       // update position
                    grid[enemy_x][enemy_y] = ENEMY;                  // populate new cell
                    check_move();
                    break;

                    // DOWN
                case 1:
                    // Don't move on a trap or another enemy!
                    if ((enemy_x + 1) == GRID_SIZE or grid[enemy_x + 1][enemy_y] == TRAP or
                        grid[enemy_x + 1][enemy_y] == GOAL or grid[enemy_x + 1][enemy_y] == ENEMY)
                        break;

                    grid[enemy_x][enemy_y] = EMPTY;                         // empty current player cell
                    enemy_x++;
                    enemies_positions[i][0]++;                              // update position
                    grid[enemy_x][enemy_y] = ENEMY;                         // populate new cell
                    check_move();
                    break;

                    // LEFT
                case 2:
                    // Don't move on a trap or another enemy!
                    if ((enemy_y - 1) < 0 or grid[enemy_x][enemy_y - 1] == TRAP or grid[enemy_x][enemy_y - 1] == ENEMY)
                        break;

                    grid[enemy_x][enemy_y] = EMPTY;                  // empty current player cell
                    enemy_y--;
                    enemies_positions[i][1]--;                       // update position
                    grid[enemy_x][enemy_y] = ENEMY;                 // populate new cell
                    check_move();
                    break;

                    // RIGHT
                case 3:
                    // Don't move on a trap or another enemy!
                    if ((enemy_y + 1) == GRID_SIZE or grid[enemy_x][enemy_y + 1] == TRAP or
                        grid[enemy_x][enemy_y + 1] == GOAL or grid[enemy_x][enemy_y + 1] == ENEMY)
                        break;

                    grid[enemy_x][enemy_y] = EMPTY;                  // empty current player cell
                    enemy_y++;
                    enemies_positions[i][1]++;                       // update position
                    grid[enemy_x][enemy_y] = ENEMY;                  // populate new cell
                    check_move();
                    break;
            }
        }
    }

}

void move_player(void){
/*
 * Moves the player depending on user input
 */

    // Keep asking until the player provides valid input (take the first letter in case of string)
    string input;
    do {
        cout << "Enter direction: ";
        getline(cin, input);
    }while(input[0] != 'a' and input[0] != 'w' and input[0] != 's' and input[0] != 'd');

    switch(input[0]){
        case UP:
            // Check validity
            if((player_position[0]-1)<0) break;

            grid[player_position[0]][player_position[1]] = EMPTY;   // empty current player cell
            player_position[0]--;                                   // update position
            check_move();
            grid[player_position[0]][player_position[1]] = PLAYER;  // populate new player cell
            break;

        case DOWN:
            // Check validity
            if((player_position[0]+1)>(GRID_SIZE-1)) break;

            grid[player_position[0]][player_position[1]] = EMPTY;   // empty current player cell
            player_position[0]++;                                   // update position
            check_move();
            grid[player_position[0]][player_position[1]] = PLAYER;  // populate new player cell
            break;

        case LEFT:
            // Check validity
            if((player_position[1]-1)<0) break;

            grid[player_position[0]][player_position[1]] = EMPTY;   // empty current player cell
            player_position[1]--;                                   // update position
            check_move();
            grid[player_position[0]][player_position[1]] = PLAYER;  // populate new player cell
            break;

        case RIGHT:
            // Check validity
            if((player_position[1]+1)>(GRID_SIZE-1)) break;

            grid[player_position[0]][player_position[1]] = EMPTY;   // empty current player cell
            player_position[1]++;                                   // update position
            check_move();
            grid[player_position[0]][player_position[1]] = PLAYER;  // populate new player cell
    }

}

void check_move(void){
/*
 * Checks if a player or enemy move resulted in defeat/victory
 */

    if(grid[player_position[0]][player_position[1]] == TRAP){
        cout << "You fall in a bottomless pit! Lord have mercy on your soul...\n\n";
        stop = true;
    }
    else if(grid[player_position[0]][player_position[1]] == ENEMY){
        cout << "You are captured by a troglodyte! "
                "Soon you will become dinner...\n\n";
        stop = true;
    }
    else if(grid[player_position[0]][player_position[1]] == GOAL){
        cout << "You made it to the light! You step outside of the cave, \n"
                "glad that your meat is still on your bones...\n\n";
        stop = true;
    }
}