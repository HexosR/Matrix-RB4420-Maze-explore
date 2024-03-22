/*
 * File: rom57_controller.c
 * Author: Robert Mlynarczyk
 *
 * Aberystwyth University 2020
 *
 */

#include "allcode_api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RIGHT_ANGLE 90
#define LEFT_ANGLE 90
#define FORWARD_AFTER_BLACK_LINE 110
#define IR_4_RIGHT_TRESHOLD_NO_WALL 100
#define IR_2_FORWARD_TRESHOLD 1500
#define BLACK_LINE_TRESHOLD 20

double LIGHT_OFF_TRESHOLD;
double IR_4_RIGHT_TRESHOLD_NEAR;
double IR_4_RIGHT_TRESHOLD_FAR;

int number = 11; //variable used for the display functions
int all[11][11]; //maze array
int roof = 0;    //variable used to count all nests
int robot_X = 10;//starting points in the array
int robot_Y = 0;
int X = 0;       //starting coordinates 
int Y = 0;
char direction = 'N';
bool go_right = false;//variable used to define if the robot need to turn left or right

//-----------------Shift-Maze-Array---------------
//  Two functions to shift maze array right and up
//  because robot starts at the 0,10 coordinates 
void shiftAllRight() {
	if (robot_Y == 0) {
		int i;
		int j;
		for (i = 9; i > -1; i--) { //shifts all nodes in the array to the right
			for (j = 0; j < 11; j++) {
				all[j][i + 1] = all[j][i];
			}
		}
		for (i = 0; i < 11; i++) { //puts zeros in the leftmost column
			all[i][0] = 0;
		}
		robot_Y += 1;
	}
}
void shiftAllUp() {
	if (robot_X == 10) {
		int i;
		int j;
		for (i = 1; i < 11; i++) { //shifts all nodes in the array up
			for (j = 0; j < 11; j++) {
				all[i - 1][j] = all[i][j];
			}
		}
		for (i = 0; i < 11; i++) { //puts zeros in the lowest row
			all[10][i] = 0;
		}
		robot_X -= 1;
	}
}
//------------------------------------------------


//--------------Compass---------------------------
//  Two functions that will change the direction
//  of the robot after turning 
char nextDestination(char dir) {
	if (dir == 'N') {
		return 'E';
	}
	else if (dir == 'E') {
		return 'S';
	}
	else if (dir == 'S') {
		return 'W';
	}
	else if (dir == 'W') {
		return 'N';
	}
}
char previousDestination(char dir) {
	if (dir == 'N') {
		return 'W';
	}
	else if (dir == 'E') {
		return 'N';
	}
	else if (dir == 'S') {
		return 'E';
	}
	else if (dir == 'W') {
		return 'S';
	}
}
//------------------------------------------------

