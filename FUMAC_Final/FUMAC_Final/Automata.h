#pragma once

#include <string>
#include <vector>
#include <map>

class Automata {
	public:
		bool loadFromFile(std::string path, std::ostream& stream);
		void printAutomataInfo(std::ostream& stream);
		void removeNonAccessibleStates(std::ostream& stream);
		void removeNonCoaccessibleStates(std::ostream& stream);
		void trim(std::ostream& stream);

	private:
		bool automataHasData(std::ostream& stream);
		bool CheckCoAc(int state, std::vector<bool>& coaccessible_states, std::vector<bool>& result_is_known, std::vector<int> path);
		void clearAutomata();
		bool keepStates(std::vector<bool> states_to_keep, std::ostream& stream);
		bool parseStream(std::istream& input_stream, std::ostream& output_stream);
		std::vector<bool> goThroughAccessibleStates(std::vector<bool> accessible_states, int state);
		
		//States are stored as ints that represent their index in the state_names vector
		std::vector<std::string> state_names;
		int initial_state;
		std::vector<int> marked_states;
		std::vector<std::string> events;
		std::map<std::pair<int, std::string>, std::vector<int> > transitions;
};