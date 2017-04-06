#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <vector>
#include "Automata.h"



//Helper functions

//Splits a string by the delimiter and returns a vector with the substrings
std::vector<std::string> split(const std::string &str, char delimiter) {
	std::vector<std::string> output_vect;
	std::stringstream stream;
	stream.str(str);
	std::string substring;

	//Get substring up to the delimiter and push it to the output vector
	while (std::getline(stream, substring, delimiter)) {
		output_vect.push_back(substring);
	}
	return output_vect;
}



//Returns true if a given string exists in a vector of strings and writes the first index where it was found to "pos"
bool doesStringExistInVector(std::vector<std::string> str_vector, std::string str, int* pos) {

	for (int i = 0; i != str_vector.size(); i++) {
		if (str_vector.at(i) == str) {
			*pos = i;
			return true;
		}
	}
	return false;
}



//Public methods

//Loads file in a given path and calls the parser
bool Automata::loadFromFile(std::string path, std::ostream& console_output) {
	std::ifstream file(path);

	console_output << "Opening file in:'" << path << "'" << std::endl;

	if (!file.is_open()) {
		console_output << "Failed to open file" << std::endl;
		return false;
	}

	parseStream(file, console_output);

	file.close();
	return true;
}



//Prints the current automata to the output stream
void Automata::printAutomataInfo(std::ostream& console_output) {

	if (automataHasData(console_output) == false) {
		return;
	}

	console_output << std::endl << std::endl << "G=(X,E,T,f,X0,Xm)" << std::endl;

	//States
	console_output << "X={" << state_names.at(0);
	for (std::vector<std::string>::iterator it = state_names.begin() + 1; it != state_names.end(); ++it) {
		console_output << "," << *it;
	}
	console_output << "}" << std::endl;

	//Events
	console_output << "E={" << events.at(0);
	for (std::vector<std::string>::iterator it = events.begin() + 1; it != events.end(); ++it) {
		console_output << "," << *it;
	}
	console_output << "}" << std::endl;

	//Active events per state

	//If "more_than_one" is true, than there is more than one active event per state 
	bool more_than_one = false; 

	//Go through all states
	for (int i = 0; i != state_names.size(); i++) {
		//Reset variable after each state
		more_than_one = false;
		console_output << "T(" << state_names.at(i) << ")={";
		//Go through all events and check active events
		for (int k = 0; k != events.size(); k++) {
			if (transitions.count({ i,events.at(k) }) == 1) {
				if (more_than_one == true) {
					console_output << ",";
				}
				console_output << events.at(k);
				more_than_one = true;
			}
		}
		console_output << "}" << std::endl;
	}

	//Transitions
	//Go through all states
	for (int i = 0; i != state_names.size(); i++) {
		//Go through all events for that state
		for (int k = 0; k != events.size(); k++) {
			//Check if a vector for this state and event exists
			if (transitions.count({ i,events.at(k) }) == 1) {
				//Get an iterator for the vector that has all the possible states for this "state i" and "event k" pair
				std::vector<int>::iterator it = transitions[{i, events.at(k)}].begin();

				console_output << "f(" << state_names.at(i) << "," << events.at(k) << ") = {" << state_names.at(*it);
				it++;
				//If there is more than 1 transition for this state and event, go through all of them
				while (it != transitions[{i, events.at(k)}].end()) {
					console_output << "," << state_names.at(*it);
					it++;
				}
				console_output << "}" << std::endl;
			}
		}
	}

	//Initial state
	console_output << "X0=" << state_names.at(initial_state) << std::endl;

	//Marked states
	console_output << "Xm={" << state_names.at(marked_states.at(0));
	for (std::vector<int>::iterator it = marked_states.begin() + 1; it != marked_states.end(); ++it) {
		console_output << "," << state_names.at(*it);
	}
	console_output << "}" << std::endl << std::endl << std::endl;
}



