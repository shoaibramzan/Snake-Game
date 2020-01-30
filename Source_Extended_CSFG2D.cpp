//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#include <fstream>
using namespace std;

//include our own libraries
#include "RandomUtils.h"							//for seed, random
#include "ConsoleUtils.h"							//for clrscr, gotoxy, etc.
#include "TimeUtils.h"								//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(12);								//horizontal dimension
const int  SIZEY(10);								//vertical dimension

//Two Player Game mode variables
const int TwoPlayerMazeSIZEY(13);
const int TwoPlayerMazeSIZEX(17);

//defining symbols used for display of the grid and content
const char HEAD('0');   							//HEAD
const char p2HEAD('O');								//player2 HEAD
const char MOUSE('@');								//MOUSE
const char PILL('+');								//PILL
const char MONGOOSE('M');							//MONGOOSE
const char TAIL('o');								//TAIL
const char TUNNEL(' ');    							//tunnel
const char WALL('#');    							//border

//defining the command letters to move the snake on the maze
const int  UP(72);									//up arrow
const int  DOWN(80); 								//down arrow
const int  RIGHT(77);								//right arrow
const int  LEFT(75);								//left arrow

//defining the other command letters
const char QUIT('Q');								//to end the game
const char CHEAT('C');
const int max = 3;

struct TwoPlayerGameData
{
	bool playerDied = false;
	string playerName;
	int playerScore = 0;

}player1, player2;

struct GameData	//Players data
{
	int mouseCollected = 0;
	string name;
	int score;
	bool invincible = false;						// Enables and disables effects of invincibility
	bool cheatEnabled = false;
	bool playerCheated = false;						//Checks if the player cheated at any point during the game
	bool playerDied = false;
	bool timeIsUp = false;
	int highScore;
	bool pillCollected = false;						//Used to disable pill in the grid when the amount of mice collected is not 2,4 or 6
	int amountOfMoves = 10;
	int InvincibleMoves = 0;						//Counts the moves taken when invincible, disables invincibility at 20 moves
    int minutes = 3;									//Used for countdown timer
    int seconds = 30;									//Used for countdown timer
	
}player;

struct Item {
    int x, y;
    char symbol;
};

struct GameItems
{
	Item mouse = { 0, 0, MOUSE };					//Mouse's position and symbol
	Item pill = { 0, 0, PILL };						//Power Up Pill's position and symbol
	Item mongoose = { 0,0, MONGOOSE };
}gameItems;

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
    //function declarations (prototypes)

    void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& snake, GameItems &gameItems, GameData &player);
    void renderGame(const char g[][SIZEX],string& mess, GameData &player);
    void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& snake, GameItems &gameItems, GameData &player);
    void updateGame(char g[][SIZEX], const char m[][SIZEX], vector<Item>& s, const int kc, string& mess, GameData &player, GameItems &gameItems);
	void playerName(GameData &player);
    bool wantsToQuit(const int key);
	bool wantsToCheat(const int key);
    bool isArrowKey(const int k);
	void storeScore(GameData player);
	void checkPlayerScore(GameData &player);
    int  getKeyPress();
    void endProgram();
    void youWin();
    void die();
    void displayClock(GameData player);
    void timer(GameData &player);
    void save();
    void load(GameData &player);					//Need to use struct to store all the variables
	bool IsTwoPlayerGame = false;

    //Load function loads the saved variable values from saved game file
	void TwoPlayerGame(bool& IsTwoPlayerGame);		//Asks the user if they want to play 1 player mode or 2 player mode
    void twoPlayerMode();							//The two player game mode
    TwoPlayerGame(IsTwoPlayerGame);
    if (IsTwoPlayerGame)
    {
        twoPlayerMode();							//loads the 2 player game mode instead of the normal game mode
        return 0;
    }

    //local variable declarations 
    char grid[SIZEY][SIZEX];						//grid for display
    char maze[SIZEY][SIZEX];						//structure of the maze
    char tailArray[max] = { TAIL,TAIL ,TAIL};
   	vector<Item> snake = { {0,0,HEAD},{0,0,TAIL},{ 0,0,TAIL },{ 0,0,TAIL } };  //Tail's position and symbol
    playerName(player);

    ifstream fin;
    fin.open(player.name + ".save", ios::out);		//Checks if player has saved data
    if (!fin.fail())
    {
        load(player);								//Loads data from save
    }
	checkPlayerScore(player);
	string message("LET'S START...");				//current message to player

													//action...
    seed();											//seed the random number generator

	setNewConsoleTitle("FoP Group Project 2019 - Task 1c - Snake Game");
    initialiseGame(grid, maze, snake, gameItems, player);	//initialise grid (incl. walls and snake)
    int key;										//current key selected by player
    do 
	{
		renderGame(grid, message, player);			//display game info, modified grid and messages 
        displayClock(player);								//Displays the timer on the console
        key = toupper(getKeyPress()); 				//read in  selected key: arrow or letter command
        timer(player);									//Decreases timer after the players first move

		if (isArrowKey(key))
		{
			updateGame(grid, maze, snake, key, message, player, gameItems);
		}

		else if (wantsToCheat(key))
		{
            player.playerCheated = true;			//Detects that the player has cheated during the game
            if (player.cheatEnabled)				//If cheat mode has already been enabled
            {
                message = "CHEAT MODE DEACTIVATED"; //Displays to the player that they have deactivated cheat mode
                player.cheatEnabled = false;        //Disables cheat mode
            }
            else
            {
                message = "CHEAT MODE ACTIVATED!";  //Displays to the player that they have activated cheat mode
                cout << '\a' << '\a' << '\a';		//Beeps 3 times letting player know cheat mode is activated
                player.cheatEnabled = true;         //Enables cheat mode

                for (int i(snake.size()); i > max + 1; --i)
                {
                    snake.pop_back();				//Resets the snake back to it's original size when enabling cheat mode
                }
            }
		}

		if(!isArrowKey(key) && player.cheatEnabled && !player.cheatEnabled)
		{
           message = "INVALID KEY!";				//set 'Invalid key' message
		}

		if (wantsToQuit(key))
		{
			message = "PLAYER WANTS TO QUIT!";
		}

        if (player.mouseCollected == 7)
        {
            message = "YOU WIN!";
        }

        if (player.minutes == 0 && player.seconds == 0)			//Pauses the game
            message = "TIME OVER, YOU LOST";

        if (key == 'S')
        {
            message = "SAVED GAME";
            save();
        }

    } while (!wantsToQuit(key) && player.mouseCollected != 7 && !player.playerDied && !player.timeIsUp);		//while user does not want to quit
    renderGame(grid, message,player);				//display game info, modified grid and messages
    if(!player.playerCheated || !(player.minutes == 0 && player.seconds == 0))      //Doesn't save the score if the player has cheated during the game or if time is 0:00
	    storeScore(player);
    if(wantsToQuit(key))
        endProgram();								//display final message when player quits the game
    if (player.mouseCollected == 7)
        youWin();									//display final message when player collects all mice
    if (player.playerDied)
        die();										//display final message when player die's from hitting wall etc.
	if (player.timeIsUp)
		die();
    return 0;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& snake, GameItems &gameItems, GameData &player)
{ //initialise grid and place snake in middle
    void setInitialMazeStructure(char maze[][SIZEX]);
    void setSnakeInitialCoordinates(const char maze[][SIZEX], vector<Item>& snake);
    void setTailInitialCoordinates(vector<Item>& snake);
    void SetMouseCoordinates(const char maze[][SIZEX],GameItems &gameItems);
    void SetPillCoordinates(const char maze[][SIZEX], GameItems &gameItems);
    void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Item>& s, GameItems &gameItems, GameData &player);
    

    setInitialMazeStructure(maze);					//initialise maze
    setSnakeInitialCoordinates(maze,snake);
    updateGrid(grid, maze, snake, gameItems, player);		//prepare grid
    SetMouseCoordinates(maze,gameItems);
    updateGrid(grid, maze, snake, gameItems, player);		//prepare grid
    SetPillCoordinates(maze, gameItems);
}

