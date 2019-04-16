#include "GameBoard.h"
#include <iostream>

int GameBoard::WrappingModulo(int pNum, int pMod)
{
	int answer = pNum % pMod;
	if (answer < 0)
		answer += pMod;
	return answer;
}

int GameBoard::SurroundingCells(int pCol, int pRow)
{
	int enabledSurroundingCells = 0;

	//Top of cell
	enabledSurroundingCells += mBoard[WrappingModulo(pCol - 1, mBoardColumns)][WrappingModulo(pRow - 1,  mBoardRows)];
	enabledSurroundingCells += mBoard[WrappingModulo(pCol    , mBoardColumns)][WrappingModulo(pRow - 1, mBoardRows)];
	enabledSurroundingCells += mBoard[WrappingModulo(pCol + 1, mBoardColumns)][WrappingModulo(pRow - 1, mBoardRows)];

	//Bottom of cell
	enabledSurroundingCells += mBoard[WrappingModulo(pCol - 1, mBoardColumns)][WrappingModulo(pRow + 1, mBoardRows)];
	enabledSurroundingCells += mBoard[WrappingModulo(pCol, mBoardColumns)][WrappingModulo(pRow + 1, mBoardRows)];
	enabledSurroundingCells += mBoard[WrappingModulo(pCol + 1, mBoardColumns)][WrappingModulo(pRow + 1, mBoardRows)];

	//Left and right of cell
	enabledSurroundingCells += mBoard[WrappingModulo(pCol - 1, mBoardColumns)][WrappingModulo(pRow, mBoardRows)];
	enabledSurroundingCells += mBoard[WrappingModulo(pCol + 1, mBoardColumns)][WrappingModulo(pRow, mBoardRows)];

	return enabledSurroundingCells;
}

//Allocate memory to the board
GameBoard::GameBoard(int pScreenWidth, int pScreenHeight, int pCellSize)
{
	//Set the member variables to the parameters
	mScreenWidth = pScreenWidth;
	mScreenHeight = pScreenHeight;
	mCellSize = pCellSize;

	//Calculate the number of cells wide and tall the board is
	mBoardColumns = mScreenWidth / mCellSize;
	mBoardRows = mScreenHeight / mCellSize;
	
	//Allocate memory to the board
	mBoard = new bool*[mBoardColumns];
	for (int col = 0; col < mBoardColumns; col++)
	{
		mBoard[col] = new bool[mBoardRows];

		//Set all the bools in this row to false by default
		for (int row = 0; row < mBoardRows; row++) 
		{
			mBoard[col][row] = false;
		}
	}

	//Allocate memory to the temporary board
	mBoard2 = new bool*[mBoardColumns];
	for (int col = 0; col < mBoardColumns; col++)
	{
		mBoard2[col] = new bool[mBoardRows];

		//Set all the bools in this row to false by default
		for (int row = 0; row < mBoardRows; row++)
		{
			mBoard2[col][row] = false;
		}
	}
}

void GameBoard::CalculateNextFrame()
{	
	//Iterate through all the cells on the board
	for (int col = 0; col < mBoardColumns; col++)
	{
		for (int row = 0; row < mBoardRows; row++)
		{
			int surroundingCells = SurroundingCells(col, row);

			if (mBoard[col][row]) 
			{
				if (surroundingCells < 2 || surroundingCells > 3)
					mBoard2[col][row] = false;
				else
					mBoard2[col][row] = true;
			}

			if (!mBoard[col][row])
			{
				if (surroundingCells == 3)
					mBoard2[col][row] = true;
				else
					mBoard2[col][row] = false;
			}
		}
	}

	//Switch the temporary board and the main board
	bool** tmp = mBoard;
	mBoard = mBoard2;
	mBoard2 = tmp;
}

void GameBoard::SetBoardPosition(int pCol, int pRow, bool pToggle)
{
	if(pCol < mBoardColumns && pRow < mBoardRows && pCol >= 0 && pRow >= 0) //If this index is within the right range
		mBoard[pCol][pRow] = pToggle;
}