//Removes all non-accessible states from automata
void Automata::removeNonAccessibleStates(std::ostream& console_output) {

	if (automataHasData(console_output) == false) {
		return;
	}

	//Each position corresponds to the state index, and a 1 indicates that the state is accessible.
	std::vector<bool> accessible_states(state_names.size(), false);

	goThroughAccessibleStates(accessible_states, initial_state);

	console_output << "Accessible states: {";

	bool hasOneState = false;

	for (int i = 0; i != accessible_states.size(); i++) {
		if (accessible_states.at(i) == true) {
			if (hasOneState == true) {
				console_output << ",";
			}
			console_output << state_names.at(i);
			hasOneState = true;
		}
	}

	console_output << "}" << std::endl;
	console_output << "Deleting all non-accessible states..." << std::endl;

	keepStates(accessible_states, console_output);
}



//Removes all non-coaccessible states from automata
void Automata::removeNonCoaccessibleStates(std::ostream& console_output) {

	std::vector<bool> coaccessible_states(state_names.size(), false);
	std::vector<bool> result_is_known(state_names.size(), false);
	std::vector<int> path;

	if (automataHasData(console_output) == false) {
		return;
	}

	//Go through all states
	for (int i = 0; i != state_names.size(); i++) {
		//If we already know if state "i" is coaccessible, then skip it
		if (result_is_known.at(i) != true) {
			goThroughCoAcStates(i, coaccessible_states, result_is_known, path);
		}
	}

	console_output << std::endl << "Coaccessible states: {";

	bool hasOneState = false;

	for (int i = 0; i != coaccessible_states.size(); i++) {
		if (coaccessible_states.at(i) == true) {
			if (hasOneState == true) {
				console_output << ",";
			}
			console_output << state_names.at(i);
			hasOneState = true;
		}
	}

	console_output << "}" << std::endl;
	console_output << "Deleting all non-coaccessible states..." << std::endl;

	keepStates(coaccessible_states, console_output);
}



//Removes both non-accessible and non-coaccessible states
void Automata::trim(std::ostream& console_output) {
	this->removeNonAccessibleStates(console_output);
	this->removeNonCoaccessibleStates(console_output);
}



//Performs NFA to DFA conversion
void Automata::toDFA() {

	std::vector<int> NFA_initial;

	getEClosure(NFA_initial, initial_state);

	std::cout << "Eclosure of the initial state: " << state_names.at(NFA_initial.at(0));

	for (int i = 1; i != NFA_initial.size(); i++) {
		std::cout << "," <<state_names.at(NFA_initial.at(i));
	}

	std::cout << std::endl;
}



//Deletes all automata data
void Automata::clearAutomata(std::ostream& console_output) {
	*this = Automata();
	console_output << "Automata data deleted" << std::endl;
}



//Private methods

//Checks if there is any data loaded into the automata
bool Automata::automataHasData(std::ostream& console_output) {

	if (state_names.size() == 0) {
		console_output << "No data has been loaded" << std::endl;
		return false;
	}
	return true;
}



//Enumerator for the parser
enum Parser_state_enum {
	eNOSTATE,
	eSTATES,
	eEVENTS,
	eTRANSITIONS,
	eINITIAL,
	eAFTER_INITIAL,
	eMARKED,
};