void setSnakeInitialCoordinates(const char maze[][SIZEX], vector<Item>& snake)
{
    //set snake coordinates inside the grid at random at beginning of game
    snake.at(0).y = random(SIZEY - 2);
    snake.at(0).x = random(SIZEX - 2);

    while (maze[snake.at(0).y][snake.at(0).x] == WALL)          //Getting new starting coordinates for snake when the current coordinates are placed in walls
    {
        snake.at(0).y = random(SIZEY - 2);
        snake.at(0).x = random(SIZEX - 2);
    }
    
    for (int i(1); i < snake.size(); ++i)
    {
        snake.at(i).y = snake.at(0).y;
        snake.at(i).x = snake.at(0).x;
    }
}

void SetMouseCoordinates(const char maze[][SIZEX], GameItems &gameItems)
{
    gameItems.mouse.x = random(SIZEX - 2);
    gameItems.mouse.y = random(SIZEY - 2);

    while (maze[gameItems.mouse.y][gameItems.mouse.x] != TUNNEL)        //Getting new coordinates for mouse when the current coordinates are placed in walls
    {
		gameItems.mouse.y = random(SIZEY - 2);
		gameItems.mouse.x = random(SIZEX - 2);
    }
}

void SetPillCoordinates(const char maze[][SIZEX], GameItems &gameItems)
{
     gameItems.pill.x = random(SIZEX - 2);
	 gameItems.pill.y = random(SIZEY - 2);

     while (maze[gameItems.pill.y][gameItems.pill.x] != TUNNEL)         //Getting new coordinates for pill when the current coordinates are not placed in a tunnel
     {
		 gameItems.pill.y = random(SIZEY - 2);
		 gameItems.pill.x = random(SIZEX - 2);
     }

     gameItems.mongoose.x = random(SIZEX - 2);
	 gameItems.mongoose.y = random(SIZEY - 2);

     while (maze[gameItems.mongoose.y][gameItems.mongoose.x] != TUNNEL) //Getting new coordinates for mongoose when the current coordinates are placed in walls
     {
		 gameItems.mongoose.y = random(SIZEY - 2);
		 gameItems.mongoose.x = random(SIZEX - 2);
     }
}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
  //initialise maze configuration
    char initialMaze[SIZEY][SIZEX] 					//local array to store the maze structure
        = { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', '#', ' ', ' ', ' ', '#', '#', ' ', ' ', '#' },
        { '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', '#' },
        { '#', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };
    //with '#' for wall, ' ' for tunnel, etc. 
    //copy into maze structure with appropriate symbols
    for (int row(0); row < SIZEY; ++row)
        for (int col(0); col < SIZEX; ++col)
            switch (initialMaze[row][col])
            {
                //not a direct copy, in case the symbols used are changed
            case '#': maze[row][col] = WALL; break;
            case ' ': maze[row][col] = TUNNEL; break;
            }
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char grid[][SIZEX], const char maze[][SIZEX], vector<Item>& snake, const int keyCode, string& mess, GameData &player, GameItems &gameItems)
{ //update game
	void updateGameData(const char g[][SIZEX], vector<Item>& s, const int kc, string& m, GameItems &gameItems, GameData &player);
    void updateGrid(char g[][SIZEX], const char maze[][SIZEX], const vector<Item>& s, GameItems &gameItems, GameData &player);
    updateGameData(grid, snake, keyCode, mess, gameItems, player);		//move snake in required direction
    updateGrid(grid, maze, snake, gameItems, player);					//update grid information
	
}
void updateGameData(const char g[][SIZEX], vector<Item>& snake, const int key, string& mess, GameItems &gameItems, GameData &player)
{ //move snake in required direction
    void die();
    bool isArrowKey(const int k);
    void setKeyDirection(int k, int& dx, int& dy);
	void updateScore(GameData &player);
    void becomeInvincible();						//Enables the invincible state
    void moveMongoose(const char g[][SIZEX], GameItems &gameItems);
    assert(isArrowKey(key));
	void moveSnake(vector<Item>& snake, int dx, int dy);
	void increaseSnakeTail(vector<Item>& snake);
	void moveMouse(const char g[][SIZEX], GameItems &gameItems);
	void movePill(const char g[][SIZEX], GameItems &gameItems);

    //reset message to blank
    mess = "";

    //calculate direction of movement for given key
    int dx(0), dy(0);
    setKeyDirection(key, dx, dy);

    //check new target position in grid and update game data (incl. snake coordinates) if move is possible
    switch (g[snake.at(0).y + dy][snake.at(0).x + dx])
    {			//...depending on what's on the target position in grid...
    case TUNNEL:									//can move
		moveSnake(snake, dx, dy);
        
        if(!player.playerCheated)					//Stops the player from scoring any more if they have cheated
            updateScore(player);
        
        
        if (player.invincible)						//Starts counting snake moves only if in invincible state
        {
            ++player.InvincibleMoves;
        }

        if (player.InvincibleMoves > 20)			//Disables the invincible state
            player.invincible = false;

        moveMongoose(g, gameItems);					//Moves the mongoose in a random direction on grid
        break;
    case WALL:  									//hit a wall and stay there

        if (!player.invincible)						//Disables end of game when invincible
        {
            mess = "GAME OVER. YOU LOSE!";
            player.playerDied = true;
        }

        if (player.invincible)						//Enables player to wrap through outer walls to other side
        {       //Requires detection of keys to stop horizontal corners from not working properly
            if (snake.at(0).x >= SIZEY && key == RIGHT)     //Moving through right border of the play area
            {
                for (int i(snake.size() - 1); i >= 1; --i)
                {
                    snake.at(i).y = snake.at(i - 1).y;      //Gets the tail following correctly

                    snake.at(i).x = snake.at(i - 1).x;
                }
                snake.at(0).x = 1;					//Puts head at left border of the play area
            }
            else if (snake.at(0).x == 1 && key == LEFT)    //Moving through left border of the play area
            {
                for (int i(snake.size() - 1); i >= 1; --i)
                {
                    snake.at(i).y = snake.at(i - 1).y;  //Gets the tail following correctly
                    snake.at(i).x = snake.at(i - 1).x;
                }
                snake.at(0).x = SIZEY;				//Puts snake head at right border of the play area
            }

            else if (snake.at(0).y == 1 && key == UP)    //Moving through the upper border of the play area
            {
                for (int i(snake.size() - 1); i >= 1; --i)
                {
                    snake.at(i).y = snake.at(i - 1).y;
                    snake.at(i).x = snake.at(i - 1).x;
                }
                snake.at(0).y = 8;					//Puts snake head at lower border of play area
            }
            else if (snake.at(0).y == SIZEY-2 && key == DOWN)    //Moving through the bottom border of the play area
            {
                for (int i(snake.size() - 1); i >= 1; --i)
                {
                    snake.at(i).y = snake.at(i - 1).y;
                    snake.at(i).x = snake.at(i - 1).x;
                }
                snake.at(0).y = 1;					//Puts snake head at upper border of the play area
            }

        }

        break;
    case MOUSE:
        mess = "MOUSE CAUGHT!";
		moveSnake(snake, dx, dy);

        if(!player.cheatEnabled)					//Only adds to the tail when the cheat function is not enabled
        { 
			increaseSnakeTail(snake);
        }
        
		moveMouse(g, gameItems);

        ++player.mouseCollected;
        player.pillCollected = false;				//Enables the pill to be collected by the snake when the amount of mice colleted equals 2,4 or 6
        player.amountOfMoves = 10;					//Resets the amount of moves done by the snake

        if (player.invincible)						//Counts snake moves while invincible
        {
            ++player.InvincibleMoves;
        }
        break;
    case TAIL:
        if (!player.invincible)						//Prevents end of game when invincible
        {
            mess = "GAME OVER. YOU LOSE!";
            player.playerDied = true;
        }
        break;

    case PILL:
        for (int i(snake.size()); i > max+1; --i)
        {
            snake.pop_back();						//Resets the snake back to it's original size when collecting a pill
        }

		moveSnake(snake, dx, dy);
		movePill(g, gameItems);
       
       player.pillCollected = true;					//Disables the pill from appearing until a mouse has been collected
       player.amountOfMoves = 10;					//Resets the amount of moves done by snake
       if (player.invincible)						//Counts snake moves while invincible
       {
           ++player.InvincibleMoves;
       }
       becomeInvincible();							//Makes the snake invincible for 20 moves
       break;
       
    case MONGOOSE:
        mess = "KILLED BY MONGOOSE";
        player.playerDied = true;
        break;
    }
}
//---------------------------------------------------------------------------
//----------------- Snake and Mouse functions
//---------------------------------------------------------------------------

