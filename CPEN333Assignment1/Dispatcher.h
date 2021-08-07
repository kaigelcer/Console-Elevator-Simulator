#pragma once
#include <stdlib.h>
#include <list>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include "Elevator.h"
#include <conio.h>
#include <time.h>
using namespace std;


class Dispatcher : public ActiveClass{
	
	const static int NUM_ELEVATORS = 5;
	string floorStrings[10] = { "th", "st", "nd", "rd", "th", "th", "th", "th", "th", "th" };
	Elevator elevators[NUM_ELEVATORS];
	bool inService = true;
	char firstInput;
	char secondInput;
	string dispatcherMessage;
	bool dynamicPassengersOn = false;


public:

	int main(void) {
		firstInput = '_';
		secondInput = '_';
		dispatcherMessage = "______";

		for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
			elevators[elNum].setNumber(elNum + 1);
		}
		for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
			elevators[elNum].Resume();
		}

		
		ClassThread<Dispatcher> inputThread(this, &Dispatcher::inputs, ACTIVE, NULL);
		ClassThread<Dispatcher> passenger(this, &Dispatcher::newDynamicPassenger, SUSPENDED, NULL);
		while (inService) {
			system("CLS");
			drawElevators();
			if (dynamicPassengersOn) {
				passenger.Resume();
			}
			inService = false;
			for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
				if (!(elevators[elNum].inService == false && elevators[elNum].currentFloor == 0 && elevators[elNum].elevatorMessage == "Doors Open")) {
					inService = true;
				}
			}

		}

		Sleep(3000);
		elevators[0].WaitForThread();
		elevators[1].WaitForThread();
		return 0;
	}

	// Deal with user inputs
	int inputs(void* args) {

		while (inService) {
			int message;
			firstInput = _getch();
			int elevator;

			switch (firstInput) {

			case 'u':
				secondInput = _getch();
				message = 10;
				break;
			case 'd':
				secondInput = _getch();
				if (secondInput == '+') {
					dispatcherMessage = "Creating dynamic passenger";
					dynamicPassengersOn = true;
					continue;
				}
				else if (secondInput == '-') {
					dynamicPassengersOn = false;
					continue;
				}
				else {
					message = 20;
					break;
				}
			case '-':
				secondInput = _getch();
				elevator = (int)secondInput - '0';
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (elevator == elNum + 1) {
						elevators[elNum].pause();
					}
				}
				continue;
			case '+':
				secondInput = _getch();
				elevator = (int)secondInput - '0';
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (elevator == elNum + 1) {
						elevators[elNum].start();
					}
				}
				continue;
			case 'e':
				secondInput = _getch();
				if (secondInput == 'e') {
					for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
						elevators[elNum].stop();
					}
				}
				continue;

			// Default assumes an INSIDE command
			default:
				elevator = (int)firstInput - '0';
				secondInput = _getch();
				int nextFloor = (int)secondInput - '0';
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (elevator == elNum + 1 && elevators[elNum].awaitingPassengerInput&& nextFloor >= 0 && nextFloor < 9) {
						elevators[elNum].Post(nextFloor);
						elevators[elNum].awaitingPassengerInput = false;
						elevators[elNum].updateQueueString();
					}
				}
				continue;
			}

			// Deal with an OUTSIDE up or down request
			int fromFloor = (int)secondInput - '0';
			if (fromFloor >= 0 && fromFloor <= 9) {
				message = message + fromFloor;
				int requestDirection = message / 10;
				bool sent = false;

				// If any elevator has an empty queue, send the request there
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (elevators[elNum].isPaused()) {
						continue;
					}
					else if (elevators[elNum].queue.empty()) {
						elevators[elNum].Post(message);
						elevators[elNum].updateQueueString();
						sent = true;
						break;
					}
				}

				// Otherwise, if an elevator is travelling towards/past the request,
				// and the request direction is the same as the elevator's current direction,
				// send request to that elevator
				if (sent == false) {
					for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
						if (elevators[elNum].isPaused()) {
							continue;
						}
						else if (elevators[elNum].currentDirection == requestDirection && xBetweenYandZ(fromFloor, elevators[elNum].currentFloor, elevators[elNum].nextFloor)) {
							elevators[elNum].Post(message);
							elevators[elNum].updateQueueString();
							sent = true;
							break;
						}
					}
				}

				// Otherwise send request to elevator with shortest queue
				if (sent == false) {
					int shortestQueueLength = 100; // Arbitrary large number
					int shortestQueueIndex = -1;
					for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
						if (!(elevators[elNum].isPaused()) && elevators[elNum].queue.size() < shortestQueueLength) {
							shortestQueueIndex = elNum;
							shortestQueueLength = elevators[elNum].queue.size();
						}
					}
					if (shortestQueueIndex != -1) {
						elevators[shortestQueueIndex].Post(message);
						elevators[shortestQueueIndex].updateQueueString();
						sent = true;
					}
				}

			}

			else {
				continue;
			}
		}

		return 0;
	}


	bool xBetweenYandZ(int x, int y, int z) {
		return ((y <= x && x <= z) || (z <= x) && (x <= y));
	}


	void drawElevators(void) {
		string emptyShaft = "|     |";
		for (int floor = 9; floor >= 0; floor--) {
			cout << "             ";
						for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
							if (floor == elevators[elNum].currentFloor) {
								cout << elevators[elNum].elevatorTop;
							}
							else {
								cout << emptyShaft;
							}
						}
						cout << '\n';
						cout << "             ";
						for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
							if (floor == elevators[elNum].currentFloor) {
								cout << elevators[elNum].elevatorMiddle;
							}
							else {
								cout << emptyShaft;
							}
						}

						cout << '\n';
						cout << "             ";
						for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
							if (floor == elevators[elNum].currentFloor) {
								cout << elevators[elNum].elevatorMiddle;
							}
							else {
								cout << emptyShaft;
							}
						}
						cout << '\n';
						cout << floor << floorStrings[floor] << " Floor____";
						for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
							if (floor == elevators[elNum].currentFloor) {
								cout << elevators[elNum].elevatorBottom;
							}
							else {
								cout << emptyShaft;
							}
						}
						cout << "____Floor " << floor << '\n';
					}
		cout << "First Input: " << firstInput << '\n';
		cout << "Second Input: " << secondInput << '\n';
		cout << "Dispatcher says: " << dispatcherMessage << '\n';
		for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
			cout << "Elevator " << elevators[elNum].getNumber() << " Status: " << elevators[elNum].elevatorMessage << "     ";
		}
		cout << "\n";
		for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
			cout << "Elevator " << elevators[elNum].getNumber() << " Queue: " << elevators[elNum].queueString << "     ";
		}
		cout << "\n";
	}

	int newDynamicPassenger(void* args) {
		srand(time(NULL));
		while (true) {

			if (!dynamicPassengersOn) {
				break;
			}

			int fromFloor = rand() % 10;
			int requestDirection = (rand() % 2 + 1) * 10;
			int toFloor;
			if (requestDirection == 10) {
				toFloor = rand() % (10 - fromFloor) + fromFloor + 1;
			}
			else {
				toFloor = rand() % (fromFloor+1);

			}
			int message = requestDirection + fromFloor;
			Elevator* chosenElevator = &elevators[0];
			bool sent = false;

			// If any elevator has an empty queue, send the request there
			for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
				if (elevators[elNum].isPaused()) {
					continue;
				}
				else if (elevators[elNum].queue.empty()) {
					elevators[elNum].Post(message);
					chosenElevator = &elevators[elNum];
					elevators[elNum].updateQueueString();
					sent = true;
					break;
				}
			}

			// Otherwise, if an elevator is travelling towards/past the request,
			// and the request direction is the same as the elevator's current direction,
			// send request to that elevator
			if (sent == false) {
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (elevators[elNum].isPaused()) {
						continue;
					}
					else if (elevators[elNum].currentDirection == requestDirection && xBetweenYandZ(fromFloor, elevators[elNum].currentFloor, elevators[elNum].nextFloor)) {
						elevators[elNum].Post(message);
						chosenElevator = &elevators[elNum];
						elevators[elNum].updateQueueString();
						sent = true;
						break;
					}
				}
			}

			// Otherwise send request to elevator with shortest queue
			if (sent == false) {
				int shortestQueueLength = 100; // Arbitrary large number
				int shortestQueueIndex = -1;
				for (int elNum = 0; elNum < NUM_ELEVATORS; elNum++) {
					if (!(elevators[elNum].isPaused()) && elevators[elNum].queue.size() < shortestQueueLength) {
						shortestQueueIndex = elNum;
						shortestQueueLength = elevators[elNum].queue.size();
					}
				}
				if (shortestQueueIndex != -1) {
					elevators[shortestQueueIndex].Post(message);
					chosenElevator = &elevators[shortestQueueIndex];
					elevators[shortestQueueIndex].updateQueueString();
					sent = true;
				}
			}

			while (true) {
				if (chosenElevator->currentFloor == fromFloor && chosenElevator->awaitingPassengerInput) {
					Sleep(500);
					chosenElevator->Post(toFloor);
					break;
				}
			}

			chosenElevator->awaitingPassengerInput = false;
			Sleep(500);

		}

		return 0;

	}

};
