#pragma once

#include <string>
#include <vector>
#include <map>

class Automata {
	public:
		bool loadFromFile(std::string path, std::ostream& stream);
		void printAutomataInfo(std::ostream& stream);
	private:
		bool doesStringExistInVector(std::vector<std::string> str_vector, std::string str, int* pos);
		std::vector<std::string> state_names;
		int initial_state;
		std::vector<int> marked_states;
		std::vector<std::string> events;
		std::map<std::pair<int, std::string>, std::vector<int> > transitions;
};