void moveSnake(vector<Item>& snake, int dx, int dy)
{
	for (int i(snake.size() - 1); i >= 1; --i)
	{
		snake.at(i).y = snake.at(i - 1).y;
		snake.at(i).x = snake.at(i - 1).x;

	}
	snake.at(0).y += dy;						//go in that Y direction
	snake.at(0).x += dx;						//go in that X direction
}

void increaseSnakeTail(vector<Item>& snake)
{
	snake.push_back({ snake.at(snake.size() - 1).x,snake.at(snake.size() - 1).y,TAIL });
	snake.push_back({ snake.at(snake.size() - 1).x,snake.at(snake.size() - 1).y,TAIL });
}

void moveMouse(const char g[][SIZEX], GameItems &gameItems)
{
	gameItems.mouse.y = random(SIZEY - 2);
	gameItems.mouse.x = random(SIZEX - 2);
	while (g[gameItems.mouse.y][gameItems.mouse.x] != TUNNEL)
	{
		gameItems.mouse.y = random(SIZEY - 2);
		gameItems.mouse.x = random(SIZEX - 2);
	}
}

void movePill(const char g[][SIZEX], GameItems &gameItems)
{
	gameItems.pill.y = random(SIZEY - 2);
	gameItems.pill.x = random(SIZEX - 2);

	while (g[gameItems.pill.x][gameItems.pill.y] != TUNNEL)
	{
		gameItems.pill.y = random(SIZEY - 2);
		gameItems.pill.x = random(SIZEX - 2);
	}

}

//---------------------------------------------------------------------------
//----------------- Rest of the code
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Item>& snake, GameItems &gameItems, GameData &player)
{ //update grid configuration after each move
    void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
    void placeItem(char g[][SIZEX], const Item& item);
    void placeItem(char g[][SIZEX], GameItems &gameItems, GameData &player);
    void placeMongoose(char g[][SIZEX],GameData &player, GameItems &gameItems);

    placeMaze(grid, maze);
    for (int i(snake.size() - 1); i >= 0; --i)		//cant be size_t because its an unsigned integer!
    {
        placeItem(grid, snake.at(i));				//set snake in grid
    }
    placeMongoose(grid, player, gameItems);
    placeItem(grid, gameItems, player);			//set snake in grid
}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
        for (int row(0); row < SIZEY; ++row)
            for (int col(0); col < SIZEX; ++col)
                grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
  g[item.y][item.x] = item.symbol;
}
void placeItem(char g[][SIZEX],GameItems &gameItems, GameData &player)
{
    if (player.mouseCollected < 7)					//Doesn't render the mouse in the grid if the maximum amount of mice have been collected
    {
        g[gameItems.mouse.y][gameItems.mouse.x] = gameItems.mouse.symbol;
    }

    if ((player.mouseCollected == 2 || player.mouseCollected == 4 || player.mouseCollected == 6) && player.pillCollected == false && player.amountOfMoves > 0)
    {
        while (g[gameItems.pill.x][gameItems.pill.y] != TUNNEL)
        {
			gameItems.pill.y = random(SIZEY - 2);
			gameItems.pill.x = random(SIZEX - 2);
        }
        g[gameItems.pill.x][gameItems.pill.y] = gameItems.pill.symbol;			//Places the pill in the grid

        --player.amountOfMoves;                     //Counts each move that the player does and removes the pill after 10 moves
    }
}

