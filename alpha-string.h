#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
/*
Here is a class for alpha-string: specific modification of the Gauss code.
It has the form: A_{l_k}A_{l_{k-1}}...A_{l_1} | A_{r_1}A_{r_2}...A_{r_k}, 
where for each i in 1:k there exists j in 1:k such that  A_{l_i} = A_{r_j}. 

Such strings are stored in vector in a form somewhat similar to chord diagrams. 
It is a vector of integers (of indexes), where
if A_{l_i} = A_{r_j} then alphas[k - l_i] = k - 1 + r_j and alphas[k - 1 + r_j] = k - l_i.
*/

class alpha_string {
private:
	int num_of_alphas;		//Number of alphas in the string. It is the same as alphas.size();
	vector <int> alphas;	//Notice: i = alpha[alpha[i]]

public:
	//Default constructors
	alpha_string()						= default;
	alpha_string(const alpha_string&)	= default;
	alpha_string(alpha_string&&)		= default;

	//Construct alpha-string from a vector
	explicit alpha_string(const vector <int>& _alphas) {
		//Check if _alphas presents a correct alpha-string
		for (int i = 0; i < _alphas.size(); ++i) {
			if ((i != _alphas[_alphas[i]]) || (i == _alphas[i])) {
				num_of_alphas = 0;
				return;
			}
		}
		//If it is correct, save it
		alphas = _alphas;
		num_of_alphas = alphas.size();
	}

	//Comparison operators
	bool operator==(const alpha_string& A) const {
		if (A.num_of_alphas != num_of_alphas)
			return false;
		if (A.alphas == alphas)
			return true;
		//Compare alpha with A.alpha reversed relative to center point 
		for (int i = 0; i < num_of_alphas; ++i)
			if (A.alphas[i] != num_of_alphas - 1 - alphas[num_of_alphas - 1 - i])
				return false;
		return true;
	}

	bool operator!=(const alpha_string& A) const {
		return !(operator==(A));
	}

	//Create alpha-string by adding new arc from first_ind to last_ind to the current string
	alpha_string expand_by_arc(int first_ind, int last_ind) const {
		//Check if new indexes are correct
		if (first_ind < 0)
			first_ind = 0;
		if (last_ind < 0)
			last_ind = 0;
		if (first_ind > num_of_alphas)
			first_ind = num_of_alphas;
		if (last_ind > num_of_alphas + 1)
			last_ind = num_of_alphas + 1;
		if (first_ind == last_ind)
			++last_ind;
		if (first_ind > last_ind)
			swap(first_ind, last_ind);

		//Create new alpha-string with n + 2 alphas
		alpha_string temp;
		temp.alphas.resize(num_of_alphas + 2);
		temp.num_of_alphas = num_of_alphas + 2;
		//Make first arc
		temp.alphas[first_ind] = last_ind;
		temp.alphas[last_ind] = first_ind;
		//This function is used to copy alphas from current string to temp taking into account ends of new arc
		auto change_func = [](int& a, int b, int first_ind, int last_ind) {
			if (b < first_ind)
				a = b;
			else if (b + 1 >= last_ind)
				a = b + 2;
			else
				a = b + 1;
		};
		//Copy alphas from current string to temp, shifting indexes if any end of the new arc was met
		for (int i = 0; i < first_ind; ++i) 
			change_func(temp.alphas[i], alphas[i], first_ind, last_ind);
		for (int i = first_ind + 1; i < last_ind; ++i) 
			change_func(temp.alphas[i], alphas[i-1], first_ind, last_ind);
		for (int i = last_ind + 1; i < num_of_alphas + 2; ++i) 
			change_func(temp.alphas[i], alphas[i-2], first_ind, last_ind);
		return temp;
	}

	//Check if all alphas can be eliminated in linear time
	bool simple() const {
		//Check if all alphas can be eliminated without passing through a loop. 
		//It means that there exists M, such that in subsequence {A_{l_M}, A_{l_{M-1}},..A{l_1} | A_{r_1},.., A_{r_{n/2-M}}} all elements are different
		//If i = 1 || i = num_of_alphas / 2 - 1 all alphas can be eliminated in linear time but constant is too big, so we have to analyze it in more detail.
		for (int i = 0; i < num_of_alphas / 2 - 1; ++i) {
			if (i == 1)
				continue;
			int j = 0;
			for (; j < num_of_alphas / 2; ++j)
				if ((alphas[i + j] >= i) && (alphas[i + j] < i + num_of_alphas / 2))
					break;
			if (j == num_of_alphas / 2)
				return true;
		}
		return false;
	}

	//Check if there are no simple loops. It means that alphas[i] != i + 1 for all i
	bool with_simple_loops() const{		
		for (int i = 0; i < num_of_alphas - 1; ++i)
			if (alphas[i] == i + 1)
				return true;	
		return false;
	}

	//Print alpha-string in a convinient way
	friend ostream& operator<<(ostream& out, const alpha_string& _a_str) {
		vector <int> temp = _a_str.alphas;
		vector <bool> temp_not_changed(_a_str.num_of_alphas, true);	//Show if alphas[i] already have a name in alpha-string
		int num = 0;											//Name of current alpha
		//Prepare right part
		for (int i = _a_str.num_of_alphas / 2; i < _a_str.num_of_alphas; ++i) {
			if (temp_not_changed[i]) {
				temp_not_changed[i] = temp_not_changed[_a_str.alphas[i]] = false;
				temp[i] = temp[_a_str.alphas[i]] = ++num;
			}
		}
		//Prepare left part
		for (int i = _a_str.num_of_alphas / 2 - 1; i > -1; --i) {
			if (temp_not_changed[i]) {
				temp_not_changed[i] = temp_not_changed[_a_str.alphas[i]] = false;
				temp[i] = temp[_a_str.alphas[i]] = ++num;
			}
		}
		//Print result
		for (int i = 0; i < _a_str.num_of_alphas; ++i) {
			if (i == _a_str.num_of_alphas / 2)
				out << "| ";
			out << "[" << temp[i] << "] ";
		}
		return out;
	}
};