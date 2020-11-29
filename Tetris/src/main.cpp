#include "Engine.h"

#include <iostream>

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	try
	{
		Tetris::Engine engine({ 640, 480 });
		engine.run();
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}	

	return 0;
}