void placeMongoose(char g[][SIZEX], GameData &player, GameItems &gameItems)    //Places the mongoose in the grid
{
    if (player.mouseCollected >= 3)					//Only spawns mongoose after 3 mice have been collected
    {
        while (g[gameItems.mongoose.y][gameItems.mongoose.x] == WALL)   //Prevents mongoose from spawning in wall
        {
            gameItems.mongoose.y = random(SIZEY - 2);
			gameItems.mongoose.x = random(SIZEX - 2);
        }
            g[gameItems.mongoose.y][gameItems.mongoose.x] = gameItems.mongoose.symbol;
    }
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
    bool isArrowKey(const int k);
    assert(isArrowKey(key));
    switch (key)									//...depending on the selected key...
    {
    case LEFT:  									//when LEFT arrow pressed...
        dx = -1;									//decrease the X coordinate
        dy = 0;
        break;
    case RIGHT: 									//when RIGHT arrow pressed...
        dx = +1;									//increase the X coordinate
        dy = 0;
        break;
    case UP:
        dx = 0;
        dy = -1;
        break;
    case DOWN:
        dx = 0;
        dy = +1;
        break;
    }
}

int getKeyPress()
{													//get key or command selected by user
    int keyPressed;
    keyPressed = _getch();							//read in the selected arrow key or command letter
    while (keyPressed == 224) 						//ignore symbol following cursor key
        keyPressed = _getch();
    return keyPressed;
}

bool isArrowKey(const int key)
{													//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
    return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);

}
bool wantsToQuit(const int key)
{													//check if the user wants to quit (when key is 'Q' or 'q')
    return toupper(key) == QUIT;
}

bool wantsToCheat(const int key)
{
	return toupper(key) == CHEAT;					//check if the user wants to cheat (when key is 'C' or 'c')
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(int x)
{	//convert an integer to a string
    std::ostringstream os;
    os << x;
    return os.str();
}
string tostring(char x)
{	//convert a char to a string
    std::ostringstream os;
    os << x;
    return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message)
{	//display a string using specified colour at a given position 
    gotoxy(x, y);
    selectBackColour(backColour);
    selectTextColour(textColour);
    cout << message + string(40 - message.length(), ' ');
}

void renderGame(const char g[][SIZEX], string& mess,GameData &player)
{ //display game title, messages, maze, snake and other items on screen
    string tostring(char x);
    string tostring(int x);
    //string tostring(int mouseCollected);
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    void paintGrid(const char g[][SIZEX]);			//Used for 1 player mode only

    //display game title
    showMessage(clBlack, clYellow, 0, 0, "___SNAKE GAME___");
    showMessage(clYellow, clBlack, 40, 0, "FoP Task 1c - February 2019   ");
    showMessage(clYellow, clBlack, 40, 1, getDate() + " " + getTime());
    showMessage(clYellow, clBlack, 40, 3, "Group: CS4G2 D      ");
    showMessage(clYellow, clBlack, 40, 4, "Adam   Student no: 27025936");
    showMessage(clYellow, clBlack, 40, 5, "Corey  Student no: 28006767");
    showMessage(clYellow, clBlack, 40, 6, "Shoaib Student no: 28011183");
    //display menu options available
    showMessage(clRed, clYellow, 40, 8, "TO MOVE   - USE KEYBOARD ARROWS ");
    showMessage(clRed, clYellow, 40, 9, "TO QUIT   - PRESS 'Q'           ");

    if(player.cheatEnabled)
        showMessage(clRed, clYellow, 40, 10, "TO DISABLE CHEAT MODE - PRESS 'C'    ");  //Only shows message when cheat mode is enabled
    else showMessage(clRed, clYellow, 40, 10, "TO CHEAT  - PRESS 'C'           ");      //Only shows message when cheat mode is disabled

    showMessage(clRed, clYellow, 40, 11, "SAVE GAME - PRESS 'S'  ");
	showMessage(clRed, clYellow, 40, 12, "PLAYER'S NAME    : " + player.name);
	showMessage(clRed, clYellow, 40, 13, "BEST SCORE SO FAR: " + tostring(player.highScore));
	showMessage(clRed, clYellow, 40, 14, "CURRENT SCORE    : " + tostring(player.score));
    showMessage(clRed, clYellow, 40, 15, "MICE COLLECTED   : " + tostring(player.mouseCollected));
	
    //print auxiliary messages if any
    showMessage(clBlack, clWhite, 40, 18, mess);	//display current message

    if(player.invincible) 
    showMessage(clRed, clYellow, 40, 24, "INVINCIBLE SNAKE");       //Only shows message while invincible
    else showMessage(clBlack, clBlack, 40, 24, "                         ");    //Only shows message while not invincible


                                                    //display grid contents
    paintGrid(g);
}

void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
    selectBackColour(clBlack);
    selectTextColour(clWhite);

    gotoxy(0, 2);
    for (int row(0); row < SIZEY; ++row)
    {
        for (int col(0); col < SIZEX; ++col)
        {
            if (g[row][col] == '0')					//Changes colour of snake head
            {
                selectTextColour(clRed);
                if (player.invincible && player.InvincibleMoves < 20) //Changes snake colour to blue when invincible
                {
                    selectTextColour(clBlue);
                }
            }

            if (g[row][col] == 'o')					//Changes colour of snake body
            {
                selectTextColour(clGreen);
                if (player.invincible && player.InvincibleMoves <= 20) //Changes snake colour to blue when invincible
                {
                    selectTextColour(clBlue);
                }
            }
            if (g[row][col] == '@' || g[row][col] == '+')   //Changes colour of pill and mouse
                selectTextColour(clYellow);

            if (g[row][col] == 'M')					//Changes colour of mongoose to red
                selectTextColour(clRed);   

            cout << g[row][col];					//output cell content
            selectTextColour(clWhite);
        }
        cout << endl;
    }
}
void playerName(GameData &player)
{

	int  MAXNAME = 20;
	selectTextColour(clRed);
	do
	{

		cout << "\n\n\n\n\n\n\n		       ENTER PLAYER NAME (max 20 characters): ";
		cin >> player.name;


		system("CLS");

	} while (player.name.length() > MAXNAME);

}
void updateScore(GameData &player)
{
	
	++player.score;

}

void checkPlayerScore(GameData &player)
{
	fstream fileRead;								// defines the file variable
	fileRead.open(player.name + ".txt", ios::in);   // Opens the file that a player has  
	if (fileRead.fail())							// if the file open fails then sets the highScore to 500
	{
		player.highScore = 500;
	}
	else
	{
		fileRead >> player.highScore;				// Sets the players previousScore as Best Score so far
	}
}
void endProgram()									//When the player chooses to quit the game
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 20, "QUITTING GAME...\n");
    system("pause");								//hold output screen until a keyboard key is hit
    return;
}

void youWin()										//When the player has collected all of the mice
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 20, "YOU COLLECTED ALL THE MICE...\n");
    system("pause");								//hold output screen until a keyboard key is hit
    return;
}

void die() 
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 23, "YOU WERE KILLED...");
	system("pause");
}

