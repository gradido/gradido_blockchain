#include "gradido_blockchain/Application.h"

#include "loguru/loguru.hpp"

#include <csignal>
#include <cstdio>

/*
* Constant	Explanation
SIGTERM	termination request, sent to the program
SIGSEGV	invalid memory access (segmentation fault)
SIGINT	external interrupt, usually initiated by the user
SIGILL	invalid program image, such as invalid instruction
SIGABRT	abnormal termination condition, as is e.g. initiated by std::abort()
SIGFPE	erroneous arithmetic operation such as divide by zero
*/

std::atomic<bool> Application::gRunning = true;

void signalHandler(int signum) {
	switch (signum) {
	case SIGTERM: LOG_F(INFO, "exit on termination request"); break;
	case SIGSEGV: printf("exit because of segmentation fault\n"); break;
	case SIGINT: LOG_F(INFO, "exit on external interrupt like STRG + C"); break;
	case SIGILL: printf("exit because invalid program image\n"); break;
	case SIGABRT: LOG_F(ERROR, "abnormal termination condition, as is e.g. initiated by std::abort()"); break;
	case SIGFPE: LOG_F(ERROR, "erroneous arithmetic operation such as divide by zero"); break;
	}
	Application::terminate();
}

Application::Application()
{
	// make sure loop will canceled if sigint signal was send from os
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGSEGV, signalHandler);
	std::signal(SIGINT, signalHandler);	
	std::signal(SIGILL, signalHandler);
	std::signal(SIGABRT, signalHandler);
	std::signal(SIGFPE, signalHandler);
}

Application::~Application()
{

}

void Application::run()
{
	if (!init()) {
		exit();
		return;
	}
	gRunning = true;
	while (gRunning) {

	}
	exit();
}