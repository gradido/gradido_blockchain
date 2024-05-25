#include "gradido_blockchain/ServerApplication.h"
#include <csignal>

/*
* Constant	Explanation
SIGTERM	termination request, sent to the program
SIGSEGV	invalid memory access (segmentation fault)
SIGINT	external interrupt, usually initiated by the user
SIGILL	invalid program image, such as invalid instruction
SIGABRT	abnormal termination condition, as is e.g. initiated by std::abort()
SIGFPE	erroneous arithmetic operation such as divide by zero
*/

std::atomic<bool> ServerApplication::gRunning = true;

void signalHandler(int signum) {
	ServerApplication::terminate();
}

ServerApplication::ServerApplication()
{
	// make sure loop will canceld if sigint signal was send from os
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGINT, signalHandler);	
	std::signal(SIGABRT, signalHandler);
	std::signal(SIGFPE, signalHandler);
}

void ServerApplication::run()
{
	init();
	gRunning = true;
	while (gRunning) {

	}
	exit();
}