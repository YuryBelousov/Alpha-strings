#include "alpha-string.h"

using namespace std;

using all_alphas_type = vector<vector <alpha_string>>;

//Check if alhpa is interesting 
bool is_interesting(const alpha_string& A) {
	return !(A.simple() || A.with_simple_loops());
}

//The prprocedure for generating all alpha-strings of complexity last_n + 1
void make_alpha_signature(all_alphas_type& all_signatures, all_alphas_type& all_interesting_signatures, int last_n){
	//Resize the container
	if (last_n == all_signatures.size() - 1) {
		all_signatures.resize(2 * (last_n + 1));
		all_interesting_signatures.resize(2* (last_n + 1));
	}
	//Generate alpha-strings of complexity last_n + 1 by adding new arc, which starts at 0 and ends at arbitrary index, to all alpha-strings of complexity last_n
	for (auto &alpha : all_signatures[last_n]) 
		for (int i = 1; i < 2 * (last_n + 2); ++i) {
			auto temp_signature = alpha.expand_by_arc(0, i);
			all_signatures[last_n + 1].push_back(temp_signature);
			if (is_interesting(temp_signature) && (find(all_interesting_signatures[last_n + 1].begin(), all_interesting_signatures[last_n + 1].end(), temp_signature) == all_interesting_signatures[last_n + 1].end())) 
				all_interesting_signatures[last_n + 1].push_back(temp_signature);
		}
}

int main(){
	//Select maximum complexity of generating alpha-string
	int max_complexity = 5;
	//Create container for alpha-strings and insert the simplest alpha-string there
	all_alphas_type all_signuters(1);	
	all_alphas_type all_interesting_signuters(1);

	all_signuters[0].push_back(alpha_string());
	
	for (size_t n = 0; n < max_complexity; ++n)
		make_alpha_signature(all_signuters, all_interesting_signuters, n);

	ofstream file_out("alpha-strings up to complexity " + to_string(max_complexity) +".txt");

	for (size_t i = 0; i <= max_complexity; ++i) {
		if (all_interesting_signuters[i].size() > 0) {
			file_out << "Interesting alpha-strings with complexity " << i << " (" << all_interesting_signuters[i].size() << "):" << endl;
			for (alpha_string& alp : all_interesting_signuters[i])
				file_out << "\t" << alp << endl;
		}
	}
	return 0;
}