#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "Automata.h"

#ifdef _WIN32
#define CLEAR "cls"
#else //For linux
#define CLEAR "clear"
#endif

int menu(std::vector<std::string> options) {
	
	int curr_option = 0;

	system(CLEAR);

	while (true) {
		std::cout << "FUMAC 2016/2017" << std::endl << "by Jose Silva (1130352) & Daniel Freire (1130858)\n\n\n";

		for (int i = 0; i != options.size(); i++) {
			std::cout << i+1 << ") " << options.at(i) << std::endl;
		}

		std::cout << std::endl << "Choose option: ";
		std::cin >> curr_option;
		std::cin.get();

		if (std::cin.fail() || curr_option < 1 || curr_option > options.size()) {
			std::cin.clear();
			std::cin.ignore(256,'\n');
			system(CLEAR);
			continue;
		}

		curr_option--;

		system(CLEAR);

		return curr_option;
	}
	
}

int main() {

	Automata my_automata;
	std::stringstream consoleOutput;
	std::string path, filename;

	while (true) {
		
		int option = menu({
			"Load Automata From File",
			"Remove non-accessible states",
			"Remove non-coaccessible states",
			"Trim",
			"Convert to DFA",
			"Check automata type",
			"Print Automata",
			"Save Automata to File",
			"Exit"
		});

		switch (option) {
		case 0:
			std::cout << "Enter file path to load (use full path or filename for relative path):" << std::endl;
			std::cin >> path;
			std::cin.get();
			my_automata.loadFromFile(path, std::cout);
			std::cin.get();
			break;

		case 1:
			my_automata.removeNonAccessibleStates(std::cout);
			std::cin.get();
			break;
		
		case 2:
			my_automata.removeNonCoaccessibleStates(std::cout);
			std::cin.get();
			break;

		case 3:
			my_automata.trim(std::cout);
			std::cin.get();
			break;

		case 4:
			my_automata.toDFA(std::cout);
			std::cin.get();
			break; 

		case 5:
			my_automata.isDFA(std::cout);
			
			std::cin.get();
			break;
		
		case 6:
			my_automata.printAutomataInfo(std::cout);
			std::cin.get();
			break;

		case 7:
			if (my_automata.automataHasData(std::cout) == false) {
				std::cin.get();
				break;
			}
			std::cout << "Enter folder to save to (full path or relative path):" << std::endl;
			std::cin >> path;
			std::cin.get();
			std::cout << "Enter file name (.aut will be added to filename)" << std::endl;
			std::cin >> filename;
			std::cin.get();
			my_automata.saveToFile(path + "//" + filename + ".aut", std::cout);
			std::cin.get();
			break;

		case 8:
			return 0;
			break;

		default:
			break;
		}
	}

	




	/*
	//my_automata.loadFromFile("C:/Users/Utilizador/Source/Repos/Example.aut", consoleOutput);
	my_automata.loadFromFile("C:/Users/Chi/Source/Repos/Example.aut", std::cout);

	//my_automata.printAutomataInfo(consoleOutput);

	//my_automata.trim(std::cout);

	my_automata.printAutomataInfo(consoleOutput);
	
	my_automata.toDFA(consoleOutput);

	my_automata.printAutomataInfo(consoleOutput);

	std::cout << consoleOutput.str();

	*/
	return 0;
}