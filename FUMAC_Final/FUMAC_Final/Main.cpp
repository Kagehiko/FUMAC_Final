#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "Automata.h"

int main() {

	Automata my_automata;
	std::stringstream consoleOutput;

	my_automata.loadFromFile("C:/Users/Utilizador/Source/Repos/Example.aut", consoleOutput);
	//my_automata.loadFromFile("C:/Users/Chi/Source/Repos/FUMAC_Final/FUMAC_Final/x64/Debug/Example.aut", std::cout);

	my_automata.trim(consoleOutput);

	my_automata.printAutomataInfo(consoleOutput);

	std::cout << consoleOutput.str();

	std::cin.get();
	return 0;
}