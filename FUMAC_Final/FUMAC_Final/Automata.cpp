#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "Automata.h"

enum Parser_state_enum {
	eNOSTATE,
	eSTATES,
	eEVENTS,
	eTRANSITIONS,
	eINITIAL,
	eAFTER_INITIAL,
	eMARKED,
	eAFTER_MARKED,
};

bool Automata::loadFromFile(std::string path) {
	Parser_state_enum parser_state = eNOSTATE;
	std::string line;
	std::ifstream file(path);

	std::cout << "Opening file in:'" << path << "'" << std::endl;

	if (!file.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return false;
	}

	while (std::getline(file, line)){
		//Remove blank spaces and the \r character if it exists
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

		//Ignore empty lines
		if (line == "") {
			continue;
		}

		//Check if parser needs to change state
		if (line == "STATES") {
			parser_state = eSTATES;
			continue;
		}

		if (line == "EVENTS") {
			parser_state = eEVENTS;
			continue;
		}

		if (line == "TRANSITIONS") {
			parser_state = eTRANSITIONS;
			continue;
		}

		if (line == "INITIAL") {
			parser_state = eINITIAL;
			continue;
		}

		if (line == "MARKED") {
			parser_state = eMARKED;
			continue;
		}

		//If it got here, the parser should now be reading a line after a label
		switch(parser_state) {
		case eSTATES:
			state_names.push_back(line);
			break;

		case eEVENTS:
			events.push_back(line);
			break;

		case eTRANSITIONS:
			break;

		case eNOSTATE:
			//This means there was trash befor the first label. Just ignore.
			break;

		default:
			return false;
			break;
		}

		std::cout << line << std::endl;
	}

	file.close();
	return true;
}