//Parser for automata loading. Allows for non-compliant state and event names, and can parse from a file or from a stringstream
bool Automata::parseStream(std::istream& input_stream, std::ostream& console_output) {

	Parser_state_enum parser_state = eNOSTATE;
	std::string line; //String with current line being read by the parser
	uint64_t line_number = 0; //Used for indicating faulty lines in the input stream
	bool got_initial_state = false, got_marker_state = false; //Used to check if the parser has read one initial and at least one marked state
	std::vector<std::string> transition_str_vector; //Used to separate the state;event;state into various strings when reading transitions
	bool automata_has_epsilon_event = false; //Minor optimization: avoids having to find the epsilon event in the "events" vector at each transition

	while (std::getline(input_stream, line)) {

		line_number++;

		//Remove all blank spaces and \r characters if they exist
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

		//Ignore empty lines and throw warning
		if (line == "") {
			console_output << "Warning: line " << line_number << " is empty" << std::endl;
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
				console_output << "Error: Invalid transition format (line " << line_number << ")" << std::endl;
			}

			//Check if first substring matches a known state
			if (!(doesStringExistInVector(state_names, transition_str_vector.at(0), &first_state))) {
				console_output << "State " << transition_str_vector.at(0) << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}

			//Check if second substring matches a known event
			if (!(doesStringExistInVector(events, transition_str_vector.at(1), &event_pos))) {
				if (transition_str_vector.at(1) == "") {
					//Epsilon transition
					if (automata_has_epsilon_event == false) {
						automata_has_epsilon_event = true;
						events.push_back("");
					}
				}
				else {
					console_output << "Event " << transition_str_vector.at(1) << " does not match any known events (line " << line_number << ")" << std::endl;
					clearAutomata();
					return false;
				}

			}

			//Check if third substring matches a known state
			if (!(doesStringExistInVector(state_names, transition_str_vector.at(2), &second_state))) {
				console_output << "State " << transition_str_vector.at(2) << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}

			//Store information
			transitions[{first_state, transition_str_vector.at(1)}].push_back(second_state);

		}
		break;

		case eINITIAL:
			if (doesStringExistInVector(state_names, line, &initial_state)) {
				got_initial_state = true;
				parser_state = eAFTER_INITIAL;
			}
			else {
				console_output << "Error: Initial state " << line << " does not match any known states (line " << line_number << ")" << std::endl;

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
			}
			else {
				console_output << "Error: Marked state " << line << " does not match any known states (line " << line_number << ")" << std::endl;
				clearAutomata();
				return false;
			}
		}
		break;

		case eNOSTATE:
			//This means there was trash befor the first label. Just ignore and throw a warning.
			console_output << "Warning: invalid line before first label (line " << line_number << ")" << std::endl;
			break;

		case eAFTER_INITIAL:
			console_output << "Error: more than 1 initial state (line " << line_number << ")" << std::endl;
			clearAutomata();
			return false;
			break;

		default:
			console_output << "Parser state: " << parser_state << std::endl;
			console_output << "Error: could not parse " << line << " at line " << line_number << std::endl;
			clearAutomata();
			return false;
			break;
		}
	}

	if (got_initial_state == false) {
		console_output << "Error: No initial state found" << std::endl;
		clearAutomata();
		return false;
	}

	if (got_marker_state == false) {
		console_output << "Error: No marked states found" << std::endl;
		clearAutomata();
		return false;
	}

	//Check if at least one marked state is accessible
	std::vector<bool> accessible_states(state_names.size(), false);
	bool one_marked_state_is_accessible = false;

	goThroughAccessibleStates(accessible_states, initial_state);

	for (int i = 0; i != marked_states.size(); i++) {
		if (accessible_states.at(marked_states.at(i)) == true) {
			one_marked_state_is_accessible = true;
			break;
		}
	}

	if (one_marked_state_is_accessible == false) {
		console_output << "Error: At least one marked state must be accessible" << std::endl;
		clearAutomata();
		return false;
	}


	bool delete_event = true;
	//Delete unused events
	for (int i = 0; i != events.size(); i++) {
		//Reset variable for each run
		delete_event = true;
		//Go thorugh all states and see if the event is used
		for (int k = 0; k != state_names.size(); k++) {
			if (transitions.count({ k,events.at(i) }) == 1) {
				delete_event = false;
				break;
			}
		}
		if (delete_event == true) {
			console_output << "Deleting unused event " << events.at(i) << std::endl;
			events.erase(events.begin() + i);
		}
	}

	console_output << "Parse successful" << std::endl;

	return true;
}



