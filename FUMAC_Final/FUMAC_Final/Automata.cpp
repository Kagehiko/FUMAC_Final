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
	
	//Get substring up to the marker from the stream and push it to the output vector
	while (std::getline(stream, substring, marker)) {
		output_vect.push_back(substring);
	}
	return output_vect;
}

//Returns true if a state exists and puts in "pos" the position of the state in the state_names vector
bool doesStringExistInVector(std::vector<std::string> str_vector, std::string str, int* pos) {
	int i;

	for (i = 0; i != str_vector.size(); i++) {
		if (str_vector.at(i) == str) {
			*pos = i;
			return true;
		}
	}
	return false;
}

//Parser for automata loading. Allows for non-compliant state and event names
bool Automata::loadFromFile(std::string path, std::ostream& stream) {
	Parser_state_enum parser_state = eNOSTATE;
	std::ifstream file(path);
	std::string line; //String with current line being read by the parser
	uint64_t line_number=0; //Used for indicating faulty lines in the file
	bool got_initial_state = false, got_marker_state = false; //Used to check if the parser has read one initial and at least one marked state
	std::vector<std::string> transition_str_vector; //Used to separate the state;event;state into various strings when reading transitions

	stream << "Opening file in:'" << path << "'" << std::endl;

	if (!file.is_open()) {
		stream << "Failed to open file" << std::endl;
		return false; //Note: no need to clean Automata data because there was no data inserted into this Automata
	}

	while (std::getline(file, line)){

		line_number++;

		//Remove all blank spaces and \r characters if they exist
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

		//Ignore empty lines and throw warning
		if (line == "") {
			stream << "Warning: line " << line_number << " in file is empty" << std::endl;
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

		//At this point the parser should be reading a line after a known label, or just trash before the first label (eNOSTATE)
		switch(parser_state) {

		case eSTATES:
			state_names.push_back(line);
			break;

		case eEVENTS:
			events.push_back(line);
			break;

		case eTRANSITIONS:
			{
				int first_state, event_pos, second_state;
				std::string event;

				transition_str_vector = split(line, ';');
				if (transition_str_vector.size() != 3) {
					stream << "Error: Invalid transition format (line " << line_number << ")" << std::endl;
				}

				//Check if first substring matches a known state
				if ( !(doesStringExistInVector(state_names, transition_str_vector.at(0), &first_state)) ) {
					stream << "State " << transition_str_vector.at(0) << " does not match any known states (line " << line_number << ")" << std::endl;
					//Delete current automata data and create new one. This resets all data and ensures that the already loaded data cannot be used.
					*this = Automata();
					return false;
				}

				//Check if second substring matches a known event
				if (!(doesStringExistInVector(events, transition_str_vector.at(1), &event_pos))) {
					stream << "Event " << transition_str_vector.at(1) << " does not match any known events (line " << line_number << ")" << std::endl;
					*this = Automata();
					return false;
				}

				//Check if third substring matches a known state
				if ( !(doesStringExistInVector(state_names, transition_str_vector.at(2), &second_state)) ) {
					stream << "State " << transition_str_vector.at(2) << " does not match any known states (line " << line_number << ")" << std::endl;
					*this = Automata();
					return false;
				}

				transitions[{first_state, transition_str_vector.at(1)}].push_back(second_state);

			}
			break;

		case eINITIAL:
			if (doesStringExistInVector(state_names, line, &initial_state)) {
				got_initial_state = true;
				parser_state = eAFTER_INITIAL;
			} else {
				stream << "Error: Initial state " << line << " does not match any known states (line " << line_number << ")" << std::endl;
				
				*this = Automata();
				return false;
			}
			break;

		case eMARKED:
			//Keep "pos" scope inside these curly brackets
			{
				int pos=0;
				if (doesStringExistInVector(state_names, line, &pos)) {
					marked_states.push_back(pos);
					got_marker_state = true;
				} else {
					stream << "Error: Marked state " << line << " does not match any known states (line " << line_number << ")" << std::endl;
					*this = Automata();
					return false;
				}
			}
			break;

		case eNOSTATE:
			//This means there was trash befor the first label. Just ignore and throw a warning.
			stream << "Warning: file contains invalid line before first label (line " << line_number << ")" << std::endl;
			break;

		case eAFTER_INITIAL:
			stream << "Error: more than 1 initial state in file (line " << line_number << ")" << std::endl;
			*this = Automata();
			return false;
			break;

		default:
			stream << "Parser state: " << parser_state << std::endl;
			stream << "Error: could not parse " << line << " at line " << line_number << std::endl;
			*this = Automata();
			return false;
			break;
		}
	}

	if (got_initial_state == false) {
		stream << "Error: No initial state found in file" << std::endl;
		*this = Automata();
		return false;
	}

	if (got_marker_state == false) {
		stream << "Error: No marked states found in file" << std::endl;
		*this = Automata();
		return false;
	}

	file.close();
	return true;
}

void Automata::printAutomataInfo(std::ostream& stream) {

	if (state_names.size() == 0) {
		stream << "No data has been loaded" << std::endl;
		return;
	}

	stream << "States:" << std::endl;
	for (std::vector<std::string>::iterator it = state_names.begin(); it != state_names.end(); ++it) {
		stream << *it << std::endl;
	}

	stream << "Events:" << std::endl;
	for (std::vector<std::string>::iterator it = events.begin(); it != events.end(); ++it) {
		stream << *it << std::endl;
	}

	stream << "Transitions:" << std::endl;
	//Go through all states
	for (int i = 0; i != state_names.size(); i++) {
		//Go through all events for that state
		for (int k = 0; k != events.size(); k++) {
			//Check if a vector for this state and event exists
			if (transitions.count({ i,events.at(k) })) {
				//Get an iterator for the vector that has all the possible states for this "state i" and "event k" pair
				std::vector<int>::iterator it = transitions[{i, events.at(k)}].begin();

				stream << "f(" << state_names.at(i) << "," << events.at(k) << ") = {" << state_names.at(*it);
				it++;
				//If there is more than 1 transition for this state and event, go through all of them
				while(it != transitions[{i, events.at(k)}].end()){
					stream << "," << state_names.at(*it);
					it++;
				}

				stream << "}" << std::endl;
			
			}
		}
	}

	stream << "Initial state: " << std::endl << state_names.at(initial_state) << std::endl;

	stream << "Marked states:" << std::endl;
	for (std::vector<int>::iterator it = marked_states.begin(); it != marked_states.end(); ++it) {
		stream << state_names.at(*it) << std::endl;
	}

}

std::vector<bool> Automata::goThroughAccessibleStates(std::vector<bool> accessible_states, int state) {

	//If a function call was made for this state, then the state is accessible
	accessible_states.at(state) = true;

	for (int i = 0; i != events.size(); i++) {
		if (transitions.count({ state,events.at(i) })) {
			//If a transition for this event exists, do recursive calls for each state that the event leads to
			for (std::vector<int>::iterator it = transitions[{state, events.at(i)}].begin(); it != transitions[{state, events.at(i)}].end(); ++it) {
				
				//Check if we already went into this state
				if (accessible_states.at(*it) == true) {
					continue;
				} else {
					accessible_states = goThroughAccessibleStates(accessible_states, *it);
				}
			}
		}
	}

	return accessible_states;
}

//WIP
void Automata::removeNonAccessibleStates() {

	//Each position corresponds to the state index, and a 1 indicates that the state is accessible.
	std::vector<bool> accessible_states;

	for (int i = 0; i != state_names.size(); i++) {
		accessible_states.push_back(false);
	}

	accessible_states = goThroughAccessibleStates(accessible_states, initial_state);

	std::cout << "Accessible states:" << std::endl;

	for (int i = 0; i != accessible_states.size() ; i++ ) {
		if (accessible_states.at(i) == true) {
			std::cout << state_names.at(i) << std::endl;
		}
	}

}