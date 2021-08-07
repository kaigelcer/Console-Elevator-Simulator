#include <stdlib.h>
#include <thread>
#include "Dispatcher.h"


int main() {
	
	Dispatcher dispatcher;
	dispatcher.Resume();
	dispatcher.WaitForThread();

	return 0;

}