//Will mark the states' index in a boolean vector as "true" if the state is accessible. 
//If not given the initial state's index, it will instead mark as "true" the states where the given state leads to
void Automata::goThroughAccessibleStates(std::vector<bool>& accessible_states, int state) {

	//If a function call was made for this state, then the state is accessible
	accessible_states.at(state) = true;

	//Iterate through all events
	for (int i = 0; i != events.size(); i++) {

		//If a transition for this event exists, let's go through all possible states where this transition leads to
		if (transitions.count({ state,events.at(i) }) == 1) {
			
			for (std::vector<int>::iterator it = transitions[{state, events.at(i)}].begin(); it != transitions[{state, events.at(i)}].end(); ++it) {
				
				//Check if we already went into this state. If so, ignore this state to avoid infinite loops, else go through all of the acessible states of the new state 
				if (accessible_states.at(*it) == true) {
					continue;
				} else {
					goThroughAccessibleStates(accessible_states, *it);
				}
			}
		}
	}
}



//Will mark the states' index in a boolean vector as "true" if the
bool Automata::goThroughCoAcStates(int state, std::vector<bool>& coaccessible_states, std::vector<bool>& result_is_known, std::vector<int> path) {

	//Check if this state is marked
	//Note: one could make all marked states in the "result_is_known" and "coaccessible_states" vectors true
	//before starting the recursive calls to this function
	if (std::find(marked_states.begin(), marked_states.end(), state) != marked_states.end()) {
		coaccessible_states.at(state) = true;
		result_is_known.at(state) = true;
		return true;
	}

	//If this state needs testing, it is added to the "path"
	//This will prevent loops from happening, since there will be no recursive calls on a state that belongs to the path
	path.push_back(state);

	//Go through all events for this state and follow the path to know if it leads to a coaccessible state
	for (int i = 0; i != events.size(); i++) {

		if (transitions.count({ state,events.at(i) }) == 1) {
			//If there are any transitions for this state, then let's go through all of them
			for (std::vector<int>::iterator it = transitions[{state, events.at(i)}].begin(); it != transitions[{state, events.at(i)}].end(); ++it) {

				//First let's check if we know anything about the next state
				if (result_is_known.at(*it) == true) {
					//If the state where we want to go to is coaccessible, then we known that this one is too and we don't need any further checks
					if (coaccessible_states.at(*it) == true) {
						coaccessible_states.at(state) = true;
						result_is_known.at(state) = true;
						return true;
					}
					else {
						//If we already known that the next state leads nowhere, then we continue searching the transitions that this event leads to
						continue;
					}
				}
				else if (std::find(path.begin(), path.end(), *it) != path.end()) {
					//If we already went into this "*it" state and we have no information on it, we've hit a loop and can't do anything about it
					continue;
				}
				else {
					//If we don't have any information on the next state, then get that information
					if (goThroughCoAcStates(*it, coaccessible_states, result_is_known, path)) {
						//It seems that this "*it" state is coaccessible, so this one also is coaccessible
						coaccessible_states.at(state) = true;
						result_is_known.at(state) = true;
						return true;
					}
				}
			}
		}
	}

	result_is_known.at(state) = true;
	//If the code gets here, we went through all possible transitions for this state and no one lead us to a coaccessible state, so this state is not coaccessible
	return false;
}



//Deletes all states marked as "false" in the "states_to_keep" vector.
bool Automata::keepStates(std::vector<bool> states_to_keep, std::ostream& console_output) {

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
		if (events.at(i) != "") {
			newAutomataInfo << events.at(i) << "\r\n";
		}
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

	console_output << "Feeding new automata information to parser..." << std::endl;
	return parseStream(newAutomataInfo, console_output);

}



//Returns a vector with the E-closure of the given state
void Automata::getEClosure(std::vector<int>& e_closure_vect,int state) {

	e_closure_vect.push_back(state);

	if (transitions.count({state,""}) == 1) {
		//Go through all states to which the epsilon transition leads to
		for (std::vector<int>::iterator it = transitions[{state,""}].begin(); it != transitions[{state,""}].end(); ++it) {
			//Get the E-closure of state *it only if it is not found in the e_closure_vect.
			if (std::find(e_closure_vect.begin(), e_closure_vect.end(), *it) == e_closure_vect.end()) {
				getEClosure(e_closure_vect, *it);
			}
		}
	}

}