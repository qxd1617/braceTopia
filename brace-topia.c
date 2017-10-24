/* Title: Brace-topia
 * Author: (Owen) Quan Dong
 * This simulation will create a 'city' of programmers who have a preference either for putting the open brace
 * on the end of the line that starts a block, or for putting the open braces on a newline by itself. These 
 * programmer 'agents' are known as the e's and the n's. This simulation will move agents around until
 * all agents are at least as happy as the "happiness threshold" (entered by the user or default value).
 */



#define _BSD_SOURCE
#include "display.h"
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>


typedef struct{
	//Struct that simulates an 'agent' or person, they have a style and a happiness value calculated from their neighbors.
	char style;
	float happiness;
	int occupiedThisTurn;
}Person;

typedef struct{
	//Struct that simulates the physical address of a block in town.
	int xCoord;
	int yCoord;
}Location;

void printstate(int dim, Person board[][dim]){
	//Print the state of the board
    for (int row = 0; row < dim; row++) {
        for (int col = 0; col < dim; col++) {
            printf("%c", board[row][col].style);
        }
        puts(" ");
    }

}


void initBoard(int eStart, int nStart, int dim, Person board[][dim]){
	/*
	@param eStart: Number of starting endline persons
	@param nStart: Number of starting newline persons
	@param dim: Dimension of the board
	@param board: board itself
	Method initializes board to starting state using random values and inputted parameters.
	*/
	int eCount = 0;
	int nCount = 0;
	int row;
	int col;
	for (int x = 0; x < dim; x++){
		for (int y = 0; y < dim; y++){
			//Fill the entire board with empty blocks
			board[x][y].style = '.';
		}
	}	
	//Random seed
	srandom(41);
	
	while (eCount < eStart){
		//Add a new endline agent at the randomly generated x/y coordinates 
		//if it is unoccupied and the maximum number of agents has not been reached.
		row = random();
        	row %= dim;
        	col = random();
        	col %= dim;
		if (board[row][col].style == '.'){
			board[row][col].style = 'e';
			eCount++;
		}
	}
	
	while (nCount < nStart){
		//Add a new newline agent at the randomly generated x/y coordinates 
		//if it is unoccupied and the maximum number of agents has not been reached.
		row = random();
        	row %= dim;
        	col = random();
       		col %= dim;
		if (board[row][col].style == '.'){
			board[row][col].style = 'n';
			nCount++;
		}
	}

}

int friendCheck(char style1, char style2){
	//Quick check to see if a neighbor is a friend or not, returns 1 for friend and 0 for non-friend.
	if (style1 == style2)
		return 1;
	else
		return 0;
}

int enemyCheck(char style1, char style2){
	//Quick check to see if a neighbor is an enemy, returns 1 for enemy and 0 for non enemy.
	if (style1 != style2 && style2 != '.')
		return 1;
	else
		return 0;
}




void updateSingleHappiness(int thisRow, int thisCol, int dim, Person board[][dim]){
	/*
	@param thisRow: the row that the measured agent is on
	@param thisCol: the col that the measured agent is on
	@param dim: size of the board
	This method calculates a single agent's happiness value from their 
	neighbors given the agent's address and the game board.
	*/
	
	//Figure out the index for cardinal directions
	int left = thisRow - 1;
    	int right = thisRow + 1;
    	int up = thisCol - 1;
    	int down = thisCol + 1;
		
	//Initialize to zero so we can perform calculations later
	float friends = 0;
	float enemies = 0;
	float neighbors = 0;

	
	//Replacement boolean, 1 means valid and 0 means invalid.
	int checkLeft = 1;
	int checkRight = 1;
	int checkUp = 1;
	int checkDown = 1;
	
	//Grab the style of the original person whose happiness we are calculating
	char style = board[thisRow][thisCol].style;

	//If we go off the edge, then set the "boolean" checks to zero as we do not wrap around.
	if (left == -1)
        	checkLeft = 0;
    	if (right == dim)
        	checkRight = 0;
    	if (up == -1)
        	checkUp = 0;
    	if (down == dim)
		checkDown = 0;

	if (checkUp == 1){
		//if checkUp is one, then it means we are not at the zero index for 
		//rows and have neighbors above us. 
		if (checkLeft == 1){
			//If checkleft is one, then it means that we are not at the 0 
			//index for columns and have neighbors to the left.
			neighbors++;
			//Increase the number of neighbors by 1.
			
			friends += friendCheck(style,  board[left][up].style);
			//Determine if the neighbor is a friend, if so, add one to the friend count.
			enemies += enemyCheck(style,  board[left][up].style);
			//Determine if the neighbor is an enemy, if so, add one to the enemy count.
		}

		if (checkRight == 1){
			//This follows the same format as the above checkUp/checkLeft section.
			neighbors++;
			friends += friendCheck(style, board[right][up].style);
			enemies += enemyCheck(style, board[right][up].style);
		}
		neighbors++;
		friends += friendCheck(style, board[thisRow][up].style);
		enemies += enemyCheck(style, board[thisRow][up].style);
	}


	if (checkDown == 1){
		if (checkLeft == 1){
			neighbors++;
			friends += friendCheck(style, board[left][down].style);
			enemies += enemyCheck(style, board[left][down].style);
		}
		if (checkRight == 1){
			neighbors++;
			friends += friendCheck(style, board[right][down].style);
			enemies += enemyCheck(style, board[right][down].style);
		}
		neighbors++;
		friends += friendCheck(style, board[thisRow][down].style);
		enemies += enemyCheck(style, board[thisRow][down].style);
	}
	

	if (checkLeft == 1){
		neighbors++;
		friends += friendCheck(style, board[left][thisCol].style);
		enemies += enemyCheck(style, board[left][thisCol].style);
	}
	
	if (checkRight == 1){
		neighbors++;
		friends += friendCheck(style, board[right][thisCol].style);
		enemies += enemyCheck(style, board[right][thisCol].style);
	}
	
	board[thisRow][thisCol].happiness = friends/ (friends + enemies);
	//Calculate happiness for this person by dividing their friends vs the 
	//total number of people around them, then set it to their happiness value.
	if (enemies == 0){
		//If there are no enemies, then default to fully happy.
		board[thisRow][thisCol].happiness = 1.0;
	}
}


