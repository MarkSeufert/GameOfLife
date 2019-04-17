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

void GameBoard::ChangedWindowSize(int pNewWidth, int pNewHeight)
{
	mScreenWidth = pNewWidth;
	mScreenHeight = pNewHeight;

	//Calculate the number of cells wide and tall the board is
	mBoardColumns = mScreenWidth / mCellSize;
	mBoardRows = mScreenHeight / mCellSize;
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

	//Calculate the center cell index
	int centerX, centerY;
	centerX = mBoardColumns / 2;
	centerY = mBoardRows / 2;

	if(pConfig == 1) //Glider
	{
		SetBoardPosition(centerX - 1, centerY - 1, true);
		SetBoardPosition(centerX    , centerY - 1, true);
		SetBoardPosition(centerX + 1, centerY - 1, true);
		SetBoardPosition(centerX + 1, centerY    , true);
		SetBoardPosition(centerX    , centerY + 1, true);
	}
	else if (pConfig == 2) //Horizontal Glider
	{
		SetBoardPosition(centerX - 2, centerY - 1, true);
		SetBoardPosition(centerX - 2, centerY    , true);
		SetBoardPosition(centerX - 2, centerY + 1, true);
		SetBoardPosition(centerX - 1, centerY + 2, true);
		SetBoardPosition(centerX - 1, centerY - 1, true);
		SetBoardPosition(centerX    , centerY - 1, true);
		SetBoardPosition(centerX + 1, centerY - 1, true);
		SetBoardPosition(centerX + 2, centerY    , true);
		SetBoardPosition(centerX + 2, centerY + 2, true);
	}
	else if (pConfig == 3) //Big Horizonal Glider
	{
		SetBoardPosition(centerX - 5, centerY + 2, true);
		SetBoardPosition(centerX - 5, centerY + 3, true);
		SetBoardPosition(centerX - 5, centerY + 4, true);
		SetBoardPosition(centerX - 4, centerY + 5, true);
		SetBoardPosition(centerX - 4, centerY + 2, true);
		SetBoardPosition(centerX - 3, centerY + 2, true);
		SetBoardPosition(centerX - 2, centerY + 2, true);
		SetBoardPosition(centerX - 1, centerY + 3, true);
		SetBoardPosition(centerX - 1, centerY + 5, true);

		SetBoardPosition(centerX - 5, centerY - 2, true);
		SetBoardPosition(centerX - 5, centerY - 3, true);
		SetBoardPosition(centerX - 5, centerY - 4, true);
		SetBoardPosition(centerX - 4, centerY - 5, true);
		SetBoardPosition(centerX - 4, centerY - 2, true);
		SetBoardPosition(centerX - 3, centerY - 2, true);
		SetBoardPosition(centerX - 2, centerY - 2, true);
		SetBoardPosition(centerX - 1, centerY - 3, true);
		SetBoardPosition(centerX - 1, centerY - 5, true);

		SetBoardPosition(centerX + 1, centerY + 1, true);
		SetBoardPosition(centerX + 1, centerY    , true);
		SetBoardPosition(centerX + 1, centerY - 1, true);
		SetBoardPosition(centerX + 2, centerY + 1, true);
		SetBoardPosition(centerX + 2, centerY    , true);
		SetBoardPosition(centerX + 2, centerY - 1, true);
		SetBoardPosition(centerX + 3, centerY + 1, true);
		SetBoardPosition(centerX + 3, centerY - 1, true);
		SetBoardPosition(centerX + 4, centerY    , true);
		SetBoardPosition(centerX + 5, centerY    , true);
	}
	else if (pConfig == 4) //Glider gun
	{
		SetBoardPosition(centerX - 16, centerY - 1, true);
		SetBoardPosition(centerX - 16, centerY, true);
		SetBoardPosition(centerX - 15, centerY - 1, true);
		SetBoardPosition(centerX - 15, centerY, true);

		SetBoardPosition(centerX - 6, centerY - 2, true);
		SetBoardPosition(centerX - 6, centerY - 1, true);
		SetBoardPosition(centerX - 6, centerY, true);
		SetBoardPosition(centerX - 5, centerY - 3, true);
		SetBoardPosition(centerX - 5, centerY + 1, true);
		SetBoardPosition(centerX - 4, centerY + 2, true);
		SetBoardPosition(centerX - 3, centerY + 2, true);
		SetBoardPosition(centerX - 4, centerY - 4, true);
		SetBoardPosition(centerX - 3, centerY - 4, true);

		SetBoardPosition(centerX - 2, centerY - 1, true);
		SetBoardPosition(centerX - 1, centerY - 3, true);
		SetBoardPosition(centerX - 1, centerY + 1, true);
		SetBoardPosition(centerX, centerY, true);
		SetBoardPosition(centerX, centerY - 1, true);
		SetBoardPosition(centerX, centerY - 2, true);
		SetBoardPosition(centerX + 1, centerY - 1, true);

		SetBoardPosition(centerX + 4, centerY, true);
		SetBoardPosition(centerX + 4, centerY + 1, true);
		SetBoardPosition(centerX + 4, centerY + 2, true);
		SetBoardPosition(centerX + 5, centerY, true);
		SetBoardPosition(centerX + 5, centerY + 1, true);
		SetBoardPosition(centerX + 5, centerY + 2, true);
		SetBoardPosition(centerX + 6, centerY + 3, true);
		SetBoardPosition(centerX + 6, centerY - 1, true);

		SetBoardPosition(centerX + 8, centerY - 1, true);
		SetBoardPosition(centerX + 8, centerY - 2, true);
		SetBoardPosition(centerX + 8, centerY + 3, true);
		SetBoardPosition(centerX + 8, centerY + 4, true);

		SetBoardPosition(centerX + 18, centerY + 2, true);
		SetBoardPosition(centerX + 18, centerY + 1, true);
		SetBoardPosition(centerX + 19, centerY + 2, true);
		SetBoardPosition(centerX + 19, centerY + 1, true);
	}
	else if (pConfig == 5) //Oscillators
	{
		//Oscillator 1
		{
			SetBoardPosition(centerX - 22, centerY - 4, true);
			SetBoardPosition(centerX - 22, centerY - 3, true);
			SetBoardPosition(centerX - 22, centerY - 2, true);
			SetBoardPosition(centerX - 22, centerY - 1, true);
			SetBoardPosition(centerX - 22, centerY, true);
			SetBoardPosition(centerX - 22, centerY + 1, true);
			SetBoardPosition(centerX - 21, centerY - 4, true);
			SetBoardPosition(centerX - 21, centerY - 3, true);
			SetBoardPosition(centerX - 21, centerY - 2, true);
			SetBoardPosition(centerX - 21, centerY - 1, true);
			SetBoardPosition(centerX - 21, centerY, true);
			SetBoardPosition(centerX - 21, centerY + 1, true);

			SetBoardPosition(centerX - 22, centerY + 3, true);
			SetBoardPosition(centerX - 21, centerY + 3, true);
			SetBoardPosition(centerX - 20, centerY + 3, true);
			SetBoardPosition(centerX - 19, centerY + 3, true);
			SetBoardPosition(centerX - 18, centerY + 3, true);
			SetBoardPosition(centerX - 17, centerY + 3, true);
			SetBoardPosition(centerX - 22, centerY + 4, true);
			SetBoardPosition(centerX - 21, centerY + 4, true);
			SetBoardPosition(centerX - 20, centerY + 4, true);
			SetBoardPosition(centerX - 19, centerY + 4, true);
			SetBoardPosition(centerX - 18, centerY + 4, true);
			SetBoardPosition(centerX - 17, centerY + 4, true);

			SetBoardPosition(centerX - 19, centerY - 4, true);
			SetBoardPosition(centerX - 18, centerY - 4, true);
			SetBoardPosition(centerX - 17, centerY - 4, true);
			SetBoardPosition(centerX - 16, centerY - 4, true);
			SetBoardPosition(centerX - 15, centerY - 4, true);
			SetBoardPosition(centerX - 14, centerY - 4, true);
			SetBoardPosition(centerX - 19, centerY - 3, true);
			SetBoardPosition(centerX - 18, centerY - 3, true);
			SetBoardPosition(centerX - 17, centerY - 3, true);
			SetBoardPosition(centerX - 16, centerY - 3, true);
			SetBoardPosition(centerX - 15, centerY - 3, true);
			SetBoardPosition(centerX - 14, centerY - 3, true);

			SetBoardPosition(centerX - 15, centerY + 4, true);
			SetBoardPosition(centerX - 15, centerY + 3, true);
			SetBoardPosition(centerX - 15, centerY + 2, true);
			SetBoardPosition(centerX - 15, centerY + 1, true);
			SetBoardPosition(centerX - 15, centerY, true);
			SetBoardPosition(centerX - 15, centerY - 1, true);
			SetBoardPosition(centerX - 14, centerY + 4, true);
			SetBoardPosition(centerX - 14, centerY + 3, true);
			SetBoardPosition(centerX - 14, centerY + 2, true);
			SetBoardPosition(centerX - 14, centerY + 1, true);
			SetBoardPosition(centerX - 14, centerY, true);
			SetBoardPosition(centerX - 14, centerY - 1, true);
		}

		//Oscillator 2
		{
			SetBoardPosition(centerX + 1, centerY + 1, true);
			SetBoardPosition(centerX + 1, centerY, true);
			SetBoardPosition(centerX + 1, centerY - 1, true);
			SetBoardPosition(centerX + 1, centerY - 2, true);
			SetBoardPosition(centerX + 1, centerY - 3, true);

			SetBoardPosition(centerX + 3, centerY + 1, true);
			SetBoardPosition(centerX + 3, centerY, true);
			SetBoardPosition(centerX + 3, centerY - 1, true);
			SetBoardPosition(centerX + 3, centerY - 2, true);
			SetBoardPosition(centerX + 3, centerY - 3, true);

			SetBoardPosition(centerX + 2, centerY - 3, true);
			SetBoardPosition(centerX + 2, centerY + 1, true);
		}

		//Oscillator 3
		{
			SetBoardPosition(centerX + 18, centerY + 1, true);
			SetBoardPosition(centerX + 18, centerY + 2, true);
			SetBoardPosition(centerX + 18, centerY + 3, true);
			SetBoardPosition(centerX + 19, centerY + 1, true);
			SetBoardPosition(centerX + 19, centerY + 2, true);
			SetBoardPosition(centerX + 19, centerY + 3, true);
			SetBoardPosition(centerX + 20, centerY + 1, true);
			SetBoardPosition(centerX + 20, centerY + 2, true);
			SetBoardPosition(centerX + 20, centerY + 3, true);

			SetBoardPosition(centerX + 21, centerY - 2, true);
			SetBoardPosition(centerX + 21, centerY - 1, true);
			SetBoardPosition(centerX + 21, centerY, true);
			SetBoardPosition(centerX + 22, centerY - 2, true);
			SetBoardPosition(centerX + 22, centerY - 1, true);
			SetBoardPosition(centerX + 22, centerY, true);
			SetBoardPosition(centerX + 23, centerY - 2, true);
			SetBoardPosition(centerX + 23, centerY - 1, true);
			SetBoardPosition(centerX + 23, centerY, true);
		}
	}
}
