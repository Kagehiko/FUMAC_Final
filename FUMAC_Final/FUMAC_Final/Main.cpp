#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "Automata.h"

int main() {

	Automata my_automata;
	std::stringstream consoleOutput;

	//my_automata.loadFromFile("C:/Users/Utilizador/Source/Repos/Example.aut", consoleOutput);
	my_automata.loadFromFile("C:/Users/Chi/Source/Repos/Example.aut", std::cout);

	//my_automata.printAutomataInfo(consoleOutput);

	//my_automata.trim(std::cout);

	my_automata.printAutomataInfo(consoleOutput);

	std::cout << consoleOutput.str();

	my_automata.toDFA();

	std::cin.get();
	return 0;
}