#include <stdlib.h>
#include <list>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include "C:\Users\kaige\source\repos\rt.h"


typedef struct request {
	int direction;
	int floor;
} Request;

class Elevator : public ActiveClass {

	int elevatorNumber;
	
	const int UP = 1;
	const int DOWN = -1;
	bool paused;

public:

	int currentDirection; // 1 for up, 0 for down
	vector<Request> queue;
	bool inService;
	int currentFloor;
	int nextFloor;
	bool awaitingPassengerInput;
	string elevatorName;
	string elevatorMessage;
	string queueString = "";
	string elevatorBottom = "|__|__|";
	string elevatorMiddle = "|  |  |";
	string elevatorTop = "|--0--|";

	int main(void) {
		// Start at floor 0 and must go up
		currentFloor = 0;
		currentDirection = 1;
		elevatorMessage = "";
		inService = true;
		paused = false;
		awaitingPassengerInput = false;
		CMailbox mailbox;


		while (inService) {

			while (paused) {
				elevatorMessage = "Paused...";
				awaitingPassengerInput = false;
			}

			if (mailbox.TestForMessage()) {
				int message = mailbox.GetMessage();
				queue.push_back(parseMessageToRequest(message));
			}

			updateQueueString();

			if (!queue.empty() && !awaitingPassengerInput) {
				nextFloor = queue.front().floor;
				updateCurrentDirection();
				for (currentFloor; currentFloor != nextFloor && !awaitingPassengerInput; currentFloor = currentFloor + currentDirection) {
					setFloor(currentFloor);
					if (mailbox.TestForMessage()) {
						int message = mailbox.GetMessage();
						queue.push_back(parseMessageToRequest(message));
						updateQueueString();
					}

					elevatorMessage = "Moving ";
					updateCurrentDirection();
					if (paused || !inService) {
						break;
					}

					for (size_t i = 0; i < queue.size(); i++) {
						if (queue[i].floor == currentFloor && queue[i].direction == currentDirection) {
							setFloor(currentFloor);
							letPassengerOn();
							queue.erase(queue.begin() + i);
						}
						else if (queue[i].floor == currentFloor && queue[i].direction == 0) {
							setFloor(currentFloor);
							letPassengerOff();
							queue.erase(queue.begin() + i);
						}
						else if (currentFloor == nextFloor) {
							setFloor(currentFloor);
							letPassengerOn();
							queue.erase(queue.begin() + i);
						}

					}
					setFloor(currentFloor);

					

					Sleep(2500);
					if (awaitingPassengerInput) {
						break;
					}
				}

				for (size_t i = 0; i < queue.size(); i++) {
					if (queue[i].floor == currentFloor && queue[i].direction == currentDirection) {
						letPassengerOn();
						queue.erase(queue.begin() + i);
					}
					else if (queue[i].floor == currentFloor && queue[i].direction == 0) {
						letPassengerOff();
						queue.erase(queue.begin() + i);
					}
					else if (currentFloor == nextFloor) {
						letPassengerOn();
						queue.erase(queue.begin() + i);
					}
				}
			}
			else if (!awaitingPassengerInput)
				elevatorMessage = "Waiting...";

		}
		setFloor(currentFloor);
		for (int i = currentFloor; i >= 0; i--) {
			currentFloor = i;
			setFloor(currentFloor);
			Sleep(1500);
		}
		openDoor();
		elevatorMessage = "Doors Open";
		Sleep(3000);


		return 0;
	}


	void setNumber(int num) {
		elevatorNumber = num;
	}

	void updateQueueString() {
		queueString = "";
		for (size_t i = 0; i < queue.size(); i++) {
			queueString = queueString + to_string(queue[i].floor) + ", ";
		}
	}

	void updateCurrentDirection() {
		if (nextFloor - currentFloor != 0) {
			currentDirection = (nextFloor - currentFloor) / abs(nextFloor - currentFloor);
		}
	}

	void setFloor(int floorNum) {
		elevatorTop = (string)("|--") + to_string(floorNum) + (string)("--|");
	}

	int getNumber() {
		return elevatorNumber;
	}

	void letPassengerOff() {
		setFloor(currentFloor);
		elevatorMessage = "Letting off Passenger ";
		updateQueueString();
		openDoor();
		Sleep(1000);
		closeDoor();
		Sleep(1000);
	}

	void letPassengerOn() {
		setFloor(currentFloor);
		elevatorMessage = "Letting on Passenger ";
		awaitingPassengerInput = true;
		updateQueueString();
		openDoor();
		Sleep(1000);
		closeDoor();
		Sleep(1000);
	}

	// Takes two digit integer message and parses it to request structure
	Request parseMessageToRequest(int message) {
		int direction = message / 10;
		int floor = message % 10;
		if (direction == 2) {
			direction = -1;
		}
		Request newRequest;
		newRequest.direction = direction;
		newRequest.floor = floor;
		return newRequest;
	}

	void start() {
		inService = true;
		paused = false;
	}

	void pause() {
		queue.clear();
		paused = true;

	}

	void stop() {
		queue.clear();
		inService = false;
		
	}

	bool isPaused() {
		return paused;
	}

	void openDoor() {
		Sleep(1000);
		elevatorBottom = "|_|_|_|";
		elevatorMiddle = "| | | |";
		Sleep(1000);
		elevatorBottom = "||___||";
		elevatorMiddle = "||   ||";
		Sleep(1000);
		elevatorBottom = "|_____|";
		elevatorMiddle = "|     |";
	}

	void closeDoor() {
		Sleep(1000);
		elevatorBottom = "||___||";
		elevatorMiddle = "||   ||";
		Sleep(1000);
		elevatorBottom = "|_|_|_|";
		elevatorMiddle = "| | | |";
		Sleep(1000);
		elevatorBottom = "|__|__|";
		elevatorMiddle = "|  |  |";
	}

	void addRequestToQueue(Request request) {
		queue.push_back(request);
	}

};