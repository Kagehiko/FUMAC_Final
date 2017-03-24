#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "Automata.h"

int main() {

	Automata my_automata;
	std::stringstream consoleOutput;

	//my_automata.loadFromFile("C:/Users/Utilizador/Source/Repos/FUMAC_Final/FUMAC_Final/Debug/Example.aut", consoleOutput);
	my_automata.loadFromFile("C:/Users/Chi/Source/Repos/FUMAC_Final/FUMAC_Final/x64/Debug/Example.aut", std::cout);

	my_automata.printAutomataInfo(consoleOutput);

	my_automata.removeNonAccessibleStates(consoleOutput);

	my_automata.printAutomataInfo(consoleOutput);

	std::cout << consoleOutput.str();

	std::cin.get();
	return 0;
}





//Test main function for testing maps with std::pair keys and std::vector values
/*
int main() {

	// <State,Event> , State
	std::map<std::pair<int,char>, std::vector<int> > test;

	//Create transition from state 1 to 2 and 3 using event "a"
	//f(1,a)={2,3}
	test[{1,'a'}].push_back(2);
	test[{1, 'a'}].push_back(3);

	std::cout << "f(1,a)=";

	//Print what is on the transition vector
	for (std::vector<int>::iterator it = test[{1, 'a'}].begin(); it != test[{1, 'a'}].end(); ++it) {
		std::cout << *it << " ";
	}
	
	std::cout << std::endl;

	//Check if any transitions from 1 with 'a' exist. Should return 1
	std::cout << test.count({1,'a'}) << std::endl;

	//Deletes all transitions from 1 using 'a'
	test.erase(test.find({1, 'a'}));

	//Check again if any transitions from 1 with 'a' exist. Should return 0
	std::cout << test.count({1,'a'}) << std::endl;

	std::cin.get();

	return 0;
}
*/