void becomeInvincible()
{
    player.invincible = true;						//Enables invincibility
    player.InvincibleMoves = 0;						//Resets moves of snake when invincible
}
void storeScore (GameData player) 
{		
	ifstream fin;
	fin.open(player.name + ".txt", ios::out);		//Declares the file under the player name for input
	if (fin.fail())
	{
		//stores new score from the player
		ofstream fout(player.name + ".txt");
		if (fout.fail())							//Allows us to check whether the open operation has been successful
			cout << "\nError: file couldn't be created";
		else
		{
			fout << player.score;
			fout.close();
		}
	}
	else
	{
		//read in old score and update it to new score 
		int previousScore;
		fin >> previousScore;
		if (previousScore > player.score)
		{
			fin.close();
			ofstream fout(player.name + ".txt");
			if (fout.fail())
				cout << "\nError: file couldn't be opened";
			else
			{
				fout << player.score;
				fout.close();						//Closes the file and updated score is written to the file
			}
		}
		else
			fin.close();							//Closes the file
	}

}

void moveMongoose(const char g[][SIZEX], GameItems &gameItems)    //Moves the mongoose when the player moves in a tunnel
{   
	int mongooseMovement = rand() % 5;				//Chooses random movement for mongoose
    switch (mongooseMovement)
    {
    case 0:											//Mongoose keeps still
        gameItems.mongoose.y += 0;
		gameItems.mongoose.x += 0;
        break;
    case 1:											//Mongoose goes down
        if (g[gameItems.mongoose.y+1][gameItems.mongoose.x] == TUNNEL)  //Keeps the mongoose out of the walls
        {
        gameItems.mongoose.y += 1;
		gameItems.mongoose.x += 0;
        }
        break;
    case 2:											//Mongoose goes up
        if (g[gameItems.mongoose.y - 1][gameItems.mongoose.x] == TUNNEL)
        {
		gameItems.mongoose.y -= 1;
		gameItems.mongoose.x += 0;
        }
        break;
    case 3:											//Mongoose goes right
        if (g[gameItems.mongoose.y][gameItems.mongoose.x + 1] == TUNNEL)
        {
		gameItems.mongoose.x += 1;
		gameItems.mongoose.y += 0;
        }
        break;
    case 4:											//Mongoose goes left
        if (g[gameItems.mongoose.y][gameItems.mongoose.x - 1] == TUNNEL)
        {
		gameItems.mongoose.x -= 1;
		gameItems.mongoose.y += 0;
        }
        break;
    }
}

void displayClock(GameData player)									//Displays the clock after the grid has been displayed
{
    cout << "\n         TIMER     \n";
    cout << "  --------------------\n";
    cout << "  |  " << setfill('0') << setw(2) << player.minutes << " min | ";
    cout << setfill('0') << setw(2) << player.seconds << " sec |" << endl;
    cout << "  --------------------\n";
}

void timer(GameData &player)
{
    if(true)
    {
        Sleep(1);

		if (player.minutes == 0 && player.seconds == 0)			//Time runs out
			player.timeIsUp = true;

        else
        {
        --player.seconds;									//Counts down
        if (player.seconds < 0)
        {
            player.minutes--;
            player.seconds = 59;
        }
        }
    }
    return;
}

void save()
{
    ifstream fin;
    fin.open(player.name + ".save", ios::out);		//Opens players saved file if one exists
    if (fin.fail())
    {
        //stores new score from the player
        ofstream fout(player.name + ".save");      //Creates file for the player
        if (fout.fail())						   //Allows us to check whether the open operation has been successful
            cout << "\nError: file couldn't be created";
        else
        {										   //Saves value of variables to the saved game file
            fout << player.score << "\n" << player.mouseCollected << "\n" << player.name << "\n" << player.pillCollected << "\n" << player.amountOfMoves << "\n" << player.invincible << "\n" << player.InvincibleMoves  << "\n" << player.cheatEnabled  <<"\n" << player.cheatEnabled;   //Will need to be replaced with struct
            fout.close();
        }
    }

    else
    {
         ofstream fout(player.name + ".save");     //Overwrites the current data in the players saved file    
         if (fout.fail())
             cout << "\nError: file couldn't be opened";
         else
         {  
             fout << player.score << "\n" << player.mouseCollected << "\n" << player.name << "\n" << player.pillCollected << "\n" << player.amountOfMoves << "\n" << player.invincible << "\n" << player.InvincibleMoves << "\n" << player.cheatEnabled << "\n" << player.playerCheated;   //Will need to be replaced with struct
             fout.close();
         }

    }
}

void load(GameData &player)
{   //Loads game data from players saved game file
    char load;									   //Option that player chooses to load data or start new game

    cout <<"\n\n\n\n\n\n\n\n		       THERE IS SAVE DATA THAT YOU CAN LOAD";
    cout << "\n		       DO YOU WANT TO LOAD THAT DATA (Y/N): ";
    cin >> load;

    if (load == toupper('y'))
    {
        ifstream loadsave(player.name + ".save", ios::in); //Opens the file to be read from
        loadsave >> player.score >> player.mouseCollected >> player.name >> player.pillCollected >> player.amountOfMoves >> player.invincible >> player.InvincibleMoves >> player.cheatEnabled >> player.playerCheated;    //Overwrites all the variables in the game to "resume" the loaded game
        loadsave.close();
    }
    system("cls");								   //Clears the console screen and loads the game screen
}

void TwoPlayerGame(bool& IsTwoPlayerGame)
{
    char amount;								   //Amount of players (1 for one player, 2 for two player)
    selectTextColour(clRed);

    do
    {
        cout << "\n\n\n\n\n\n\n		       HOW MANY PLAYERS (1 OR 2): ";
        amount = getch();					       //Gets single inputted character
        system("cls");							   //clears the screen until an appropriate character is inputted (1 or 2)

    } while (amount != '1' && amount != '2');

    if (amount == '2')							   //Sets the game to 2 player mode
    {
        IsTwoPlayerGame = true;
    }
    system("cls");
}


//---------------------------------------------------------------------------
//----- Two Player Game Mode
//---------------------------------------------------------------------------