void GameBoard::DrawGrid() 
{
	//Initialize openGl for drawing lines
	glOrtho(0, mScreenWidth, 0, mScreenHeight, 0, 1);
	glLoadIdentity();
	glLineWidth(1); //Set the width of the line to 1
	glColor3f(0.8f, 0.8f, 0.8f); //Set the color of the line to grey
	glBegin(GL_LINES);

	//Draw the vertical lines
	for (int col = 0; col <= mBoardColumns; col++) 
	{
		float xPos = ((float)col / (float)mBoardColumns) * 2 - 1;
		glVertex2f(xPos, -1);
		glVertex2f(xPos, 1);
	}

	//Draw the horizontal lines
	for (int row = 0; row <= mBoardRows; row++)
	{
		float yPos = ((float)row / (float)mBoardRows) * 2 - 1;
		glVertex2f(-1, yPos);
		glVertex2f(1, yPos);
	}
	glEnd();
}

void GameBoard::DrawBoard()
{
	//Initialize openGl for drawing quads
	glViewport(0, 0, mScreenWidth, mScreenHeight);
	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_QUADS);

	double xSize = (float)1 / (float)mBoardColumns * 2; //The width of 1 cell
	double ySize = (float)1 / (float)mBoardRows * 2; //The height of 1 cell

	//Iterate through all the cells on the board
	for (int col = 0; col < mBoardColumns; col++) 
	{
		for (int row = 0; row < mBoardRows; row++) 
		{
			if (mBoard[col][row]) //If this cell is enabled
			{
				double xPos = ((float)col / (float)mBoardColumns) * 2 - 1;
				double yPos = ((float)row / (float)mBoardRows) * 2 - 1;
				glVertex2f(xPos, yPos);
				glVertex2f(xPos + xSize, yPos);
				glVertex2f(xPos + xSize, yPos + ySize);
				glVertex2f(xPos    , yPos + ySize);
			}
		}
	}
	glEnd();
}

void GameBoard::DrawMouseHover(double pMouseX, double pMouseY)
{
	//Find what index on the board the mouse position corresponds to
	int xIndex, yIndex;
	xIndex = (int)(pMouseX / (double)mScreenWidth * mBoardColumns);
	yIndex = mBoardRows - (int)(pMouseY / (double)mScreenHeight * mBoardRows) - 1; //Y is inverted

	double xPos = ((float)xIndex / (float)mBoardColumns) * 2 - 1; //The left of the box we want to draw
	double yPos = ((float)yIndex / (float)mBoardRows) * 2 - 1; //The bottom of the box we want to draw

	double xSize = (float)1 / (float)mBoardColumns * 2; //The width of 1 cell
	double ySize = (float)1 / (float)mBoardRows * 2; //The height of 1 cell

	 //Initialize openGl for drawing lines
	glOrtho(0, mScreenWidth, 0, mScreenHeight, 0, 1);
	glLoadIdentity();
	glLineWidth(2); //Set the width of the line to 1
	glColor3f(0.2f, 0.2f, 0.2f); //Set the color of the line to grey
	glBegin(GL_LINES);

	//Top of box
	glVertex2f(xPos, yPos);
	glVertex2f(xPos + xSize, yPos);

	//Bottom of box
	glVertex2f(xPos + xSize, yPos + ySize);
	glVertex2f(xPos, yPos + ySize);

	//Left of box
	glVertex2f(xPos, yPos + ySize);
	glVertex2f(xPos, yPos);

	//Right of box
	glVertex2f(xPos + xSize, yPos + ySize);
	glVertex2f(xPos + xSize, yPos);

	glEnd();
}

void GameBoard::MouseClick(double pMouseX, double pMouseY, bool pEnable)
{
	//Find what index on the board the mouse position corresponds to
	int xIndex, yIndex;
	xIndex = (int)(pMouseX / (double)mScreenWidth * mBoardColumns);
	yIndex = mBoardRows - (int)(pMouseY / (double)mScreenHeight * mBoardRows) - 1; //Y is inverted

	//Set that cell to be enabled/disabled
	SetBoardPosition(xIndex, yIndex, pEnable);
}

