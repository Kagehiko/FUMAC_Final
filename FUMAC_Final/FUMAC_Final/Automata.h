#pragma once

#include <string>
#include <vector>
#include <map>

class Automata {
	public:
		bool loadFromFile(std::string path, std::ostream& stream);
		void printAutomataInfo(std::ostream& stream);
		void removeNonAccessibleStates(std::ostream& stream);

	private:
		void clearAutomata();
		bool keepStates(std::vector<bool> states_to_keep, std::ostream& stream);
		bool parseStream(std::istream& input_stream, std::ostream& output_stream);
		std::vector<bool> goThroughAccessibleStates(std::vector<bool> accessible_states, int state);
		std::vector<std::string> state_names;
		int initial_state;
		std::vector<int> marked_states;
		std::vector<std::string> events;
		std::map<std::pair<int, std::string>, std::vector<int> > transitions;
};