void twoPlayerMode()							   //Main of the two player game mode
{
    void createGame(char grid[][TwoPlayerMazeSIZEX], char twoPlayermaze[][TwoPlayerMazeSIZEX], vector<Item>& player1snake, vector<Item>& player2Snake, GameItems &gameItems); //Initialising the game
    void showGame(const char grid[][TwoPlayerMazeSIZEX], string& mess, TwoPlayerGameData &player1, TwoPlayerGameData &player2);    //Rendering the game
    void updatingGame(char grid[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake, vector<Item>& player2Snake, GameItems &gameItems, const int kc, string& mess, TwoPlayerGameData &player1, TwoPlayerGameData &player2);
    void playerOneName(TwoPlayerGameData &player1);//Gets the first players name
    void playerTwoName(TwoPlayerGameData &player2);//Gets the second player name
    bool wantsToQuit(const int key);			   //When either player wants to quit the game
    bool isArrowKey(const int k);				   //First player movement of the snake
    bool isWASDKey(const int k);			       //Second player movement of the snake
    int getKeyPress();
    void endProgram();							   //Ends the game when the player quits
    void player1Wins();							   //When player 2 has died
    void player2Wins();							   //When player 1 has died
    void noWinner();						       //When both player 1 and 2 have died


    char grid[TwoPlayerMazeSIZEY][TwoPlayerMazeSIZEX];  //grid for display
    char twoPlayermaze[TwoPlayerMazeSIZEY][TwoPlayerMazeSIZEX];  //structure of the maze
    vector<Item> player1Snake = { {0,0,HEAD},{0,0,TAIL},{ 0,0,TAIL },{ 0,0,TAIL } };
    vector<Item> player2Snake = { {0,0,p2HEAD},{0,0,TAIL},{ 0,0,TAIL },{ 0,0,TAIL } };

    string message("LET'S START...");
    seed();
    setNewConsoleTitle("FoP Group Project 2019 - Task 1c - Snake Game");
    playerOneName(player1);						   //Gets the first players name
    playerTwoName(player2);						   //Gets the second players name
    createGame(grid, twoPlayermaze, player1Snake, player2Snake, gameItems);
    int key;
    do
    {
        showGame(grid, message, player1, player2);
        key = toupper(getKeyPress());

        if (isArrowKey(key))					   //Player 1 movement
        {
            updatingGame(grid, twoPlayermaze, player1Snake, player2Snake, gameItems, key, message, player1, player2);
        }

        if (isWASDKey(key))					       //Player 2 movement
        {
            updatingGame(grid, twoPlayermaze, player1Snake, player2Snake, gameItems, key, message, player1, player2);
        }

        if (player1.playerDied)
        {
            message = "PLAYER 2 WINS";
        }
        if (player2.playerDied)
        {
            message = "PLAYER 1 WINS";
        }
        if (player1.playerDied && player2.playerDied)
        {
            message = "THERE IS NO WINNER";
        }

        if (wantsToQuit(key))
        {
            message = "PLAYER WANTS TO QUIT!";
        }
    } while (!wantsToQuit(key) && !player1.playerDied && !player2.playerDied);
    showGame(grid, message, player1,player2);

    if (player1.playerDied)
    {
        player2Wins();							   //Function that outputs the message of player 2 wins
    }

    if (player2.playerDied)
    {
        player1Wins();							   //Function that outputs the message of player 1 wins
    }
    if (player1.playerDied && player2.playerDied)
    {
        noWinner();								   //Function that outputs when both players have died
    }

    if (wantsToQuit(key))
    {
        endProgram();
    }
    return;
}

void playerOneName(TwoPlayerGameData &player1)	   //Gets the first players name
{
	int  MAXNAME = 20;
	selectTextColour(clRed);
	do
	{

		cout << "\n\n\n\n\n\n\n		       ENTER PLAYER NAME (max 20 characters): ";
		cin >> player1.playerName;


		system("CLS");

	} while (player1.playerName.length() > MAXNAME);

}

void createGame(char grid[][TwoPlayerMazeSIZEX], char twoPlayerMaze[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake, vector<Item>& player2Snake, GameItems &gameItems)
{
    void setTwoPlayerMazeStructure(char twoPlayerMaze[][TwoPlayerMazeSIZEX]);   //Setting structure of the maze
    void setPlayer1SnakeCoordinates(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake); //Sets position of player 1 snake
    void setPlayer2SnakeCoordinates(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], vector<Item>& player2Snake); //Sets position of player 2 snake
    void setMousePosition(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], GameItems &gameItems);
    void updateTheGrid(char grid[][TwoPlayerMazeSIZEX], const char twoPlayerMaze[][TwoPlayerMazeSIZEX], const vector<Item>& player1Snake, const vector<Item>& player2Snake, GameItems &gameItems);


    setTwoPlayerMazeStructure(twoPlayerMaze);
    setPlayer1SnakeCoordinates(twoPlayerMaze, player1Snake);
    updateTheGrid(grid, twoPlayerMaze, player1Snake, player2Snake, gameItems);
    setPlayer2SnakeCoordinates(twoPlayerMaze, player2Snake);
    setMousePosition(twoPlayerMaze, gameItems);
    updateTheGrid(grid, twoPlayerMaze, player1Snake, player2Snake, gameItems);
}

void setPlayer1SnakeCoordinates(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake)
{
    player1Snake.at(0).y = random(TwoPlayerMazeSIZEY - 2);
    player1Snake.at(0).x = random(TwoPlayerMazeSIZEX - 2);

    while (twoPlayerMaze[player1Snake.at(0).y][player1Snake.at(0).x] == WALL)
    {
        player1Snake.at(0).y = random(TwoPlayerMazeSIZEY - 2);
        player1Snake.at(0).x = random(TwoPlayerMazeSIZEX - 2);
    }

    for (int i(1); i < player1Snake.size(); ++i)
    {
        player1Snake.at(i).y = player1Snake.at(0).y;
        player1Snake.at(i).x = player1Snake.at(0).x;
    }
}

void setPlayer2SnakeCoordinates(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], vector<Item>& player2Snake)
{
    player2Snake.at(0).y = random(TwoPlayerMazeSIZEY - 2);
    player2Snake.at(0).x = random(TwoPlayerMazeSIZEX - 2);

    while (twoPlayerMaze[player2Snake.at(0).y][player2Snake.at(0).x] != TUNNEL)
    {
        player2Snake.at(0).y = random(TwoPlayerMazeSIZEY - 2);
        player2Snake.at(0).x = random(TwoPlayerMazeSIZEX - 2);
    }

    for (int i(1); i < player2Snake.size(); ++i)
    {
        player2Snake.at(i).y = player2Snake.at(0).y;
        player2Snake.at(i).x = player2Snake.at(0).x;
    }
}

void setMousePosition(const char twoPlayerMaze[][TwoPlayerMazeSIZEX], GameItems &gameItems)
{
    gameItems.mouse.x = random(TwoPlayerMazeSIZEX - 2);
	gameItems.mouse.y = random(TwoPlayerMazeSIZEY - 2);

    while (twoPlayerMaze[gameItems.mouse.y][gameItems.mouse.x] != TUNNEL)   //Getting new coordinates for mouse when the current coordinates are placed in walls
    {
		gameItems.mouse.y = random(TwoPlayerMazeSIZEY - 2);
		gameItems.mouse.x = random(TwoPlayerMazeSIZEX - 2);
    }
}

