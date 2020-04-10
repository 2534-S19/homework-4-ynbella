/*
 * homework4.h
 *
 *  Created on: April 09, 2020
 *      Author: Youness Bella
 */

#ifndef HOMEWORK4_H_
#define HOMEWORK4_H_

// This function initializes the board by turning off the Watchdog Timer.
void initBoard();
bool charFSM(char rChar);

// Define any constants that are local to homework.c using #define
enum State {FIRST, SECOND, THIRD, FOURTH}; // Used to keep track of FSM state

#endif /* HOMEWORK4_H_ */
