#include <GLFW/glfw3.h>
#include "GameBoard.h"

//Constants
#define _SCREEN_WIDTH 1600
#define _SCREEN_HEIGHT 900
#define _CELL_SIZE 20
#define _INITIAL_FRAMESPEED 0.1

//Global variables
GameBoard board(_SCREEN_WIDTH, _SCREEN_HEIGHT, _CELL_SIZE);
bool leftClickDown = false;
bool rightClickDown = false;
bool paused = false;
bool hasQuit = false; //Set to true when the player presses 'q' or 'esc'
float frameSpeed = _INITIAL_FRAMESPEED;

//If the left or right click has been pressed
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//If the left click has been pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			leftClickDown = true;
		else if (action == GLFW_RELEASE)
			leftClickDown = false;
	}
	
	//If the right click has been pressed
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			rightClickDown = true;
		else if (action == GLFW_RELEASE)
			rightClickDown = false;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//'R' key resets the board
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		board.ResetBoard();

	//'Space' key pauses/unpauses the board
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		paused = !paused;

	//'+' key increases the board speed
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		frameSpeed /= 2;
		if(frameSpeed < _INITIAL_FRAMESPEED / 16)
			frameSpeed = _INITIAL_FRAMESPEED / 16;
	}

	//'-' key increases the board speed
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		frameSpeed *= 2;
		if (frameSpeed > _INITIAL_FRAMESPEED * 4)
			frameSpeed = _INITIAL_FRAMESPEED * 4;
	}

	//'esc' or 'q' key closes the program
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) 
	{
		hasQuit = true;
		glfwTerminate();
	}

	//'1' to '5' load a premade sequence
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		board.LoadPreconfiguration(1);
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		board.LoadPreconfiguration(2);
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		board.LoadPreconfiguration(3);
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		board.LoadPreconfiguration(4);
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		board.LoadPreconfiguration(5);
}

int main(void)
{
	//-----------------Initialize openGL window------------------------------//
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	//Create the window
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(_SCREEN_WIDTH, _SCREEN_HEIGHT, "Game of Life", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Make the new window appear in front of all other windows
	glfwMakeContextCurrent(window);

	//Event handlers
	glfwSetMouseButtonCallback(window, mouse_button_callback); //Mouseclick event
	glfwSetKeyCallback(window, key_callback); //Keyboard event

	//-----------------Initialize Game of Life varianbles--------------------//
	double xpos, ypos; //The x and y of the mouse

	//Give the board an initial pattern
	board.LoadPreconfiguration(2);

	//Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Get the position of the mouse
		glfwGetCursorPos(window, &xpos, &ypos);

		//Enable/disable the cells that the player left/right clicks on
		if (leftClickDown) 
		{
			board.MouseClick(xpos, ypos, true);
		}
		else if (rightClickDown)
		{
			board.MouseClick(xpos, ypos, false);
		}

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1, 1, 1, 1); //Set the background color to all white
		
		//Drawing
		board.DrawBoard(); //Draw the board
		board.DrawGrid(); //Draw the gridlines

		//Draw the cell outline we're hovering over
		board.DrawMouseHover(xpos, ypos);

		//Calculate the next frame if a certain amount of time has elapsed
		if (glfwGetTime() > frameSpeed && !paused)
		{
			board.CalculateNextFrame();
			glfwSetTime(0);
		}

		//Double buffer
		glfwSwapBuffers(window);

		//Poll for events
		glfwPollEvents();

		if (hasQuit)
			return 0;
	}

	glfwTerminate();
	return 0;
}