#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
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

//Deletes all automata data
void Automata::clearAutomata() {
	*this = Automata();
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

//Parser for automata loading. Allows for non-compliant state and event names, and can parse from a file or from a stringstream
bool Automata::parseStream(std::istream& input_stream, std::ostream& output_stream) {

	Parser_state_enum parser_state = eNOSTATE;
	std::string line; //String with current line being read by the parser
	uint64_t line_number = 0; //Used for indicating faulty lines in the input stream
	bool got_initial_state = false, got_marker_state = false; //Used to check if the parser has read one initial and at least one marked state
	std::vector<std::string> transition_str_vector; //Used to separate the state;event;state into various strings when reading transitions

	while (std::getline(input_stream, line)) {

		line_number++;

		//Remove all blank spaces and \r characters if they exist
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

		//Ignore empty lines and throw warning
		if (line == "") {
			output_stream << "Warning: line " << line_number << " is empty" << std::endl;
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
		switch (parser_state) {

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
				output_stream << "Error: Invalid transition format (line " << line_number << ")" << std::endl;
			}

			//Check if first substring matches a known state
			if (!(doesStringExistInVector(state_names, transition_str_vector.at(0), &first_state))) {
				output_stream << "State " << transition_str_vector.at(0) << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}

			//Check if second substring matches a known event
			if (!(doesStringExistInVector(events, transition_str_vector.at(1), &event_pos))) {
				output_stream << "Event " << transition_str_vector.at(1) << " does not match any known events (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}

			//Check if third substring matches a known state
			if (!(doesStringExistInVector(state_names, transition_str_vector.at(2), &second_state))) {
				output_stream << "State " << transition_str_vector.at(2) << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
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
				output_stream << "Error: Initial state " << line << " does not match any known states (line " << line_number << ")" << std::endl;

				clearAutomata();
				return false;
			}
			break;

		case eMARKED:
			//Keep "pos" scope inside these curly brackets
		{
			int pos = 0;
			if (doesStringExistInVector(state_names, line, &pos)) {
				marked_states.push_back(pos);
				got_marker_state = true;
			} else {
				output_stream << "Error: Marked state " << line << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}
		}
		break;

		case eNOSTATE:
			//This means there was trash befor the first label. Just ignore and throw a warning.
			output_stream << "Warning: invalid line before first label (line " << line_number << ")" << std::endl;
			break;

		case eAFTER_INITIAL:
			output_stream << "Error: more than 1 initial state (line " << line_number << ")" << std::endl;
			clearAutomata();
			return false;
			break;

		default:
			output_stream << "Parser state: " << parser_state << std::endl;
			output_stream << "Error: could not parse " << line << " at line " << line_number << std::endl;
			clearAutomata();
			return false;
			break;
		}
	}

	if (got_initial_state == false) {
		output_stream << "Error: No initial state found" << std::endl;
		clearAutomata();
		return false;
	}

	if (got_marker_state == false) {
		output_stream << "Error: No marked states found" << std::endl;
		clearAutomata();
		return false;
	}

	//Check if at least one marked state is accessible
	std::vector<bool> accessible_states(state_names.size(),false);
	bool one_marked_state_is_accessible = false;

	accessible_states = goThroughAccessibleStates(accessible_states,initial_state);

	for (int i = 0; i != marked_states.size(); i++) {
		if (accessible_states.at(marked_states.at(i)) == true) {
			one_marked_state_is_accessible = true;
			break;
		}
	}

	if (one_marked_state_is_accessible == false) {
		output_stream << "Error: At least one marked state must be accessible" << std::endl;
		clearAutomata();
		return false;
	}

	output_stream << "Parse successful" << std::endl;
	automata_has_data = true;

	return true;
}

//Loads file in a given path and calls the parser
bool Automata::loadFromFile(std::string path, std::ostream& stream) {
	std::ifstream file(path);
	
	stream << "Opening file in:'" << path << "'" << std::endl;

	if (!file.is_open()) {
		stream << "Failed to open file" << std::endl;
		return false; //Note: no need to clean Automata data because there was no data inserted into this Automata
	}

	parseStream(file, stream);

	file.close();
	return true;
}

//Prints the current automata to the output stream
void Automata::printAutomataInfo(std::ostream& stream) {

	bool flag = false;

	if (state_names.size() == 0) {
		stream << "No data has been loaded" << std::endl;
		return;
	}

	stream << std::endl << std::endl << "G=(X,E,T,f,X0,Xm)" << std::endl;

	//States
	stream << "X={";
	for (std::vector<std::string>::iterator it = state_names.begin(); it != state_names.end(); ++it) {
		if (flag == true) {
			stream << ",";
		}
		stream << *it;
		flag = true;
	}

	stream << "}" << std::endl;
	flag = false;

	//Events
	stream << "E={";
	for (std::vector<std::string>::iterator it = events.begin(); it != events.end(); ++it) {
		if (flag == true) {
			stream << ",";
		}
		stream << *it;
		flag = true;
	}

	stream << "}" << std::endl;
	flag = false;

	//Active events for each state
	for (int i = 0; i != state_names.size(); i++) {
		flag = false;
		stream << "T(" << state_names.at(i) << ")={";
		for (int k = 0; k != events.size(); k++) {
			if (transitions.count({ i,events.at(k) }) == 1) {
				if (flag == true) {
					stream << ",";
				}
				stream << events.at(k);
				flag = true;
			}
		}
		stream << "}" << std::endl;
	}

	//Transitions
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

	flag = false;

	//Initial state
	stream << "X0=" << state_names.at(initial_state) << std::endl;

	//Marked states
	stream << "Xm={";
	for (std::vector<int>::iterator it = marked_states.begin(); it != marked_states.end(); ++it) {
		if (flag == true) {
			stream << ",";
		}
		stream << state_names.at(*it);
		flag = true;
	}
	stream << "}" << std::endl << std::endl << std::endl;
}

//Returns a boolean vector where all accessible states are marked as true.
std::vector<bool> Automata::goThroughAccessibleStates(std::vector<bool> accessible_states, int state) {

	//If a function call was made for this state, then the state is accessible
	accessible_states.at(state) = true;

	//Iterate through all events
	for (int i = 0; i != events.size(); i++) {

		//If a transition for this event exists, let's go through all possible states where this transition leads to
		if (transitions.count({ state,events.at(i) })) {
			
			for (std::vector<int>::iterator it = transitions[{state, events.at(i)}].begin(); it != transitions[{state, events.at(i)}].end(); ++it) {
				
				//Check if we already went into this state. If so, ignore this state to avoid infinite loops, else go through all of the acessible states of the new state 
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

//Deletes all states marked as "false" in the "states_to_keep" argument. The second argument returns parser messages
bool Automata::keepStates(std::vector<bool> states_to_keep, std::ostream& stream) {

	//Create a stringstream and copy only the relevant states to it
	std::stringstream newAutomataInfo;

	newAutomataInfo << "STATES\r\n";

	//Copy only the states marked as true
	for (int i = 0; i != state_names.size(); i++) {
		if (states_to_keep.at(i) == true) {
			newAutomataInfo << state_names.at(i) << "\r\n";
		}
	}

	//Copy all events
	newAutomataInfo << "EVENTS\r\n";
	for (int i = 0; i != events.size(); i++) {
		newAutomataInfo << events.at(i) << "\r\n";
	}

	//Copy all transitions from states marked as false
	newAutomataInfo << "TRANSITIONS\r\n";

	for (int i = 0; i != state_names.size(); i++) {
		if (states_to_keep.at(i) == true) {
			//Go through all events and check if a transition exists
			for (int k = 0; k != events.size(); k++) {
				if (transitions.count({ i,events.at(k) }) == 1) {
					//Go through all states to where this the "i state" and "k event" lead to
					for (std::vector<int>::iterator it = transitions[{i, events.at(k)}].begin(); it != transitions[{i, events.at(k)}].end(); ++it) {
						//If the state where this transition leads to is not supposed to be removed, then write it to the stream
						if (states_to_keep.at(*it) == true) {
							newAutomataInfo << state_names.at(i) << ";" << events.at(k) << ";" << state_names.at(*it) << "\r\n";
						}
					}
				}
			}
		}
	}

	newAutomataInfo << "INITIAL\r\n";

	newAutomataInfo << state_names.at(initial_state) << "\r\n";

	newAutomataInfo << "MARKED\r\n";

	for (int i = 0; i != marked_states.size(); i++) {
		if (states_to_keep.at(marked_states.at(i)) == true) {
			newAutomataInfo << state_names.at(marked_states.at(i)) << "\r\n";
		}
	}

	clearAutomata();

	stream << "Feeding new automata information to parser..." << std::endl;
	return parseStream(newAutomataInfo, stream);

}

//Removes all non-accessible states from automata
void Automata::removeNonAccessibleStates(std::ostream& stream) {

	if (automata_has_data == false) {
		return;
	}

	//Each position corresponds to the state index, and a 1 indicates that the state is accessible.
	std::vector<bool> accessible_states(state_names.size(), false);

	accessible_states = goThroughAccessibleStates(accessible_states, initial_state);

	stream << "Accessible states: {";

	bool hasOneState = false;

	for (int i = 0; i != accessible_states.size() ; i++ ) {
		if (accessible_states.at(i) == true) {
			if (hasOneState == true) {
				stream << ",";
			}
			stream << state_names.at(i);
			hasOneState = true;
		}
	}

	stream << "}" << std::endl;
	stream << "Deleting all non-accessible states..." << std::endl;

	keepStates(accessible_states, stream);
}

bool Automata::CheckCoAc(int state,std::vector<bool>& coaccessible_states, std::vector<bool>& result_is_known, std::vector<int> path) {

	//Found this state in the marked_states vector, therefore this state is marked
	//Note: yes, you could make all marked states in the "result_is_known" and "coaccessible_states" vectors true
	//but this check allows for calling this recursive function without having to set up the vectors
	if (std::find(marked_states.begin(), marked_states.end(), state) != marked_states.end()) {
		std::cout << state_names.at(state) << " is marked!" << std::endl;
		coaccessible_states.at(state) = true;
		result_is_known.at(state) = true;
		return true;
	}

	std::cout << "Checking state " << state_names.at(state) << std::endl;

	//If this state needs testing, then we this state to our "path"
	//This will prevent loops from happening, since we will not recursively call this function
	//on a state that belongs to the path that we are taking.
	path.push_back(state);
	
	//Go through all events for this state and follow the path to know if it leads to a coaccessible state
	for (int i = 0; i != events.size(); i++) {
		if (transitions.count({state,events.at(i)}) == 1) {
			std::cout << "There are transitions for " << state_names.at(state) << " for the event " << events.at(i) << std::endl;
			//If there are any transitions for this state, then let's go through all of them
			for (std::vector<int>::iterator it = transitions[{state, events.at(i)}].begin(); it != transitions[{state, events.at(i)}].end(); ++it) {
				std::cout << "Checking transition to " << state_names.at(*it) << std::endl;
				//First let's check if we know anything about the next state
				if (result_is_known.at(*it) == true) {
					std::cout << "I know the result for " << state_names.at(*it) <<": ";
					//If the state where we want to go to is coaccessible, then we known that this one is too and we don't need any further checks
					if (coaccessible_states.at(*it) == true) {
						std::cout << "it is coaccessible! " << state_names.at(state) << " is coaccessible." << std::endl;
						coaccessible_states.at(state) = true;
						result_is_known.at(state) = true;
						return true;
					} else {
						//If we already known that the next state leads nowhere, then we continue searching the transitions that this event leads to
						std::cout << "it is NOT coaccessible!" << std::endl;
						continue;
					}
				} else if(std::find(path.begin(), path.end(), *it) != path.end()){
					//If we already went into this "*it" state and we have no information on it, we've hit a loop and can't do anything about it
					std::cout << state_names.at(*it) << " belongs to the path I've taken. I'll ignore this state." << std::endl;
					continue;
				} else {
					std::cout << "I have no info on " << state_names.at(*it) << ". Let's use recursion to see where it leads..." << std::endl;
					//If we don't have any information on the next state, then get that information
					if (CheckCoAc(*it, coaccessible_states, result_is_known, path) ) {
						std::cout << state_names.at(*it) << " lead me to a coaccessible state." << state_names.at(state) << " is coaccessible!" << std::endl;
						//It seems that this "*it" state is coaccessible, so this one also is
						coaccessible_states.at(state) = true;
						result_is_known.at(state) = true;
						return true;
					}
					std::cout << state_names.at(*it) << " lead me to a non-coaccessible state. Let's continue checking." << std::endl;
				}
			}
		}
	}

	std::cout << "No " << state_names.at(state) << " transition leads to a coaccessible state. " << state_names.at(state) << "is not coaccessible." << std::endl;
	result_is_known.at(state) = true;
	//If the code gets here, we went through all possible transitions for this state and no one lead us to a coaccessible state, so this state is not coaccessible
	return false;
}

//Removes all non-coaccessible states from automata
void Automata::removeNonCoAccessibleStates(std::ostream& stream) {

	std::vector<bool> coaccessible_states(state_names.size(), false);
	std::vector<bool> result_is_known(state_names.size(), false);
	std::vector<int> path;

	if (automata_has_data == false) {
		return;
	}

	//Go through all states
	for (int i = 0; i != state_names.size(); i++) {
		//If we already know if state "i" is coaccessible, then skip it
		if (result_is_known.at(i) != true) {
			CheckCoAc(i,coaccessible_states, result_is_known, path);
		}
	}


	stream << std::endl << "Co-accessible states: {";

	bool hasOneState = false;

	for (int i = 0; i != coaccessible_states.size(); i++) {
		if (coaccessible_states.at(i) == true) {
			if (hasOneState == true) {
				stream << ",";
			}
			stream << state_names.at(i);
			hasOneState = true;
		}
	}

	stream << "}" << std::endl;
	stream << "Deleting all non-co-accessible states..." << std::endl;

	keepStates(coaccessible_states, stream);
}