void calculateAllHappiness(int dim, Person board[][dim]){
	//Calculate the happiness of the entire board, this method 
	//is just to divide up some work so main isn't so bloated.
	//Calls updateSingleHappiness on the whole board.
	for (int x = 0; x < dim; x++){
		for (int y = 0; y < dim; y++){
			if (board[x][y].style != '.'){
				//Only calculate if the block has a person in it.
				updateSingleHappiness (x, y, dim, board);
			}
		}
	}	
}

float averageHappiness(int dim, Person board[][dim]){
	//Calculate the average happiness of the entire board, then return it.
	float average = 0;
	int persons = 0;
	
	for (int x = 0; x < dim; x++){
		for (int y = 0; y < dim; y++){
			if (board[x][y].style != '.'){
				average += board[x][y].happiness;
				persons++;
			}
		}
	}	
	
	return average/persons;

}

int moveAgents(float prefStr, int vacCount, int dim, Person board[][dim]){
	/*
	@param dim: dimension size
	@Param board: the game board
	@param prefStr: The percentage of happiness that a person must meet, 
                        otherwise they will have to move at the next turn.
	@param vacCount: The total number of empty slots in a city.
	
	This method moves agents following this algorithm
		1. Go through the 2d array city and calculate the happiness for all occupied city blocks.
		2. Allocate a list (using malloc) of structs of type Location, which 
		   contains a x/y coordinate in integer form. (referred to as vacancies list)
		3. Find all the blocks in the "city" that are vacant (not occupied by either n or e) 
		   and put their address in vacancies list..
		4. Shuffle the vacancies list so that the Locations are randomly sorted.
		5. Set initial index for vacancy list back to 0
		6. Go through the 2d array representation of the "city" and for every individual that is unhappy:
			6a. Set the style of the address of the current indexed value of the vacancy list 
			    to the individual's style ('e' or 'n').
			6b. Set the original style of the individual to '.'
			6c. Increment vacancy index by 1
			6d. Increment total number of moves this turn by 1
		7. free() the vacancies list.
		8. Return the total number of moves.
	*/
	Location *vacancies = malloc(sizeof(Location) * (vacCount + 1));
	
	//allocate space for an array of struct Location called vacancies.
	//We want to dynamically allocate this so we can free it before this method is over as it is called every turn.
	
	int totalMoves = 0;
	//Total number of agent moves
	int index = 0;
	//Index that we use to traverse the vacancy list
	int emptySpaces = 0;
	//total number of empty spaces left on the vacancy list
	
	for (int x = 0; x < dim; x++){
		for (int y = 0; y < dim; y++){
			board[x][y].occupiedThisTurn = 0;
			if (board[x][y].style == '.'){
				//If a space is empty, increment emptySpaces by 1 and add it's coordinates 
				//to the indexed struct in the vacancy list.
				emptySpaces++;
				vacancies[index].xCoord = x;
				vacancies[index].yCoord = y;
				index++;
				//Increment index by 1 as we have filled one slot in the vacancy list.
			}
		}
	}
	
	for (int i = 0; i < vacCount; i++){
		//quick method to shuffle the vacancy list so that agents move to random vacant locations rather than in order.
		int j = (random()%vacCount) + 1;
		Location temp = vacancies[i];
		vacancies[i] = vacancies[j];
		vacancies[j] = temp;
	}
	
	
	index = 0;
	//Reset index to 0 so we can traverse through the vacancy list
	int xDest;
	int yDest;
	char temp;
	for (int x1 = 0; x1 < dim; x1++){
		for (int y1 = 0; y1 < dim; y1++){ 
			//printf("agent [%d][%d]", x1, y1);
			//printf(" has style of %c\n", board[x1][y1].style);
			//printf("Agent has happiness of %2.2f\n\n", board[x1][y1].happiness);
		
		
			//Look for blocks with individuals living in them that are unhappy(happiness below the prefStr variable)
			if (board[x1][y1].style != '.' 
				&& board[x1][y1].happiness < prefStr 
				&& index < emptySpaces 
				&& board[x1][y1].occupiedThisTurn == 0){
				//If we haven't gone through all the empty spaces yet then move 
				//the unhappy individual to the coordinates held by the indexed value of the vacancy list.
				//Then reset the style held by the original position to '.' as they have moved somewhere else.

				xDest = vacancies[index].xCoord;
				yDest = vacancies[index].yCoord;
				
				//printf("moving agent [%d][%d] to [%d][%d]\n", x1, y1, xDest, yDest);
				//printf("Agent has style of %c\n", board[x1][y1].style);
				//printf("Agent has happiness of %2.2f\n\n", board[x1][y1].happiness);
				temp = board[x1][y1].style;
				board[xDest][yDest].style = temp;
				board[x1][y1].style = '.';
				board[x1][y1].happiness = 1.0;
				board[xDest][yDest].occupiedThisTurn = 1;
				board[x1][y1].occupiedThisTurn = 1;
				
				index++;
				//Increment index so we keep track of how far down the vacancy list we've gone
				totalMoves++;
				//Increment total moves
			}
		}
	}
	free(vacancies);
	return totalMoves;
	//Return the total number of moves to be outputted.
	
}

