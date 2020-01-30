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

//defining symbols used for display of the grid and content
const char HEAD('0');   							//HEAD
const char MOUSE('@');								//MOUSE
const char PILL('+');								//PILL
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



struct GameData	//Players data
{
	int mouseCollected = 0;
	string name;
	int score;
	bool cheatEnabled = false;
	bool playerCheated = false;						//Checks if the player cheated at any point during the game
	bool playerDied = false;
	int highScore;
	bool pillCollected = false;						//Used to disable pill in the grid when the amount of mice collected is not 2,4 or 6
	int amountOfMoves = 10;
	
}player;

struct Item {
    int x, y;
    char symbol;
};

struct GameItems
{
	Item mouse = { 0, 0, MOUSE };					//Mouse's position and symbol
	Item pill = { 0, 0, PILL };						//Power Up Pill's position and symbol
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
  	  
    //local variable declarations 
    char grid[SIZEY][SIZEX];						//grid for display
    char maze[SIZEY][SIZEX];						//structure of the maze
   
	vector<Item> snake = { {0,0,HEAD},{0,0,TAIL},{ 0,0,TAIL },{ 0,0,TAIL } };  //Tail's position and symbol
    playerName(player);
	  
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
        key = toupper(getKeyPress()); 				//read in  selected key: arrow or letter command
      
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
		     

    } 
	while (!wantsToQuit(key) && player.mouseCollected != 7 && !player.playerDied);		//while user does not want to quit
    renderGame(grid, message,player);				//display game info, modified grid and messages
    if(!player.playerCheated)						 //Doesn't save the score if the player has cheated during the game 
		storeScore(player);
    if(wantsToQuit(key))
        endProgram();								//display final message when player quits the game
    if (player.mouseCollected == 7)
        youWin();									//display final message when player collects all mice
    if (player.playerDied)
        die();										//display final message when player die's from hitting wall etc.
	
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
	void moveMouse(const char g[][SIZEX], GameItems& gameItems);
	void movePill(const char g[][SIZEX], GameItems& gameItems);

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
      
		break;
    case WALL:  									//hit a wall and stay there
           
		mess = "GAME OVER. YOU LOSE!";
        player.playerDied = true;
        
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

        break;
    case TAIL:
       
        mess = "GAME OVER. YOU LOSE!";
        player.playerDied = true;
       
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
       break;
   
    }
}

//---------------------------------------------------------------------
//-------------Snake and Mouse functions
//---------------------------------------------------------------------

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

void moveMouse(const char g[][SIZEX], GameItems& gameItems)
{
	gameItems.mouse.y = random(SIZEY - 2);
	gameItems.mouse.x = random(SIZEX - 2);
	while (g[gameItems.mouse.y][gameItems.mouse.x] != TUNNEL)
	{
		gameItems.mouse.y = random(SIZEY - 2);
		gameItems.mouse.x = random(SIZEX - 2);
	}
}

void movePill(const char g[][SIZEX], GameItems& gameItems)
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
//----- Rest of the code
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Item>& snake, GameItems &gameItems, GameData &player)
{ //update grid configuration after each move
    void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
    void placeItem(char g[][SIZEX], const Item& item);
    void placeItem(char g[][SIZEX], GameItems &gameItems, GameData &player);
   
    placeMaze(grid, maze);
    for (int i(snake.size() - 1); i >= 0; --i)		//cant be size_t because its an unsigned integer!
    {
        placeItem(grid, snake.at(i));				//set snake in grid
    }
   
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

	showMessage(clRed, clYellow, 40, 11, "PLAYER'S NAME    : " + player.name);
	showMessage(clRed, clYellow, 40, 12, "BEST SCORE SO FAR: " + tostring(player.highScore));
	showMessage(clRed, clYellow, 40, 13, "CURRENT SCORE    : " + tostring(player.score));
    showMessage(clRed, clYellow, 40, 14, "MICE COLLECTED   : " + tostring(player.mouseCollected));
	
    //print auxiliary messages if any
    showMessage(clBlack, clWhite, 40, 18, mess);	//display current message

   
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
               
            }

            if (g[row][col] == 'o')					//Changes colour of snake body
            {
                selectTextColour(clGreen);
               
            }
            if (g[row][col] == '@' || g[row][col] == '+')   //Changes colour of pill and mouse
                selectTextColour(clYellow);

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