void GameBoard::ResetBoard()
{
	//Iterate through all the cells on the board
	for (int col = 0; col < mBoardColumns; col++)
	{
		for (int row = 0; row < mBoardRows; row++)
		{
			mBoard[col][row] = false;
			mBoard2[col][row] = false;
		}
	}
}

void GameBoard::LoadPreconfiguration(int pConfig)
{
	//Clear the board before we load the new config
	ResetBoard();

	if(pConfig == 1) //Glider
	{
		SetBoardPosition(40, 25, true);
		SetBoardPosition(41, 25, true);
		SetBoardPosition(42, 25, true);
		SetBoardPosition(42, 26, true);
		SetBoardPosition(41, 27, true);
	}
	else if (pConfig == 2) //Horizontal Glider
	{
		SetBoardPosition(40, 25, true);
		SetBoardPosition(40, 26, true);
		SetBoardPosition(40, 27, true);
		SetBoardPosition(41, 28, true);
		SetBoardPosition(41, 25, true);
		SetBoardPosition(42, 25, true);
		SetBoardPosition(43, 25, true);
		SetBoardPosition(44, 26, true);
		SetBoardPosition(44, 28, true);
	}
	else if (pConfig == 3) //Big Horizonal Glider
	{
		SetBoardPosition(40, 25, true);
		SetBoardPosition(40, 26, true);
		SetBoardPosition(40, 27, true);
		SetBoardPosition(41, 28, true);
		SetBoardPosition(41, 25, true);
		SetBoardPosition(42, 25, true);
		SetBoardPosition(43, 25, true);
		SetBoardPosition(44, 26, true);
		SetBoardPosition(44, 28, true);

		SetBoardPosition(40, 21, true);
		SetBoardPosition(40, 20, true);
		SetBoardPosition(40, 19, true);
		SetBoardPosition(41, 18, true);
		SetBoardPosition(41, 21, true);
		SetBoardPosition(42, 21, true);
		SetBoardPosition(43, 21, true);
		SetBoardPosition(44, 20, true);
		SetBoardPosition(44, 18, true);

		SetBoardPosition(46, 24, true);
		SetBoardPosition(46, 23, true);
		SetBoardPosition(46, 22, true);
		SetBoardPosition(47, 24, true);
		SetBoardPosition(47, 23, true);
		SetBoardPosition(47, 22, true);
		SetBoardPosition(48, 24, true);
		SetBoardPosition(48, 22, true);
		SetBoardPosition(49, 23, true);
		SetBoardPosition(50, 23, true);
	}
	else if (pConfig == 4) //Glider gun
	{
		SetBoardPosition(20, 25, true);
		SetBoardPosition(20, 26, true);
		SetBoardPosition(21, 25, true);
		SetBoardPosition(21, 26, true);

		SetBoardPosition(30, 24, true);
		SetBoardPosition(30, 25, true);
		SetBoardPosition(30, 26, true);
		SetBoardPosition(31, 23, true);
		SetBoardPosition(31, 27, true);
		SetBoardPosition(32, 28, true);
		SetBoardPosition(33, 28, true);
		SetBoardPosition(32, 22, true);
		SetBoardPosition(33, 22, true);

		SetBoardPosition(34, 25, true);
		SetBoardPosition(35, 23, true);
		SetBoardPosition(35, 27, true);
		SetBoardPosition(36, 26, true);
		SetBoardPosition(36, 25, true);
		SetBoardPosition(36, 24, true);
		SetBoardPosition(37, 25, true);
	
		SetBoardPosition(40, 26, true);
		SetBoardPosition(40, 27, true);
		SetBoardPosition(40, 28, true);
		SetBoardPosition(41, 26, true);
		SetBoardPosition(41, 27, true);
		SetBoardPosition(41, 28, true);
		SetBoardPosition(42, 29, true);
		SetBoardPosition(42, 25, true);

		SetBoardPosition(44, 25, true);
		SetBoardPosition(44, 24, true);
		SetBoardPosition(44, 29, true);
		SetBoardPosition(44, 30, true);

		SetBoardPosition(54, 28, true);
		SetBoardPosition(54, 27, true);
		SetBoardPosition(55, 28, true);
		SetBoardPosition(55, 27, true);
	}
	else if (pConfig == 5) //Oscillators
	{
		//Oscillator 1
		{
			SetBoardPosition(15, 18, true);
			SetBoardPosition(15, 19, true);
			SetBoardPosition(15, 20, true);
			SetBoardPosition(15, 21, true);
			SetBoardPosition(15, 22, true);
			SetBoardPosition(15, 23, true);
			SetBoardPosition(16, 18, true);
			SetBoardPosition(16, 19, true);
			SetBoardPosition(16, 20, true);
			SetBoardPosition(16, 21, true);
			SetBoardPosition(16, 22, true);
			SetBoardPosition(16, 23, true);

			SetBoardPosition(15, 25, true);
			SetBoardPosition(16, 25, true);
			SetBoardPosition(17, 25, true);
			SetBoardPosition(18, 25, true);
			SetBoardPosition(19, 25, true);
			SetBoardPosition(20, 25, true);
			SetBoardPosition(15, 26, true);
			SetBoardPosition(16, 26, true);
			SetBoardPosition(17, 26, true);
			SetBoardPosition(18, 26, true);
			SetBoardPosition(19, 26, true);
			SetBoardPosition(20, 26, true);

			SetBoardPosition(18, 18, true);
			SetBoardPosition(19, 18, true);
			SetBoardPosition(20, 18, true);
			SetBoardPosition(21, 18, true);
			SetBoardPosition(22, 18, true);
			SetBoardPosition(23, 18, true);
			SetBoardPosition(18, 19, true);
			SetBoardPosition(19, 19, true);
			SetBoardPosition(20, 19, true);
			SetBoardPosition(21, 19, true);
			SetBoardPosition(22, 19, true);
			SetBoardPosition(23, 19, true);

			SetBoardPosition(22, 26, true);
			SetBoardPosition(22, 25, true);
			SetBoardPosition(22, 24, true);
			SetBoardPosition(22, 23, true);
			SetBoardPosition(22, 22, true);
			SetBoardPosition(22, 21, true);
			SetBoardPosition(23, 26, true);
			SetBoardPosition(23, 25, true);
			SetBoardPosition(23, 24, true);
			SetBoardPosition(23, 23, true);
			SetBoardPosition(23, 22, true);
			SetBoardPosition(23, 21, true);
		}

		//Oscillator 2
		{
			SetBoardPosition(38, 23, true);
			SetBoardPosition(38, 22, true);
			SetBoardPosition(38, 21, true);
			SetBoardPosition(38, 20, true);
			SetBoardPosition(38, 19, true);

			SetBoardPosition(40, 23, true);
			SetBoardPosition(40, 22, true);
			SetBoardPosition(40, 21, true);
			SetBoardPosition(40, 20, true);
			SetBoardPosition(40, 19, true);

			SetBoardPosition(39, 19, true);
			SetBoardPosition(39, 23, true);
		}

		//Oscillator 3
		{
			SetBoardPosition(55, 23, true);
			SetBoardPosition(55, 24, true);
			SetBoardPosition(55, 25, true);
			SetBoardPosition(56, 23, true);
			SetBoardPosition(56, 24, true);
			SetBoardPosition(56, 25, true);
			SetBoardPosition(57, 23, true);
			SetBoardPosition(57, 24, true);
			SetBoardPosition(57, 25, true);

			SetBoardPosition(58, 20, true);
			SetBoardPosition(58, 21, true);
			SetBoardPosition(58, 22, true);
			SetBoardPosition(59, 20, true);
			SetBoardPosition(59, 21, true);
			SetBoardPosition(59, 22, true);
			SetBoardPosition(60, 20, true);
			SetBoardPosition(60, 21, true);
			SetBoardPosition(60, 22, true);
		}
	}
}
