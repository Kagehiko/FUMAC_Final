#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include "Automata.h"

enum Parser_state_enum {
	eNOSTATE,
	eSTATES,
	eEVENTS,
	eTRANSITIONS,
	eINITIAL,
	eAFTER_INITIAL,
	eMARKED,
};

//Returns a vector of the strings using a marker
std::vector<std::string> split(const std::string &str, char marker) {
	std::vector<std::string> output_vect;
	std::stringstream stream;
	stream.str(str);
	std::string substring;

	while (std::getline(stream, substring, marker)) {
		output_vect.push_back(substring);
	}
	return output_vect;
}

//Parser for automata loading. Allows for non-compliant state and event names
bool Automata::loadFromFile(std::string path) {
	Parser_state_enum parser_state = eNOSTATE;
	std::string line;
	std::ifstream file(path);
	uint64_t line_number=0;

	std::cout << "Opening file in:'" << path << "'" << std::endl;

	if (!file.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return false;
	}

	std::vector<std::string> test;

	while (std::getline(file, line)){

		//Increase line number. Only used for indicating a faulty line in the file 
		line_number++;

		//Remove blank spaces and the \r character if it exists
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

		//Ignore empty lines
		if (line == "") {
			std::cout << "Warning: line " << line_number << " in file is empty" << std::endl;
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

		//At this point the parser should be reading a line after a label
		switch(parser_state) {
		case eSTATES:
			state_names.push_back(line);
			break;

		case eEVENTS:
			events.push_back(line);
			break;

		case eTRANSITIONS:
			/*
			test = split(line, ';');
			std::cout << "test[0] = " << test[0] << std::endl;
			std::cout << "test[1] = " << test[1] << std::endl;
			std::cout << "test[2] = " << test[2] << std::endl;
			*/
			break;

		case eINITIAL:
			for (int i=0 ; i != state_names.size() ; i++) {
				if (state_names.at(i) == line) {
					break;
				}
				/*
				if () {

				}
				*/
			}
			parser_state = eAFTER_INITIAL;
			break;

		case eMARKED:
			break;

		case eNOSTATE:
			//This means there was trash befor the first label. Just ignore.
			std::cout << "Warning: file contains invalid line before first label (line " << line_number << ")" << std::endl;
			break;

		case eAFTER_INITIAL:
			std::cout << "Error: more than 1 initial state in file" << std::endl;
			return false;
			break;

		default:
			std::cout << "Parser state: " << parser_state << std::endl;
			std::cout << "Error: could not parse " << line << " at line " << line_number << std::endl;
			return false;
			break;
		}
	}

	std::cout << "States:" << std::endl;
	for (std::vector<std::string>::iterator it = state_names.begin(); it != state_names.end(); ++it) {
		std::cout << *it << std::endl;
	}

	std::cout << "Events:" << std::endl;
	for (std::vector<std::string>::iterator it = events.begin(); it != events.end(); ++it) {
		std::cout << *it << std::endl;
	}

	file.close();
	return true;
}