int main(int argc, char *argv[]) {
	
	int eStart;
	//number of endline agents
	int nStart;
	//number of newline agents
	int vacCount;
	//number of empty slots
	int moveCount;
	
	int dim = 15;
	//Dimension of board, board must be a square. 
	int cycle = 0;
	//Number of cycles iterated through
	int totalCycles = -1;
	//Number of cycles to go through for non-infinite mode until termination
	int delay = 900000;
	//Delay for infinite mode before a new turn is executed.

	float prefStr = 50;
	//How strongly a person needs to like their neighbors to avoid moving.
	float vac = 20;
	//Percent of vacant slots in the city
	float nPercent = 60;
	//Percent of newline agents relative to endline agents
	float ePercent;
	//Percent of endline agents relative to newline agents
	float currentHappiness;
	//Calculated average happiness of the city.
	
	int param;
	//The below section deals with the optional parameters a user can input.
	while ((param = getopt (argc, argv, "ht:c:d:s:v:e:")) != -1){
    	switch (param){
	      	case 'h':
				fprintf( stderr, "usage:\n");
				fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");
				fprintf( stderr, "Option  	Default	Example	Description\n");
				fprintf( stderr, "'-h'    	NA	-h	print this usage message. \n");
				fprintf( stderr, "'-t N'  	900000	-t 5000	microseconds cycle delay. \n");
				fprintf( stderr, "'-c N'   	-1	-c4	count cycle maximum value. \n");
				fprintf( stderr, "'-d dim'	15	-d 7	width and height dimension. \n");
				fprintf( stderr, "'-s %%str'	50	-s 30	strength of preference. \n");
				fprintf( stderr, "'-v %%vac'	20	-v30	percent vacancies. \n");
				fprintf( stderr, "'-e %%end'	60	-e75	percent Endline braces. Others want Newline. \n");
				return(1 + EXIT_FAILURE );
		        break;
			case 't':
				delay = atoi(optarg);
				if (delay < 1 ){
					fprintf( stderr, "A value for delay was entered that was less than one :%d\n", delay);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );
				}
 	        	break;
      		case 'c':
				totalCycles = atoi(optarg);
				if (totalCycles <= 0){
					fprintf( stderr, "count (%d) must be a non-negative integer.\n", totalCycles);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );
				}
        		break;
			case 'd':
				dim = atoi(optarg);
				if (dim > 39 || dim < 5){
					fprintf( stderr, "dimension (%d) must be a value in [5...39]\n", dim);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );
				}
        		break;	
			case 's':
				prefStr = atof(optarg);
				if (prefStr > 99 || prefStr < 1){
					fprintf( stderr, "preference strength (%1f) must be a value in [1...99]\n", prefStr);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );	
				}
        		break;
			case 'v':
				vac = atof(optarg);
				if (vac < 1 || vac > 99){
					fprintf( stderr, "vacancy strength (%1f) must be a value in [1...99]\n", vac);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );
				}
        		break;
        	case 'e':
				nPercent = atof(optarg);
				if (nPercent > 99 || nPercent < 1){
					fprintf( stderr, "endline proportion (%1f) must be a value in [1...99]\n", nPercent);
					fprintf( stderr, "usage:\n");
					fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");	
					return(1 + EXIT_FAILURE );
				}
				break;
			case '?':
				fprintf( stderr, "usage:\n");
				fprintf( stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] [-e %%end]\n");
				return(1 + EXIT_FAILURE );	
				break;
			
		}
	}

	prefStr = prefStr/100;
	//Divide prefStr by 100 so we can get a decimal representation of the percentage (10% -> .1)
	vac = vac/100;
	//Divide vacancies by 100 so we can get a decimal representation of the percentage (10% -> .1)
	ePercent = (100-nPercent)/100;
	//Find the percentage of endline users by subtracting the number of newline users from 100 
	//then dividing it by 100 to get the decimal reprsentation
	nPercent = nPercent/100;
	//Divide nPercent by 100 so we can get a decimal representation of the percentage (10% -> .1)
	
	vacCount = vac * dim * dim;
	//Figure out how many vacancies are in the city by multiplying the vacancy percentage by total size.
	nStart = ((dim * dim)-vacCount) * nPercent;
	//Calculate out the number of newline users by getting the total number of slots in the city, 
	//then multiply the total slots by the number of slots to be occupied by people, 
	//then multiply that value by the percent of the city that are newline users.
	eStart = ((dim * dim)-vacCount) * ePercent;
	//Same as for newline
	
	vacCount += (dim * dim) - vacCount - nStart - eStart;
	
	//printf("%d\n", dim*dim);
	//printf("%d\n", vacCount);
	//printf("%d\n", nStart);
	//printf("%d\n", eStart);
	


	Person board[dim][dim];
	initBoard(eStart, nStart, dim, board);
	//create and initialize the board
	
	
	if (totalCycles > 0){
		//If there was an inputted -c with a valid number of cycles, then do the following
		while (cycle <= totalCycles){
			//loop for every cycle until cyclelimit reached
			calculateAllHappiness(dim, board);
			//calculate the happiness of every person in the city
			currentHappiness = averageHappiness(dim, board);
			//Get the average happiness in the city and set currentHappiness to it

			
			moveCount = 0;
			if (cycle != 0){
				moveCount = moveAgents(prefStr, vacCount, dim, board);
			}
			
			

			printstate(dim, board);
			//print the game state
			

			//Move all the agents in the city and grab the returned moveCount value
			printf("Cycle: %d\n", cycle);
			//Print the current cycle number.
			printf("Number of agent moves: %d\n", moveCount);
			//Print the number of agent moves this turn
			printf("Average happiness: %2.2f\n", currentHappiness);
			//Print the average happiness of the city this turn after moves have been made
			printf("Dimensions: %d, Preference Str: %2.2f, Vacancy Percentage: %2.2f, Newline Percentage: %2.2f\n\n", dim, prefStr, vac, nPercent);
			//Print basic simulation parameters
			cycle++;
			//Increment the cycle by 1.
		}
	}
	if (totalCycles <= 0){
		//If a total cycle value was not given
		while (1){
			//Continuously simulate the game, clearing the display for every turn until user uses CONTROL+C to terminate.
			clear();
			//Clear the display
			set_cur_pos(0,0);
			//Set the print position to start at the top left of the screen.
			
			calculateAllHappiness(dim, board);
			//calculate the happiness of every person in the city
			currentHappiness = averageHappiness(dim, board);
			//Get the average happiness in the city and set currentHappiness to it
			moveCount = 0;
			if (cycle != 0){
				moveCount = moveAgents(prefStr, vacCount, dim, board);
			}
			
			
			printstate(dim, board);
			//Print the gamestate

			//Move all the agents in the city and grab the returned moveCount value
			printf("Cycle: %d\n", cycle);
			//Print the current cycle number.
			printf("Number of agent moves: %d\n", moveCount);
			//Print the number of agent moves this turn
			printf("Average happiness: %2.2f\n", currentHappiness);
			//Print the average happiness of the city this turn after moves have been made
			printf("Dimensions: %d, Preference Str: %2.2f, Vacancy Percentage: %2.2f, Newline Percentage: %2.2f\n\n", dim, prefStr, vac, nPercent);
			//Print basic simulation parameters
			puts("Use Control-C to quit.");
			//Remind users how to exit.
			usleep(delay);
			//Delay the next turn by "delay" microseconds. Default is 900000.
			cycle++;
			//Increase cycle by 1
			
		}
	}
	return 0;
}