//--------------Displays--LCD---------------------
//  Three functions to display all informations 
//  to the LCD and via Bluetooth
//  Before program will run and after exploring whole maze
void displayLCDStart() {
	FA_LCDClear();
	FA_LCDPrint("Please put me in", 17, 0, 0, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("the maze and after", 18, 0, 8, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("pushing left button", 20, 0, 16, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("we will start", 14, 0, 24, FONT_NORMAL, LCD_OPAQUE);
	printf("\nPlease put me in the maze and after pushing left button we will start\n");
	FA_Switch0WaitHigh();
	FA_Switch0WaitLow();
	FA_DelaySecs(2); // delay 2 seconds, which will minimalise the chance of touching the robot while the program is running
	FA_LCDClear();
}
void displayAndEnd() {
	int i;
	int j;
	FA_LCDClear();
	FA_LCDPrint("Encountered", 12, 0, 0, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDNumber(roof, 72, 0, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("nested areas", 13, 0, 8, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("Maze will be displa-", 21, 0, 16, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("yed via Bluetooth", 18, 0, 24, FONT_NORMAL, LCD_OPAQUE);
	printf("Encountered %d nested areas.\nOn map below:\n1 - WALL\n0 - NO WALL\n8 - NEST\n", roof);
	for (i = 0; i < 11; i++) { //prints whole maze array
		for (j = 0; j < 11; j++) {
			printf("%d", all[i][j]);
			if (j == 10) {
				printf("\n");
			}
			else {
				printf(" ");
			}
		}
	}
	FA_DelaySecs(10);
	FA_LCDClear();
	FA_LCDPrint("It works and whole", 19, 0, 0, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("maze is explored.", 18, 0, 8, FONT_NORMAL, LCD_OPAQUE);
	FA_LCDPrint("The End.", 9, 0, 16, FONT_NORMAL, LCD_OPAQUE);
	printf("It works and whole maze is explored.\nThe End.\n");
	FA_PlayNote(500, 500);
	while (1) {
		FA_LEDOn(0);
		FA_LEDOff(1);
		FA_LEDOn(2);
		FA_LEDOff(3);
		FA_LEDOff(4);
		FA_LEDOn(5);
		FA_LEDOff(6);
		FA_LEDOn(7);
		FA_DelaySecs(1);
		FA_LEDOff(0);
		FA_LEDOn(1);
		FA_LEDOff(2);
		FA_LEDOn(3);
		FA_LEDOn(4);
		FA_LEDOff(5);
		FA_LEDOn(6);
		FA_LEDOff(7);
		FA_DelaySecs(1);
	}
}
void displayCountdown() {
	number--;
	switch (number) {
	case 10:
		FA_LCDClear();
		FA_LCDPrint("      ##   #####", 17, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("     ###  ##   ##", 18, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("      ##  ##   ##", 18, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("      ##  ##   ##", 18, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("     ####  #####", 17, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("10\n");
		break;

	case 9:
		FA_LCDClear();
		FA_LCDPrint("         #####", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         ######", 16, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("             ##", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         #####", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("9\n");
		break;

	case 8:
		FA_LCDClear();
		FA_LCDPrint("         #####", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         #####", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         #####", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("8\n");
		break;
	case 7:
		FA_LCDClear();
		FA_LCDPrint("        #######", 16, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("            ##", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("           ##", 14, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("          ##", 13, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("7\n");
		break;
	case 6:
		FA_LCDClear();
		FA_LCDPrint("         #####", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##", 11, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ######", 16, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         #####", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("6\n");
		break;
	case 5:
		FA_LCDClear();
		FA_LCDPrint("        ######", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##", 11, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ######", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("             ##", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ######", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("5\n");
		break;
	case 4:
		FA_LCDClear();
		FA_LCDPrint("          ####", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         ## ##", 15, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##  ##", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        #######", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("            ##", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("4\n");
		break;
	case 3:
		FA_LCDClear();
		FA_LCDPrint("        #######", 16, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("             ##", 16, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("           ###", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         #####", 15, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("3\n");
		break;
	case 2:
		FA_LCDClear();
		FA_LCDPrint("         #####", 15, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        ##   ##", 16, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("           ###", 15, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         ###", 13, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("        #######", 16, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("2\n");
		break;
	case 1:
		FA_LCDClear();
		FA_LCDPrint("          ##", 13, 0, 0, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         ###", 13, 0, 6, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("          ##", 13, 0, 12, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("          ##", 13, 0, 18, FONT_NORMAL, LCD_OPAQUE);
		FA_LCDPrint("         ####", 14, 0, 24, FONT_NORMAL, LCD_OPAQUE);
		printf("1\n");
		break;
	default:
		break;
	}
}
//------------------------------------------------

//--------------Initialize_IR's-------------------
//  Function to calculate average of the right IR4 
//  and light sensor and save these values in the variables
void initializeIRs() {
	int i;
	long clock1 = FA_ClockMS();
	long clock2 = FA_ClockMS();
	for (i = 0; i < 110; i++) {
		LIGHT_OFF_TRESHOLD += FA_ReadLight();
		IR_4_RIGHT_TRESHOLD_NEAR += FA_ReadIR(IR_RIGHT);
		while (FA_ClockMS() < clock1 + 100) { //loop continuously executed for 100ms
			if (clock2 + 1000 < FA_ClockMS()) { // loop executed after 1 second
				displayCountdown();
				clock2 = FA_ClockMS();
			}
		}
		clock1 = FA_ClockMS();
	}

	LIGHT_OFF_TRESHOLD = (LIGHT_OFF_TRESHOLD / 110) / 4;
	IR_4_RIGHT_TRESHOLD_NEAR = (IR_4_RIGHT_TRESHOLD_NEAR / 110) * 3;
	IR_4_RIGHT_TRESHOLD_FAR = IR_4_RIGHT_TRESHOLD_NEAR - 400;

	//printf("%.0f\n%.0f\n%.0f\n\n",LIGHT_OFF_TRESHOLD,IR_4_RIGHT_TRESHOLD_NEAR,IR_4_RIGHT_TRESHOLD_FAR);
	printf("Values for IR 4 and Light Sensor initialized.\nStarting!\n");
	FA_PlayNote(320, 500);
}
//------------------------------------------------

//--------------Start-Position--------------------
// Function that will position the robot 
// in the position to start exploring the maze
void initializeWalls() { //robot will move to the middle of the node and position itself near the right wall
	while (FA_ReadIR(IR_REAR) > 2000) {
		FA_Forwards(10);
	}
	while (FA_ReadIR(IR_FRONT) > 2000) {
		FA_Backwards(10);
	}
	while (FA_ReadIR(IR_LEFT) > 450) {
		FA_Right(11);
		FA_Forwards(8);
		FA_Left(9);
		FA_Backwards(7);
	}
	while (FA_ReadIR(IR_RIGHT) < IR_4_RIGHT_TRESHOLD_NO_WALL) {
		FA_Right(90);
	}
	while (FA_ReadIR(IR_RIGHT) < 1000) {
		FA_Right(11);
		FA_Forwards(8);
		FA_Left(9);
		FA_Backwards(7);
	}
}
//------------------------------------------------

//---------------Check-Nest-----------------------
//  Function that uses Light Sensor to check
//  if there is a nest above the robot 
void checkNest() {
	if (FA_ReadLight() < LIGHT_OFF_TRESHOLD) {
		++roof;
		all[robot_X][robot_Y] = 8;
		FA_PlayNote(120, 500);
	}
}
//------------------------------------------------

//------Functions-To-Explore-The-Maze-------------
// Three functions to change and check the position of the robot
// Change the direction after turns and save all data in the maze array
void stickToRightWall() {
	if (FA_ReadIR(IR_RIGHT) < IR_4_RIGHT_TRESHOLD_FAR) {
		FA_Right(11);
		FA_Forwards(8);
		FA_Left(9);
		FA_Backwards(7);
	}
	else if (FA_ReadIR(IR_RIGHT) > IR_4_RIGHT_TRESHOLD_NEAR) {
		FA_Left(3);
	}
}
void ifBlackStripeOrWallAndGoLeft() {
	if (FA_ReadLine(CHANNEL_LEFT) < BLACK_LINE_TRESHOLD || FA_ReadLine(CHANNEL_RIGHT) < BLACK_LINE_TRESHOLD) {
		if (direction == 'N') {
			all[robot_X][robot_Y + 1] = 1;
			all[robot_X - 1][robot_Y + 1] = 1;
			robot_X -= 2;
			X++;
		}
		else if (direction == 'E') {
			shiftAllUp();
			all[robot_X + 1][robot_Y] = 1;
			all[robot_X + 1][robot_Y + 1] = 1;
			robot_Y += 2;
			Y++;
		}
		else if (direction == 'S') {
			shiftAllRight();
			shiftAllUp();
			all[robot_X][robot_Y - 1] = 1;
			all[robot_X + 1][robot_Y - 1] = 1;
			robot_X++;
			shiftAllUp();
			robot_X++;
			X--;
		}
		else if (direction == 'W') {
			shiftAllRight();
			all[robot_X - 1][robot_Y - 1] = 1;
			all[robot_X - 1][robot_Y] = 1;
			robot_Y--;
			shiftAllRight();
			robot_Y--;
			Y--;
		}
		FA_Forwards(FORWARD_AFTER_BLACK_LINE);
		checkNest();
		if (FA_ReadIR(IR_RIGHT) < IR_4_RIGHT_TRESHOLD_NO_WALL) {
			FA_Right(RIGHT_ANGLE);
			direction = nextDestination(direction);
			go_right = true;
		}
	}
	else if (FA_ReadIR(IR_FRONT) > IR_2_FORWARD_TRESHOLD) { //checks forward IR
		if (direction == 'N') {
			all[robot_X][robot_Y + 1] = 1;
			all[robot_X - 1][robot_Y] = 1;
			all[robot_X - 1][robot_Y + 1] = 1;
		}
		else if (direction == 'E') {
			shiftAllUp();
			all[robot_X + 1][robot_Y] = 1;
			all[robot_X][robot_Y + 1] = 1;
			all[robot_X + 1][robot_Y + 1] = 1;
		}
		else if (direction == 'S') {
			shiftAllRight();
			shiftAllUp();
			all[robot_X + 1][robot_Y] = 1;
			all[robot_X][robot_Y - 1] = 1;
			all[robot_X + 1][robot_Y - 1] = 1;
		}
		else if (direction == 'W') {
			shiftAllRight();
			all[robot_X - 1][robot_Y] = 1;
			all[robot_X][robot_Y - 1] = 1;
			all[robot_X - 1][robot_Y - 1] = 1;
		}
		FA_Left(LEFT_ANGLE);
		direction = previousDestination(direction);
	}
}
void IfBlackStripeAndGoRight() {
	if (FA_ReadLine(LCD_OPAQUE) < BLACK_LINE_TRESHOLD || FA_ReadLine(LCD_TRANSPARENT) < BLACK_LINE_TRESHOLD) {
		if (direction == 'N') {
			robot_X -= 2;
			X++;
		}
		else if (direction == 'E') {
			robot_Y += 2;
			Y++;
		}
		else if (direction == 'S') {
			robot_X++;
			shiftAllUp();
			shiftAllRight();
			robot_X++;
			shiftAllUp();
			shiftAllRight();
			X--;
		}
		else if (direction == 'W') {
			robot_Y--;
			shiftAllRight();
			robot_Y--;
			shiftAllRight();
			Y--;
		}
		FA_Forwards(FORWARD_AFTER_BLACK_LINE);
		checkNest();
		if (FA_ReadIR(4) < IR_4_RIGHT_TRESHOLD_NO_WALL) {
			FA_Right(RIGHT_ANGLE);
			direction = nextDestination(direction);
			go_right = true;
		}
		else {
			go_right = false;
		}
	}
}
//------------------------------------------------

int main() {
	//----Init---
	FA_RobotInit();
	FA_LCDBacklight(100);
	FA_SetDriveSpeed(10);
	XFA_ClockMS_Initialise();
	//-----------
	while (1) {
		if (FA_BTConnected() == 1) {
			displayLCDStart();
			initializeIRs();
			FA_LCDClear();
			FA_LCDPrint("Initializing point", 19, 0, 0, FONT_NORMAL, LCD_OPAQUE);
			FA_LCDPrint("to start", 9, 0, 7, FONT_NORMAL, LCD_OPAQUE);
			printf("Initializing point to start\n");
			initializeWalls();
			FA_LCDClear();
			FA_LCDPrint("Exploring The Maze!", 20, 0, 0, FONT_NORMAL, LCD_OPAQUE);
			printf("Exploring The Maze!\n");
			//--EXPLORING--
			while (1) {
				if (roof > 0 && X == 0 && Y == 0 && direction == 'N') { //end of the program when roof counts is more than zero 
					displayAndEnd();                                    //so robot encounter at least one nest
				}                                                       //X and Y equals to zero so robot is in the first node
				else {                                                  //and faces north as in th beginning of the program                                                 
					FA_SetMotors(10, 10);
					if (go_right == false) {
						stickToRightWall();
						ifBlackStripeOrWallAndGoLeft();
					}
					else {
						IfBlackStripeAndGoRight();
					}
				}
			}
			//--------------
		}
		else {
			FA_LCDClear();
			FA_LCDPrint("Please connect me", 18, 1, 0, FONT_NORMAL, LCD_OPAQUE);
			FA_LCDPrint("with your bluetooth", 20, 1, 11, FONT_NORMAL, LCD_OPAQUE);
			FA_LCDPrint("device! :D", 11, 1, 22, FONT_NORMAL, LCD_OPAQUE);
			FA_DelayMillis(200);
		}
	}
}


