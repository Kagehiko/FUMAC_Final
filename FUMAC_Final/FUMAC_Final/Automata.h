#pragma once

#include <string>
#include <vector>
#include <map>


class Automata {
	//Most functions have an optional "console_output" argument
	public:
		bool loadFromFile(std::string path, std::ostream& console_output = std::stringstream());
		void printAutomataInfo(std::ostream& console_output);
		void removeNonAccessibleStates(std::ostream& console_output = std::stringstream());
		void removeNonCoaccessibleStates(std::ostream& console_output = std::stringstream());
		void trim(std::ostream& console_output = std::stringstream());
		void clearAutomata(std::ostream& console_output = std::stringstream());

	private:
		bool automataHasData(std::ostream& console_output);
		bool parseStream(std::istream& input_stream, std::ostream& console_output = std::stringstream());
		void goThroughAccessibleStates(std::vector<bool>& accessible_states, int state);
		bool goThroughCoAcStates(int state, std::vector<bool>& coaccessible_states, std::vector<bool>& result_is_known, std::vector<int> path);
		bool keepStates(std::vector<bool> states_to_keep, std::ostream& console_output = std::stringstream());
		
		
		//States are stored as ints that represent their index in the state_names vector
		std::vector<std::string> state_names;
		int initial_state;
		std::vector<int> marked_states;
		std::vector<std::string> events;
		std::map<std::pair<int, std::string>, std::vector<int> > transitions;
};