#include <GLFW/glfw3.h>

///Create a class that handles everything related to the game of life
class GameBoard
{
private:
	bool** mBoard; //A 2d array of bools representing whether a given cell is currently alive or dead
	bool** mBoard2; //Used temporarily in the calculation for the next frame. Similar to double buffering
	int mScreenWidth; //The size of the screens' width
	int mScreenHeight; //The size of the screens' height
	int mBoardColumns; //The number of column in the board
	int mBoardRows; //The number of rows in the board
	int mCellSize; //The width/height in pixels of a cell
	
	int WrappingModulo(int, int); //Modulo operator that enforces the answer to be positive
	int SurroundingCells(int, int); //Returns the number of enabled cells surrounding a given cell

public:
	GameBoard(int, int, int); ///The constructor which takes the screenwidth, screenheight, and cellSize
	void CalculateNextFrame(); ///Calculates the next frame given the current frame
	void SetBoardPosition(int, int, bool); ///Sets the board value at a given index
	void DrawGrid(); ///Draws a grid onto the window depending on the boards width and height
	void DrawBoard(); ///Draws the cells of the board onto the main screen
	void DrawMouseHover(double, double); ///Draws the square over the cell that the player is hovering over
	void MouseClick(double, double, bool); ///Enables/disables the cell the player clicked on
	void ResetBoard(); ///Resets all the cells on the board to false
	void LoadPreconfiguration(int); ///Loads a premade sequence
};