void setTwoPlayerMazeStructure(char twoPlayerMaze[][TwoPlayerMazeSIZEX])
{
        char TwoPlayerMaze[TwoPlayerMazeSIZEY][TwoPlayerMazeSIZEX] 	//local array to store the 2 player maze structure
        = { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
        { '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
        { '#', ' ', '#', '#', '#', '#', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', ' ', '#', '#', '#', '#', ' ', ' ', ' ', ' ', ' ', '#', '#', '#', '#', ' ', '#' },
        { '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
        { '#', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', '#' },
        { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
        { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' } };

        for (int row(0); row < TwoPlayerMazeSIZEY; ++row)
            for (int col(0); col < TwoPlayerMazeSIZEX; ++col)
                switch (TwoPlayerMaze[row][col])
                {
                case '#': twoPlayerMaze[row][col] = WALL; break;
                case ' ': twoPlayerMaze[row][col] = TUNNEL; break;
                }
}

void updatingGame(char grid[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake, vector<Item>& player2Snake, GameItems &gameItems, const int kc, string& mess, TwoPlayerGameData &player1, TwoPlayerGameData &player2)
{
    void updateData(const char g[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake, const int kc, string& m, GameItems &gameItems, TwoPlayerGameData &player1, vector<Item>& player2Snake, TwoPlayerGameData &player2);
    void updateTheGrid(char g[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX], const vector<Item>& player1Snake, const vector<Item>& player2Snake, GameItems &gameItems);
    updateData(grid, player1Snake, kc, mess, gameItems, player1, player2Snake, player2);
    updateTheGrid(grid, twoPlayermaze, player1Snake, player2Snake, gameItems);
}

void updateData(const char g[][TwoPlayerMazeSIZEX], vector<Item>& player1Snake, int key, string& m, GameItems &gameItems, TwoPlayerGameData &player1, vector<Item>& player2Snake, TwoPlayerGameData &player2)
{
    //void die();
    bool isArrowKey(const int k);
    bool isWASDKey(const int k);
    void setKeyDirection(int k, int& dx, int& dy);
    void setPlayer1Direction(int k, int& dx, int& dy);  //Moving player 1 snake
    void setPlayer2Direction(int k, int& dx, int& dy);  //Moving player 2 snake
	void moveP1Snake(vector<Item>& player1Snake, int dx, int dy);
	void moveP2Snake(vector<Item>& player2Snake, int dx, int dy);
	void move2PlayerMouse(const char g[][TwoPlayerMazeSIZEX], GameItems& gameItems);

														
	//void updateScore();


    m = "";

    int dx(0), dy(0);

    if (isArrowKey(key))								//Allows the first player to move around the grid
    {

    setPlayer1Direction(key, dx, dy);

    switch (g[player1Snake.at(0).y + dy][player1Snake.at(0).x + dx])        //Player 1 movement around grid
    {
    case TUNNEL:
        //Player 1 movement
		moveP1Snake(player1Snake, dx, dy);

        break;

    case WALL:
        m = "GAME OVER";
        player1.playerDied = true;
        break;

    case MOUSE:
        m = "MOUSE CAUGHT";

        for (int i(player1Snake.size() - 1); i >= 1; --i)
        {
            player1Snake.at(i).y = player1Snake.at(i - 1).y;
            player1Snake.at(i).x = player1Snake.at(i - 1).x;
        }

        player1Snake.push_back({ player1Snake.at(player1Snake.size() - 1).x,player1Snake.at(player1Snake.size() - 1).y,TAIL });
        player1Snake.push_back({ player1Snake.at(player1Snake.size() - 1).x,player1Snake.at(player1Snake.size() - 1).y,TAIL });

        player1Snake.at(0).x += dx;						//go in that X direction
        player1Snake.at(0).y += dy;						//go in that Y direction

        player1.playerScore += 10;						//Adds to the first players score when they collect a mouse
		
		move2PlayerMouse(g, gameItems);

        break;

    case TAIL:
        m = "GAME OVER. YOU LOSE";
        player1.playerDied = true;
        break;

    case HEAD:
        m = "GAME OVER. YOU LOSE";
        player1.playerDied = true;
        player2.playerDied = true;
        break;
    }
    }

    else if(isWASDKey(key))								//Allows the second player to move around the grid
    {

    setPlayer2Direction(key, dx, dy);
    switch (g[player2Snake.at(0).y + dy][player2Snake.at(0).x + dx])        //Player 2 movement around grid
    {
    case TUNNEL:
        //Player 2 movement
		moveP2Snake(player2Snake, dx, dy);

        break;

    case WALL:
        m = "GAME OVER";
        player2.playerDied = true;
        break;

    case MOUSE:
        m = "MOUSE CAUGHT";

        for (int i(player2Snake.size() - 1); i >= 1; --i)
        {
            player2Snake.at(i).y = player2Snake.at(i - 1).y;
            player2Snake.at(i).x = player2Snake.at(i - 1).x;
        }

        player2Snake.push_back({ player2Snake.at(player2Snake.size() - 1).x,player2Snake.at(player2Snake.size() - 1).y,TAIL });
        player2Snake.push_back({ player2Snake.at(player2Snake.size() - 1).x,player2Snake.at(player2Snake.size() - 1).y,TAIL });

        player2Snake.at(0).x += dx;						//go in that X direction
        player2Snake.at(0).y += dy;						//go in that Y direction

        player2.playerScore += 10;						//Adds to the first players score when they collect a mouse
		move2PlayerMouse(g, gameItems);

        break;

    case TAIL:
        m = "GAME OVER. YOU LOSE";
        player2.playerDied = true;
        break;

    case HEAD:
        m = "PLAYER 2 DIED...";
        player2.playerDied = true;
        player1.playerDied = true;
        break;
    }
    }

}

//---------------------------------------------------------------------
//-------------Snake and Mouse functions
//---------------------------------------------------------------------

void moveP1Snake(vector<Item>& player1Snake, int dx, int dy)
{
	for (int i(player1Snake.size() - 1); i >= 1; --i)
	{
		player1Snake.at(i).y = player1Snake.at(i - 1).y;
		player1Snake.at(i).x = player1Snake.at(i - 1).x;
	}

	player1Snake.at(0).y += dy;						//go in that Y direction
	player1Snake.at(0).x += dx;						//go in that X direction
}

void moveP2Snake(vector<Item>& player2Snake, int dx, int dy)
{
	for (int i(player2Snake.size() - 1); i >= 1; --i)
	{
		player2Snake.at(i).y = player2Snake.at(i - 1).y;
		player2Snake.at(i).x = player2Snake.at(i - 1).x;
	}

	player2Snake.at(0).y += dy;						//go in that Y direction
	player2Snake.at(0).x += dx;						//go in that X direction
}

void move2PlayerMouse(const char g[][TwoPlayerMazeSIZEX], GameItems& gameItems)
{
	gameItems.mouse.y = random(TwoPlayerMazeSIZEY - 2);
	gameItems.mouse.x = random(TwoPlayerMazeSIZEX - 2);

	while (g[gameItems.mouse.y][gameItems.mouse.x] != TUNNEL)
	{
		gameItems.mouse.y = random(TwoPlayerMazeSIZEY - 2);
		gameItems.mouse.x = random(TwoPlayerMazeSIZEX - 2);
	}
}

//---------------------------------------------------------------------------
//----- Rest of the code
//---------------------------------------------------------------------------

void setPlayer1Direction(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
    switch (key)										//...depending on the selected key...
    {
    case LEFT:  										//when LEFT arrow pressed...
        dx = -1;										//decrease the X coordinate
        dy = 0;
        break;
    case RIGHT: 										//when RIGHT arrow pressed...
        dx = +1;										//increase the X coordinate
        dy = 0;
        break;
    case UP:
        dx = 0;
        dy = -1;
        break;
    case DOWN:
        dx = 0;
        dy = +1;
        break;
    }
}

void setPlayer2Direction(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
    switch (key)										//...depending on the selected key...
    {
    case 'A':  											//when LEFT arrow pressed...
        dx = -1;										//decrease the X coordinate
        dy = 0;
        break;
    case 'D': 											//when RIGHT arrow pressed...
        dx = +1;										//increase the X coordinate
        dy = 0;
        break;
    case 'W':
        dx = 0;
        dy = -1;
        break;
    case 'S':
        dx = 0;
        dy = +1;
        break;
    }
}


void updateTheGrid(char g[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX], const vector<Item>& player1Snake, const vector<Item>& player2Snake, GameItems &gameItems)
{
    void placetwoPlayerMaze(char g[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX]);  //Places the maze in the console
    void placePlayer1Snake(char g[][TwoPlayerMazeSIZEX], const Item& player1Snake); //Places the first players snake on the grid
    void placePlayer2Snake(char g[][TwoPlayerMazeSIZEX], const Item& player2Snake); //Places the second players snake on the grid
    void placeMouse(char g[][TwoPlayerMazeSIZEX], GameItems &gameItems);

    placetwoPlayerMaze(g, twoPlayermaze);

    for (int i(player1Snake.size() - 1); i >= 0; --i)	//cant be size_t because its an unsigned integer! BOOOMMMMM
    {
        placePlayer1Snake(g, player1Snake.at(i));		//set player 1 snake in grid
    }

    for (int i(player2Snake.size() - 1); i >= 0; --i)	//cant be size_t because its an unsigned integer! BOOOMMMMM
    {
        placePlayer2Snake(g, player2Snake.at(i));		//set player 2 snake in grid
    }
    placeMouse(g, gameItems);
}

void placetwoPlayerMaze(char g[][TwoPlayerMazeSIZEX], const char twoPlayermaze[][TwoPlayerMazeSIZEX])
{
    for (int row(0); row < TwoPlayerMazeSIZEY; ++row)
        for (int col(0); col < TwoPlayerMazeSIZEX; ++col)
            g[row][col] = twoPlayermaze[row][col];
}

void placePlayer1Snake(char g[][TwoPlayerMazeSIZEX], const Item& player1Snake)
{
    g[player1Snake.y][player1Snake.x] = player1Snake.symbol;
}

void placePlayer2Snake(char g[][TwoPlayerMazeSIZEX], const Item& player2Snake)
{
    g[player2Snake.y][player2Snake.x] = player2Snake.symbol;
}

void placeMouse(char g[][TwoPlayerMazeSIZEX], GameItems &gameItems)
{
    g[gameItems.mouse.y][gameItems.mouse.x] = gameItems.mouse.symbol;
}

bool isWASDKey(const int k)								//second player controls
{
    return (k == 'A') || (k == 'D') || (k == 'W') || (k == 'S');
}

void showGame(const char grid[][TwoPlayerMazeSIZEX], string& mess, TwoPlayerGameData &player1, TwoPlayerGameData &player2)
{
    string tostring(char x);
    string tostring(int x);

    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& mess);
    void ShowGrid(const char grid[][TwoPlayerMazeSIZEX]);

    showMessage(clBlack, clYellow, 0, 0, "___SNAKE GAME___");
    showMessage(clYellow, clBlack, 40, 0, "FoP Task 1c - February 2019   ");
    showMessage(clYellow, clBlack, 40, 1, getDate() + " " + getTime());
    showMessage(clYellow, clBlack, 40, 3, "Group: CS4G2 D      ");
    showMessage(clYellow, clBlack, 40, 4, "Adam   Student no: 27025936");
    showMessage(clYellow, clBlack, 40, 5, "Corey  Student no: 28006767");
    showMessage(clYellow, clBlack, 40, 6, "Shoaib Student no: 28011183");

    showMessage(clYellow, clBlack, 40, 7, "TO QUIT   - PRESS 'Q'           ");
    //Player 1 information
    showMessage(clRed, clYellow, 40, 9, "PLAYER ONE CONTROLS  : ARROW KEYS");
    showMessage(clRed, clYellow, 40, 10, "PLAYER ONE'S NAME    : " + player1.playerName); //Showing first players name
    showMessage(clRed, clYellow, 40, 11, "PLAYER ONE'S SCORE   : " + tostring(player1.playerScore));  //Showing first players score
    //Player 2 information
    showMessage(clRed, clYellow, 40, 13, "PLAYER TWO CONTROLS  : WASD");
    showMessage(clRed, clYellow, 40, 14, "PLAYER TWO'S NAME    : " + player2.playerName); //Showing second players name
    showMessage(clRed, clYellow, 40, 15, "PLAYER TWO'S SCORE   : " + tostring(player2.playerScore));  //Showing second players score

    showMessage(clBlack, clWhite, 40, 18, mess);		//display current message
    ShowGrid(grid);
}

void ShowGrid(const char grid[][TwoPlayerMazeSIZEX])
{
    selectBackColour(clBlack);
    selectTextColour(clWhite);
    gotoxy(0, 2);

    for (int row(0); row < TwoPlayerMazeSIZEY; ++row)
    {
        for (int col(0); col < TwoPlayerMazeSIZEX; ++col)
        {
            if (grid[row][col] == '0')
            {
                selectTextColour(clRed);
            }

			if (grid[row][col] == p2HEAD)
			{
				selectTextColour(clBlue);
			}

            if (grid[row][col] == 'o')
            {
                selectTextColour(clGreen);
            }

            if (grid[row][col] == '@')
            {
                selectTextColour(clYellow);
            }

            cout << grid[row][col];						//output cell content
            selectTextColour(clWhite);
        }

        cout << endl;
    }
}

void playerTwoName(TwoPlayerGameData &player2)
{
    int MAXNAME = 20;

    selectTextColour(clRed);
    do
    {

        cout << "\n\n\n\n\n\n\n		       ENTER SECOND PLAYER'S NAME (max 20 characters): ";
        cin >> player2.playerName;


        system("cls");

    } while (player2.playerName.length() > MAXNAME);
}

void player1Wins()
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 20, "PLAYER 1 WINS...\n");
    system("pause");									//hold output screen until a keyboard key is hit
    return;
}

void player2Wins()
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 20, "PLAYER 2 WINS...\n");
    system("pause");									//hold output screen until a keyboard key is hit
    return;
}

void noWinner()											 
{
    void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
    showMessage(clRed, clYellow, 40, 20, "PLAYER 1 AND PLAYER 2 DIED...\n");
    system("pause");									//hold output screen until a keyboard key is hit
    return;
}

