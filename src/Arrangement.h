/*
 * Arrangement.h
 *
 *  Created on: Dec 8, 2016
 *      Author: diepbp
 */

#ifndef ARRANGEMENT_H_
#define ARRANGEMENT_H_

#include "utils.h"
#include "regex.h"

#define ROUNDCHECK 1
#define LOCALSPLITMAX 20
#define SUMFLAT 100000000
#define EMPTYFLAT 9999999

#define REGEX_CODE -100
#define MINUSZERO 999

class Arrangment{
public:

	std::vector<int> left_arr;
	std::vector<int> right_arr;

	std::map<std::string, std::string> constMap; // extra info for all Arrangement

	Arrangment(std::vector<int> _left_arr,
			std::vector<int> _right_arr,
			std::map<std::string, std::string> _constMap){

		constMap.clear();

		left_arr.insert(left_arr.end(), _left_arr.begin(), _left_arr.end());
		right_arr.insert(right_arr.end(), _right_arr.begin(), _right_arr.end());

		/* extra info */
		constMap.insert(_constMap.begin(), _constMap.end());

	}

	void addLeft(int number) {
		left_arr.emplace_back(number);
	}

	void addRight(int number) {
		right_arr.emplace_back(number);
	}

	bool canSplit(int boundedFlat, int boundSize, int pos, std::string frame, std::vector<std::string> &flats) {
		if ((int)flats.size() == boundedFlat)
			return false;

		for (int size = 1; size <= boundSize; ++size) { /* size of a flat */
			std::string flat = frame.substr(pos, size);
			flats.emplace_back(flat); /* add to stack */
			int tmpPos = pos + size;

			while (true) {
				std::string nextIteration = frame.substr(tmpPos, size);
				if (nextIteration.compare(flat) != 0)
					break;
				else if (tmpPos < (int)frame.length() && tmpPos + size <= (int)frame.length()){
					tmpPos += size;
				}
				else
					break;
			}
			if (tmpPos < (int)frame.length()){
				if (canSplit(boundedFlat, boundSize, tmpPos, frame, flats))
					return true;
				else {
					/* de-stack */
					flats.pop_back();
				}
			}
			else {
				return true;
			}
		}
		return false;
	}

	/*
	 * Input: string that we are going to split, and number of flats
	 * Ouput: able to split or not
	 */
	int splitWithMinFlats(int boundedFlat, int boundSize, std::string frame){
		for (int i = 1; i <= boundedFlat; ++i) { /* number of flats */
			std::vector<std::string> flats;
			if (canSplit(i, PMAX, 0, frame, flats)){
				return i;
			}
			flats.clear();
		}
		return -1;
	}

	void splitPrintTest(std::vector<int> currentSplit, std::string msg = ""){
		__debugPrint(logFile, "%s\n", msg.c_str());
		for (unsigned int i = 0; i < currentSplit.size(); ++i)
			__debugPrint(logFile, "%d - ", currentSplit[i]);
		__debugPrint(logFile, "\n------------\n");
	}

	/**
	 * Print a list
	 */
	template<typename T>
	void printList(T list, std::string msg = "") {
		if (msg.length() > 0 )
			printf("%s\n", msg.c_str());
		for (std::vector<int>::iterator it = list.begin(); it != list.end(); ++it) {
			printf("%d ", *it);
		}
		printf("\n");
	}

	bool isUnionStr(std::string str){
		return str.find("|") != std::string::npos;
	}

	bool feasibleSplit_const(
			std::string str,
			std::vector<std::pair<std::string, int> > elementNames,
			std::vector<int> currentSplit){
		/* check feasible const split */
		int pos = 0;
		for (unsigned i = 0; i < currentSplit.size(); ++i) {
			if (elementNames[i].second == REGEX_CODE || isUnionStr(elementNames[i].first)) {
			}

			/* TODO: bound P */
			else if (elementNames[i].second < 0) { /* const */
				if (currentSplit[i] > (int)elementNames[i].first.length()) {
					__debugPrint(logFile, "%d %s (%ld), part %d -- %d\n", __LINE__, elementNames[i].first.c_str(), elementNames[i].first.length(), elementNames[i].second, currentSplit[i]);
				}
				assert ((int)elementNames[i].first.length() >= currentSplit[i]);

				std::string lhs = str.substr(pos, currentSplit[i]);
				std::string rhs = "";
				if (elementNames[i].second % QCONSTMAX == -1) /* head */ {
					rhs = elementNames[i].first.substr(0, currentSplit[i]);

					if (i + 1 < elementNames.size()) {
						if (QCONSTMAX == 1 || elementNames[i].first.length() == 1) {
							assert (currentSplit[i] == (int)elementNames[i].first.length()); /* const length must be equal to length of const */
						}
						else {
							assert (elementNames[i + 1].second % QCONSTMAX == 0);
							assert ((currentSplit[i] + currentSplit[i + 1] == (int)elementNames[i].first.length())); /* sum of all const parts must be equal to length of const */
						}
					}
				}
				else { /* tail */
					rhs = elementNames[i].first.substr(elementNames[i].first.length() - currentSplit[i], currentSplit[i]);
				}

				if (lhs.compare(rhs) != 0){
					assert(false);
					__debugPrint(logFile, "%d FALSE %s - %s\n", __LINE__, lhs.c_str(), rhs.c_str());
					return false;
				}
			}
			pos += currentSplit[i];
		}
//		__debugPrint(logFile, "%d TRUE\n", __LINE__);
		return true;
	}

	/*
	 * (lhs)* = (rhs)* where lhs starts at posLHS
	 */
	std::vector<int> regex_in_regex_at_pos(std::string lhs, std::string rhs, int posLhs) {
		/* extend string if it is short */
		rhs = parse_regex_content(rhs);

		std::string initialLhs = lhs;
		while (rhs.length() + posLhs > initialLhs.length()) /* make sure that | initialLhs | > | rhs | */
			initialLhs = initialLhs + lhs;

		// bool isloopExist = true;

		std::vector<int> possibleCases;
		possibleCases.emplace_back(0);
		if (rhs.compare(initialLhs.substr(posLhs, rhs.length())) == 0) {
			int pos_lhs = (posLhs + rhs.length()) % lhs.length();
			int iterRhs = 1;

			possibleCases.emplace_back(1);

			std::string double_str = initialLhs + initialLhs;
			while (pos_lhs != posLhs) {

				/* loop until it goes back the inital possition */
				if (rhs.compare(double_str.substr(pos_lhs, rhs.length())) == 0) {
					possibleCases.emplace_back(++iterRhs);
					pos_lhs = (pos_lhs + rhs.length()) % lhs.length();
				}
				else {
					// isloopExist = false;
					break;
				}
			}
		}
		else /* cannot happens */ {
			// isloopExist = false;
		}

		return possibleCases;
	}

	/*
	 * (lhs)* = .* rhs .* where lhs starts at posLhs
	 */
	bool const_in_regex_at_pos(std::string lhs, std::string rhs, int posLhs){

		/* extend string if it is short */
		std::string initialLhs = lhs;
		while (rhs.length() + posLhs > initialLhs.length()) /* make sure that | initialLhs | > | rhs | */
			initialLhs = initialLhs + lhs;

		if (rhs.compare(initialLhs.substr(posLhs, rhs.length())) == 0) {
			return true;
		}
		return false;
	}

	/*
	 * (lhs)* = .* rhs.-2 .* where lhs starts at posLHS
	 */
	std::vector<int> tail_in_regex_at_pos(std::string lhs, std::string rhs, int posLhs){

		std::vector<int> potentialPos;
		for (int i = 0; i <= (int)rhs.length(); ++i) {
			/* length = i */
			std::string tmpRhs = rhs.substr(i);

			/* extend string if it is short */
			std::string initialLhs = lhs;
			while (tmpRhs.length() + posLhs > initialLhs.length()) /* make sure that | initialLhs | > | rhs | */
				initialLhs = initialLhs + lhs;

			if (tmpRhs.compare(initialLhs.substr(posLhs, tmpRhs.length())) == 0) {
				potentialPos.emplace_back(rhs.length() - i);
			}
		}
		return potentialPos;
	}

	/*
	 * (lhs)* = .* rhs.-1 .* where lhs starts at posLHS
	 */
	std::vector<int> head_in_regex_at_pos(std::string lhs, std::string rhs, int posLhs){

		std::vector<int> potentialPos;
		for (int i = 0; i <= (int)rhs.length(); ++i) {
			/* length = i */
			std::string tmpRhs = rhs.substr(0, i);

			/* extend string if it is short */
			std::string initialLhs = lhs;
			while (tmpRhs.length() + posLhs > initialLhs.length()) /* make sure that | initialLhs | > | rhs | */
				initialLhs = initialLhs + lhs;

			if (tmpRhs.compare(initialLhs.substr(posLhs, tmpRhs.length())) == 0) {
				potentialPos.emplace_back(i);
			}
			else
				return potentialPos;
		}
		return potentialPos;
	}

	/*
	 * textLeft: length of string
	 * nMax: number of flats
	 * pMax: size of a flat
	 *
	 * Pre-Condition: 1st flat and n-th flat must be greater than 0
	 * Output: of the form 1 * 1 + 1 * 0 + 1 * 0 + 1 * 0 + 1 * 0 + 1 * 0 + 1 * 0 + 1 * 0 + 1 * 3 + 2 * 3 = 10
	 */
	void collectAllPossibleSplits_const(
			int pos,
			std::string str, /* const */
			int pMax,
			std::vector<std::pair<std::string, int> > elementNames,
			std::vector<int> currentSplit,
			std::vector<std::vector<int> > &allPossibleSplits
	) {
		assert(pos <= (int) str.length());
		/* reach end */
		if (currentSplit.size() == elementNames.size()){
			if (pos == (int)str.length() &&
					feasibleSplit_const(str, elementNames, currentSplit)) {
				allPossibleSplits.emplace_back(currentSplit);
			}
			else {
			}
			return;
		}

		unsigned textLeft = str.length() - pos;

		/* special case for const: leng = leng */
		if (elementNames[currentSplit.size()].second % QCONSTMAX == -1 && (QCONSTMAX == 1 || elementNames[currentSplit.size()].first.length() == 1)) {
			if (elementNames[currentSplit.size()].first.length() <= textLeft) {
				std::string constValue = str.substr(pos, elementNames[currentSplit.size()].first.length());

				if (constValue.compare(elementNames[currentSplit.size()].first) == 0) {
					currentSplit.emplace_back(elementNames[currentSplit.size()].first.length());
					collectAllPossibleSplits_const(pos + elementNames[currentSplit.size() - 1].first.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
					currentSplit.pop_back();
				}
			}
		}

		/* const head */
		else if (elementNames[currentSplit.size()].second % QCONSTMAX == -1 &&
				QCONSTMAX == 2) {
			if (elementNames[currentSplit.size()].first.length() <= textLeft) {
				std::set<std::string> values;
				if (isUnionStr(elementNames[currentSplit.size()].first)){
					values = extendComponent(elementNames[currentSplit.size()].first);
				}
				else
					values.emplace(elementNames[currentSplit.size()].first);

				for (const auto& value : values) {
					std::string constValue = str.substr(pos, value.length());
					if (constValue.compare(value) == 0) {
						if (values.size() > 1)
							__debugPrint(logFile, "%d passed value: %s\n", __LINE__, value.c_str());
						for (int i = 0; i < std::min(7, (int)value.length()); ++i) {
							currentSplit.emplace_back(i);
							collectAllPossibleSplits_const(pos + i, str, pMax, elementNames, currentSplit, allPossibleSplits);
							currentSplit.pop_back();
						}
					}
				}
			}
		}

		/* special case for const tail, when we know the length of const head */
		else if (currentSplit.size() > 0 &&
					elementNames[currentSplit.size()].second % QCONSTMAX == 0 &&
					elementNames[currentSplit.size()].second < 0 &&
					elementNames[currentSplit.size()].second > REGEX_CODE &&
					QCONSTMAX == 2) /* const */ {
			assert (elementNames[currentSplit.size() - 1].second % QCONSTMAX == -1);
			std::set<std::string> values;
			if (isUnionStr(elementNames[currentSplit.size()].first)){
				values = extendComponent(elementNames[currentSplit.size()].first);
			}
			else
				values.emplace(elementNames[currentSplit.size()].first);

			for (const auto& value : values) {
				std::string constValue = str.substr(pos - currentSplit[currentSplit.size() - 1], value.length());
				unsigned length = (unsigned)value.length() - currentSplit[currentSplit.size() - 1]; /* this part gets all const string remaining */

				if (constValue.compare(value) == 0) {
					if (length <= textLeft) {
						currentSplit.emplace_back(length);
						collectAllPossibleSplits_const(pos + length, str, pMax, elementNames, currentSplit, allPossibleSplits);
						currentSplit.pop_back();
					}
				}
			}
		}

		/* head is const part 2*/
		else if (currentSplit.size() == 0 &&
				elementNames[0].second % QCONSTMAX == 0 &&
				elementNames[0].second < 0 &&
				elementNames[0].second > REGEX_CODE &&
				QCONSTMAX == 2) /* const */ {
			std::set<std::string> values;
			if (isUnionStr(elementNames[currentSplit.size()].first)){
				values = extendComponent(elementNames[currentSplit.size()].first);
			}
			else
				values.emplace(elementNames[currentSplit.size()].first);
			for (const auto& value : values)
				for (unsigned i = 0; i < std::min(value.length(), str.length()); ++i) {
					if (values.size() > 1)
						__debugPrint(logFile, "%d passed value: %s\n", __LINE__, value.c_str());
					std::string tmp00 = value.substr(i);
					std::string tmp01 = str.substr(0, i);
					if (tmp00.compare(tmp01) == 0){
						currentSplit.emplace_back(tmp00.length());
						collectAllPossibleSplits_const(pos + tmp00.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
						currentSplit.pop_back();
					}
			}
		}

		else {
			assert(!(elementNames[currentSplit.size()].second < 0 && elementNames[currentSplit.size()].second > REGEX_CODE));

			std::string regexContent = "";
			RegEx re;
			bool canCompile = false;
			if (elementNames[currentSplit.size()].second == REGEX_CODE) /* regex */ {
				regexContent = parse_regex_full_content(elementNames[currentSplit.size()].first);
				if (regexContent.find('|') != std::string::npos) {
					assert(regexContent.find('&') == std::string::npos);
					re.Compile(regexContent);
					canCompile = true;
				}
				else
					regexContent = parse_regex_content(elementNames[currentSplit.size()].first);
			}

			for (unsigned i = 0; i <= textLeft; ++i) {
				unsigned length = i;
				if (elementNames[currentSplit.size()].second == REGEX_CODE) /* regex */ {
					std::string regexValue = str.substr(pos, length);
					if (canCompile == true) {
						if (re.MatchAll(regexValue) == true) {
							currentSplit.emplace_back(length);
							collectAllPossibleSplits_const(pos + length, str, pMax, elementNames, currentSplit, allPossibleSplits);
							currentSplit.pop_back();
						}
					}
					else {
						/* manually doing matching regex */
						std::string tmp = "";
						if (elementNames[currentSplit.size()].first.find('+') != std::string::npos)
							tmp = regexContent;
						else
							assert(elementNames[currentSplit.size()].first.find('*') != std::string::npos);
						while(tmp.length() < regexValue.length())
							tmp += regexContent;

						__debugPrint(logFile, "%d matching %s -- %s\n", __LINE__, tmp.c_str(), regexValue.c_str());
						if (tmp.compare(regexValue) == 0) {
							currentSplit.emplace_back(length);
							collectAllPossibleSplits_const(pos + length, str, pMax, elementNames, currentSplit, allPossibleSplits);
							currentSplit.pop_back();
						}
					}
				}
				else {
					currentSplit.emplace_back(length);
					collectAllPossibleSplits_const(pos + length, str, pMax, elementNames, currentSplit, allPossibleSplits);
					currentSplit.pop_back();
				}
			}
		}
	}

	void collectAllPossibleSplits_regex(
			int pos,
			std::string str, /* regex */
			int pMax,
			std::vector<std::pair<std::string, int> > elementNames,
			std::vector<int> currentSplit,
			std::vector<std::vector<int> > &allPossibleSplits
	) {
		/* reach end */
		if (currentSplit.size() == elementNames.size() &&
				pos == 0) {

			allPossibleSplits.emplace_back(currentSplit);
			return;
		}
		else if (currentSplit.size() >= elementNames.size()) {
			return;
		}

		assert(!isUnionStr(elementNames[currentSplit.size()].first));

		/* special case for const: regex = .* const .* */
		if (elementNames[currentSplit.size()].second == -1 && QCONSTMAX == 1) {
			/* compare text, check whether the string can start from the location pos in text */
			if (const_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos)) {
				currentSplit.emplace_back(elementNames[currentSplit.size()].first.length());
				collectAllPossibleSplits_regex((pos + elementNames[currentSplit.size() - 1].first.length()) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
				currentSplit.pop_back();
			}
		}

		/* special case for const tail, when we know the length of const head */
		else if (elementNames[currentSplit.size()].second % QCONSTMAX == 0 &&
				elementNames[currentSplit.size()].second < 0 &&
				elementNames[currentSplit.size()].second > REGEX_CODE &&
				currentSplit.size() > 0) /* tail, not the first */ {
			assert (elementNames[currentSplit.size() - 1].second % QCONSTMAX == -1);
			int length = elementNames[currentSplit.size()].first.length() - currentSplit[currentSplit.size() - 1]; /* this part gets all const string remaining */

			currentSplit.emplace_back(length);
			collectAllPossibleSplits_regex((pos + length) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
			currentSplit.pop_back();
		}

		else if (elementNames[currentSplit.size()].second % QCONSTMAX == 0 &&
				elementNames[currentSplit.size()].second < 0 &&
				elementNames[currentSplit.size()].second > REGEX_CODE &&
				currentSplit.size() == 0) /* tail, first */ {
			/* find all possible start points */
			std::vector<int> tail = tail_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos);
			for (unsigned i = 0 ; i < tail.size(); ++i) {
				currentSplit.emplace_back(tail[i]);
				collectAllPossibleSplits_regex((pos + tail[i]) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
				currentSplit.pop_back();
			}
		}

		else if (elementNames[currentSplit.size()].second % QCONSTMAX == -1 &&
				elementNames[currentSplit.size()].second < 0 &&
				elementNames[currentSplit.size()].second > REGEX_CODE &&
				currentSplit.size() + 1 == elementNames.size() &&
				QCONSTMAX == 2) /* head, the last element */ {
			/* find all possible start points */
			std::vector<int> head = head_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos);
			for (unsigned i = 0 ; i < head.size(); ++i) {
				currentSplit.emplace_back(head[i]);
				collectAllPossibleSplits_regex((pos + head[i]) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
				currentSplit.pop_back();
			}
		}

		else if (elementNames[currentSplit.size()].second % QCONSTMAX == -1 &&
				elementNames[currentSplit.size()].second < 0 &&
				elementNames[currentSplit.size()].second > REGEX_CODE &&
				currentSplit.size() + 1 < elementNames.size() &&
				QCONSTMAX == 2) /* head, not the last element */ {
			/* check full string */
			if (elementNames[currentSplit.size() + 1].second % QCONSTMAX == 0 &&
					elementNames[currentSplit.size() + 1].second < 0 &&
					elementNames[currentSplit.size() + 1].second > REGEX_CODE){
				if (const_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos)) {
					for (unsigned i = 1 ; i <= elementNames[currentSplit.size()].first.length(); ++i) { /* cannot be empty */
						currentSplit.emplace_back(i);
						collectAllPossibleSplits_regex((pos + i) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
						currentSplit.pop_back();
					}
				}
			}
			else {
				/* this const only has 1 part */
				if (const_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos)) {
					for (unsigned i = 1 ; i <= elementNames[currentSplit.size()].first.length(); ++i) { /* cannot be empty */
						currentSplit.emplace_back(i);
						collectAllPossibleSplits_regex((pos) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
						currentSplit.pop_back();
					}
				}
			}
		}

		else if (elementNames[currentSplit.size()].second == REGEX_CODE) /* regex */ {
			std::string content = parse_regex_content(elementNames[currentSplit.size()].first);

			std::vector<int> regexPos = regex_in_regex_at_pos(str, elementNames[currentSplit.size()].first, pos);
			/* loop ? */
			bool loop = false;
			if (regexPos.size() > 0 &&
					regexPos[regexPos.size() - 1] * content.length() % str.length() == 0)
				loop = true;

			assert(regexPos.size() >= 1);
			if (loop == true) /* assign value < 0 */
				for (unsigned int i = 1 ; i < regexPos.size(); ++i) {
					/* because of loop, do not care about 0 iteration */
					int tmp = (content.length() * regexPos[i]) % str.length();
					if (tmp == 0)
						currentSplit.emplace_back(MINUSZERO);
					else
						currentSplit.emplace_back(-tmp);
					collectAllPossibleSplits_regex((pos + content.length() * regexPos[i]) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
					currentSplit.pop_back();
				}
			else
				for (unsigned int i = 0 ; i < regexPos.size(); ++i) { /* assign value >= 0 */
					int tmp = (pos + content.length() * regexPos[i]) % str.length();
					currentSplit.emplace_back(content.length() * regexPos[i]);
					collectAllPossibleSplits_regex(tmp, str, pMax, elementNames, currentSplit, allPossibleSplits);
					currentSplit.pop_back();
				}
		}

		else {
			for (unsigned i = 0; i < str.length(); ++i) { /* assign value < 0 because it can iterate many times */
				int length = i;
				if (length == 0)
					currentSplit.emplace_back(MINUSZERO);
				else
					currentSplit.emplace_back(- length);
				collectAllPossibleSplits_regex((pos + length) % str.length(), str, pMax, elementNames, currentSplit, allPossibleSplits);
				currentSplit.pop_back();
			}
		}
	}

	/*
	 * Given a flat,
	 * generate its size constraint
	 */
	std::string generateFlatSize(std::pair<std::string, int> a, std::string l_r_hs = ""){
		std::string result = "";
		if (a.second >= 0) {
			/* simpler version */
			result += LENPREFIX;
			result += a.first;
			result += "_";
			result += std::to_string(a.second);
		}
		else {
			/* const string */
			assert (l_r_hs.length() > 0);

			assert (constMap.find(a.first) != constMap.end());
			result += LENPREFIX;
			result += constMap[a.first];
			result += "_";
			result += std::to_string(std::abs(a.second));
		}
		return result;
	}

	/*
	 * Given a flat,
	 * generate its array name
	 */
	std::string generateFlatArray(std::pair<std::string, int> a, std::string l_r_hs = ""){
		std::string result = "";
		if (a.second >= 0) {
			/* simpler version */
			result += ARRPREFIX;
			result += a.first;
		}
		else {
			/* const string */
			assert (l_r_hs.length() > 0);
			result += ARRPREFIX;
			result += constMap[a.first];
		}
		return result;
	}

	/*
	 * Given a flat,
	 * generate its array name
	 */
	std::string generateFlatArray_forComponent(std::pair<std::string, int> a, std::string l_r_hs = ""){
		assert(a.second != REGEX_CODE);
		std::string result =  generateFlatArray(a, l_r_hs) + "_" + std::to_string(a.second);
		return result;
	}

	std::string addConstraint(std::vector<std::string> elements) {
		std::string result = "";
		if (elements.size() > 1) {
			result = "(+ ";
			for (unsigned int i = 0; i < elements.size(); ++i)
				result = result + elements[i] + " ";
			result = result + ")";
		}
		else if (elements.size() == 1){
			result = elements[0];
		}
		else
			result = ZERO;
		return result;
	}

	/*
	 * (+ a b c)
	 */
	std::string addConstraint_full(std::vector<std::string> elements) {
		std::string result = "";
		if (elements.size() == 0)
			return ZERO;
		if (elements.size() == 1)
			return elements[0];
		if (elements.size() > 1) {
			result = "(+ ";
			for (unsigned int i = 0; i < elements.size(); ++i)
				result = result + elements[i] + " ";
			result = result + ")";
		}
		return result;
	}

	/*
	 * Input: constA and a number of flats
	 * Output: all possible ways to split constA
	 */
	std::vector<std::vector<int> > collectAllPossibleSplits(
			std::pair<std::string, int> lhs,
			std::vector<std::pair<std::string, int> > elementNames
	){
		/* create alias elementNames with smaller number of elements*/
		std::vector<std::pair<std::string, int> > alias;
		int pre = 0;
		int cnt = 0;
		for (unsigned i = 0; i < elementNames.size(); ++i)
			if (elementNames[i].second < 0) {
				if (pre > 0) {
					alias.emplace_back(std::make_pair("e" + std::to_string(cnt++), pre));
					pre = 0;
				}
				alias.emplace_back(elementNames[i]);
			}
			else
				pre++;
		if (pre > 0)
			alias.emplace_back(std::make_pair("e" + std::to_string(cnt++), pre));

		/* use alias instead of elementNames */
		std::vector<std::vector<int> > allPossibleSplits;
		assert(lhs.second < 0);

		if (lhs.second == REGEX_CODE) /* regex */ {
			std::vector<int> curr;
			std::string regexContent = parse_regex_content(lhs.first);
			collectAllPossibleSplits_regex(0, regexContent, 10, alias, curr, allPossibleSplits);
		}
		else if (lhs.second % QMAX == 0) /* tail */ {
			for (unsigned int i = 0; i <= lhs.first.length(); ++i) {
				std::vector<int> curr;
				collectAllPossibleSplits_const(0, lhs.first.substr(i), 10, alias, curr, allPossibleSplits);
			}
		}
		else if (lhs.second % QMAX == -1) /* head */ {
			if (QCONSTMAX == 1) {
				std::vector<int> curr;
				collectAllPossibleSplits_const(0, lhs.first, 10, alias, curr, allPossibleSplits);
			}
			else for (unsigned i = 0; i <= lhs.first.length(); ++i) {
				std::vector<int> curr;

				collectAllPossibleSplits_const(0, lhs.first.substr(0, i), 10, alias, curr, allPossibleSplits);

			}
		}
		else {
			__debugPrint(logFile, "%d %s - %d: %d\n", __LINE__, lhs.first.c_str(), lhs.second, lhs.second % QMAX);
			assert(false);
		}

		/* print test */
		__debugPrint(logFile, "%d *** %s *** ", __LINE__, __FUNCTION__);
		for (unsigned int i = 0; i < allPossibleSplits.size(); ++i){
			splitPrintTest(allPossibleSplits[i], "Accepted");
		}

		return allPossibleSplits;
	}

	/*
	 * a b c
	 */
	std::string addConstraint_half(std::vector<std::string> elements) {
		std::string result = "";
		if (elements.size() == 0)
			return ZERO;
		if (elements.size() == 1)
			return elements[0];
		if (elements.size() > 1) {
			result = "";
			for (unsigned int i = 0; i < elements.size(); ++i)
				result = result + elements[i] + " ";
		}
		return result;
	}

	/*
	 * pre elements + pre fix of itself
	 */
	std::string leng_prefix_lhs(std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs, std::string rhs,
			int pos) {
		std::vector<std::string> addElements;

		if (a.second != REGEX_CODE) {
			if (a.second % QCONSTMAX != -1)
				for (int i = a.second + 1; i < 0; ++i){ /* prefix of a - const */
					addElements.emplace_back(generateFlatSize(std::make_pair(a.first, i), lhs));
					if (i % QCONSTMAX == -1)
						break;
				}

			if (a.second % QMAX != 0)
				for (int i = a.second - 1; i >= 0; --i){ /* a is var */
					addElements.emplace_back(generateFlatSize(std::make_pair(a.first, i), lhs));
					if (i % QMAX == 0)
						break;
				}
		}
		else {
			// skip
		}

		for (int i = pos - 1; i >= 0; --i) { /* pre-elements */
			addElements.emplace_back(generateFlatSize(elementNames[i], rhs));
		}

		return addConstraint_half(addElements);
	}

	/*
	 *  pre fix of itself
	 */
	std::string leng_prefix_rhs(std::pair<std::string, int> a, /* var */
			std::string rhs) {
		std::vector<std::string> addElements;
		if (a.second != REGEX_CODE) {
			if (a.second % QCONSTMAX != -1)
				for (int i = a.second + 1; i < 0; ++i){ /* a is const */
					addElements.emplace_back(generateFlatSize(std::make_pair(a.first, i), rhs));
					if (i % QCONSTMAX == -1)
						break;
				}

			if (a.second % QMAX != 0)
				for (int i = a.second - 1; i >= 0; --i){ /* a is var */
					addElements.emplace_back(generateFlatSize(std::make_pair(a.first, i), rhs));
					if (i % QMAX == 0)
						break;
				}
		}
		else {
			// skip
		}

		return addConstraint_half(addElements);
	}

	/*
	 * 0: No const, No connected var
	 * 1: const		No connected var
	 * 2: no const, connected var
	 * 3: have both
	 */
	int checkTheBestSplitType(
			std::vector<std::pair<std::string, int>> elementNames,
			std::map<std::string, int> connectedVariables){

		bool havingConst = false;
		bool havingConnectedVar = false;

		/* check if containing const | regex */
		for (unsigned int i = 0 ; i < elementNames.size(); ++i)
			if (elementNames[i].second < 0) {
				havingConst = true;
				break;
			}

		/* check if containing connected vars */
		for (unsigned int i = 0 ; i < elementNames.size(); ++i)
			if (connectedVariables.find(elementNames[i].first) != connectedVariables.end()) {
				havingConnectedVar = true;
				break;
			}

		/* 0: No const, No connected var */
		/* 1: No const */
		if (!havingConnectedVar && !havingConst)
			return 0;
		else if (!havingConnectedVar && havingConst)
			return 1;
		else if (havingConnectedVar && !havingConst)
			return 2;
		else return 3;
	}

	/*
	 *
	 */
	std::string lengthConstraint_toConnectedVarConstraint(
			std::pair<std::string, int> a, /* const || regex */
			std::vector<std::pair<std::string, int> > elementNames,
			std::vector<std::string> subElements,
			int currentPos,
			int subLength,
			std::string lhs, std::string rhs,
			std::map<std::string, int> connectedVariables){

		/*TODO: only hande 1 connected var here. Need to extend. Or re-write the alias step*/
		int connectedVarPos = -1;
		int connectedVarCnt = 0;
		std::string constraint = "";
		for (int i = currentPos - subElements.size() + 1; i <= currentPos; ++i)
			if (connectedVariables.find(elementNames[i].first) != connectedVariables.end()) {
				connectedVarPos = i;
				connectedVarCnt += 1;
				constraint += connectedVar_atSpecificLocation(
									a, /* const or regex */
									elementNames, /* have connected var */
									connectedVarPos,
									subLength,
									lhs, rhs,
									connectedVariables);
			}

		if (connectedVarCnt == 0 || constraint.length() < 3)
			return "";
		else if (connectedVarCnt == 1)
			return constraint;
		else
			return "(and " + constraint + ")";
	}

	/*
	 * Input: split a string
	 * Output: SMT
	 */
	std::string fromSplitToLengConstraint_havingConnectedVar_andConst(
			std::pair<std::string, int> a, /* const || regex */
			std::vector<std::pair<std::string, int> > elementNames, /* const + connected var */
			std::vector<int> split,
			std::string lhs, std::string rhs,
			std::map<std::string, int> connectedVariables){
//		__debugPrint(logFile, "%d const|regex = const + connected var\n", __LINE__);
		int totalLength = 0;
		for (unsigned int j = 0; j < split.size(); ++j)
			if (split[j] > 0 && split[j] != MINUSZERO)
				totalLength = totalLength + split[j];
			else {
				totalLength = -1;
				break;
			}

		std::vector<std::string> strAnd;
		if (totalLength > 0) /* only has const, does not have regex */ {
			strAnd.emplace_back(createEqualConstraint(generateFlatSize(a, lhs), std::to_string(totalLength)));
		}
		std::vector<std::string> addElements;

		addElements.clear();
		unsigned int splitPos = 0;

		std::string content = "";
		if (a.second == REGEX_CODE)
			content = parse_regex_content(a.first);

		for (unsigned int i = 0; i < elementNames.size(); ++i){
			if (elementNames[i].second >= 0) /* not const */ {
				addElements.emplace_back(generateFlatSize(elementNames[i]));
			}
			else { /* const */
				if (addElements.size() > 0){ /* create a sum for previous elements */
					std::string constraintForConnectedVar = lengthConstraint_toConnectedVarConstraint(
							a, /* const or regex */
							elementNames, /* have connected var */
							addElements,
							i - 1,
							split[splitPos],
							lhs, rhs,
							connectedVariables);
					strAnd.emplace_back(constraintForConnectedVar);
					if (split[splitPos] == MINUSZERO) {
						/* looping */
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(ZERO, createModOperator(addConstraint_full(addElements), std::to_string(content.length()))));
					}
					else if (split[splitPos] < 0) {
						/* looping */
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(createModOperator(addConstraint_full(addElements), std::to_string(content.length())),
																	std::to_string(std::abs(split[splitPos]))));
					}
					else {
						strAnd.emplace_back(createEqualConstraint(addConstraint_full(addElements), std::to_string(split[splitPos])));
					}
					splitPos++;
					addElements.clear();

				}

				if (elementNames[i].second % QCONSTMAX == -1 && i < elementNames.size() - 1) {
					if (QCONSTMAX == 1 || elementNames[i].first.length() == 1) {
						strAnd.emplace_back(createEqualConstraint(generateFlatSize(elementNames[i], rhs), std::to_string(split[splitPos])));
						splitPos++;
					}
					else {
						assert(elementNames[i + 1].second % QCONSTMAX == 0);
						strAnd.emplace_back(createEqualConstraint(
								"(+ " + generateFlatSize(elementNames[i], rhs) + " " + generateFlatSize(elementNames[i + 1], rhs) + ")",
								std::to_string(split[splitPos] + split[splitPos + 1])));
						i++;
						splitPos += 2;
					}
				}
				else {
					if (split[splitPos] == MINUSZERO) {
						/* looping at 0 */
						assert(elementNames[i].second == REGEX_CODE);
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(
								ZERO,
								createModOperator(generateFlatSize(elementNames[i], rhs), std::to_string(content.length()))));
						splitPos++;
					}
					else if (split[splitPos] < 0) {
						/* looping */
						assert(elementNames[i].second == REGEX_CODE);
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(
								createModOperator(generateFlatSize(elementNames[i], rhs), std::to_string(content.length())),
								std::to_string(std::abs(split[splitPos++]))));
					}
					else
						strAnd.emplace_back(createEqualConstraint(
								generateFlatSize(elementNames[i], rhs),
								std::to_string(split[splitPos++])));
				}
			}
		}

		if (addElements.size() > 0) {
			std::string constraintForConnectedVar = lengthConstraint_toConnectedVarConstraint(
					a, /* const or regex */
					elementNames, /* have connected var */
					addElements,
					elementNames.size() - 1,
					split[splitPos],
					lhs, rhs,
					connectedVariables);
//			__debugPrint(logFile, "%d constraintForConnectedVar: %s", __LINE__, constraintForConnectedVar.c_str());
			strAnd.emplace_back(constraintForConnectedVar);

			/* create a sum for previous elements */
			if (split[splitPos] == MINUSZERO) {
				/* looping */
				assert(a.second == REGEX_CODE);
				strAnd.emplace_back(createEqualConstraint(
						ZERO,
						createModOperator(addConstraint_full(addElements), std::to_string(content.length()))));
			}
			else if (split[splitPos] < 0) {
				/* looping */
				assert(a.second == REGEX_CODE);
				strAnd.emplace_back(createEqualConstraint(
						createModOperator(addConstraint_full(addElements), std::to_string(content.length())),
						std::to_string(std::abs(split[splitPos]))));
			}
			else {
				strAnd.emplace_back(createEqualConstraint(addConstraint_full(addElements), std::to_string(split[splitPos])));
			}
			splitPos++;
		}
		assert(splitPos == split.size());
		return andConstraint(strAnd);
	}

	/*
	 * Input: split a string
	 * Output: SMT
	 */
	std::string fromSplitToLengConstraint_NoConnectedVar(
			std::pair<std::string, int> a, /* const || regex */
			std::vector<std::pair<std::string, int> > elementNames, /* no connected var */
			std::vector<int> split,
			std::string lhs, std::string rhs){
		__debugPrint(logFile, "%d const|regex = const + ...\n", __LINE__);
		int totalLength = 0;
		for (unsigned int j = 0; j < split.size(); ++j)
			if (split[j] > 0)
				totalLength = totalLength + split[j];
			else {
				totalLength = -1;
				break;
			}

		std::vector<std::string> strAnd;
		if (totalLength > 0) /* only has const, does not have regex */
			strAnd.emplace_back(createEqualConstraint(generateFlatSize(a, lhs), std::to_string(totalLength)));

		std::vector<std::string> addElements;

		/* simple case: check if size of all consts = 0 */
		bool sumConst_0 = true, metVar = false;
		unsigned int splitPos = 0;
		for (unsigned int i = 0; i < elementNames.size(); ++i) {
			if (elementNames[i].second < 0) {
				if (metVar)
					splitPos++;
				if (split[splitPos++] > 0){
					sumConst_0 = false;
					break;
				}
				addElements.emplace_back(generateFlatSize(elementNames[i], rhs));
				metVar = false;
			}
			else
				metVar = true;
		}

		if (sumConst_0 == true) {
			return createEqualConstraint(ZERO, addConstraint_full(addElements));
		}
		else {
		}

		/* work as usual */
		addElements.clear();
		splitPos = 0;
		std::string content = "";
		if (a.second == REGEX_CODE)
			content = parse_regex_content(a.first);

		for (unsigned int i = 0; i < elementNames.size(); ++i){
			if (elementNames[i].second >= 0) /* not const */ {
				addElements.emplace_back(generateFlatSize(elementNames[i]));
			}
			else { /* const */
				if (addElements.size() > 0){ /* create a sum for previous elements */
					if (split[splitPos] == MINUSZERO) {
						/* looping */
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(ZERO, createModOperator(addConstraint_full(addElements), std::to_string(content.length()))));

					}
					else if (split[splitPos] < 0) {
						/* looping */
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(createModOperator(addConstraint_full(addElements), std::to_string(content.length())), std::to_string(std::abs(split[splitPos]))));
					}
					else {
						strAnd.emplace_back(createEqualConstraint(addConstraint_full(addElements), std::to_string(split[splitPos])));
					}
					splitPos++;
					addElements.clear();

				}

				if (elementNames[i].second % QCONSTMAX == -1 && i < elementNames.size() - 1) {
					if (QCONSTMAX == 1 || elementNames[i].first.length() == 1) {
						strAnd.emplace_back(createEqualConstraint(generateFlatSize(elementNames[i], rhs), std::to_string(split[splitPos])));
						splitPos++;
					}
					else {
						assert(elementNames[i + 1].second % QCONSTMAX == 0);
						strAnd.emplace_back(createEqualConstraint("(+ " + generateFlatSize(elementNames[i], rhs) + " " + generateFlatSize(elementNames[i + 1], rhs) + ")", std::to_string(split[splitPos] + split[splitPos + 1])));
						i++;
						splitPos += 2;
					}
				}
				else {
					if (split[splitPos] == MINUSZERO) {
						/* looping at 0 */
						assert(elementNames[i].second == REGEX_CODE);
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(
								ZERO,
								createModOperator(generateFlatSize(elementNames[i], rhs), std::to_string(content.length()))));
						splitPos++;
					}
					else if (split[splitPos] < 0) {
						/* looping */
						assert(elementNames[i].second == REGEX_CODE);
						assert(a.second == REGEX_CODE);
						strAnd.emplace_back(createEqualConstraint(
								createModOperator(generateFlatSize(elementNames[i], rhs), std::to_string(content.length())),
								std::to_string(std::abs(split[splitPos++]))));
					}
					else
						strAnd.emplace_back(createEqualConstraint(generateFlatSize(elementNames[i], rhs), std::to_string(split[splitPos++])));
				}
			}
		}

		if (addElements.size() > 0) {
			/* create a sum for previous elements */
			if (split[splitPos] == MINUSZERO) {
				/* looping */
				assert(a.second == REGEX_CODE);
				strAnd.emplace_back(createEqualConstraint(ZERO, createModOperator(addConstraint_full(addElements), std::to_string(content.length()))));
			}
			else if (split[splitPos] < 0) {
				/* looping */
				assert(a.second == REGEX_CODE);
				strAnd.emplace_back(createEqualConstraint(
						createModOperator(addConstraint_full(addElements), std::to_string(content.length())),
						std::to_string(std::abs(split[splitPos]))));
			}
			else {
				strAnd.emplace_back(createEqualConstraint(addConstraint_full(addElements), std::to_string(split[splitPos])));
			}
			splitPos++;
		}
		assert(splitPos == split.size());
		return andConstraint(strAnd);
	}

	/*
	 * elementNames[pos] is a connected.
	 * how many parts of that connected variable are in the const | regex
	 */
	int find_partsOfConnectedVariablesInAVector(
			int pos,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string &subLen){
		int partCnt = 1;
		std::vector<std::string> addElements; addElements.emplace_back(generateFlatSize(elementNames[pos]));
		unsigned int j = pos + 1;
		for (j = pos + 1; j < elementNames.size(); ++j)
			if (elementNames[j].second > elementNames[j - 1].second &&
					elementNames[j].second > 0 &&
					elementNames[j].first.compare(elementNames[j - 1].first) == 0 &&
					elementNames[j].second % QMAX != 0 &&
					elementNames[j].second != REGEX_CODE) {
				partCnt++;
				addElements.emplace_back(generateFlatSize(elementNames[j]));
			}
			else
				break;

		/* sublen = part_1 + part2 + .. */
		subLen = addConstraint_full(addElements);
		return partCnt;
	}

	/*
	 *
	 */
	std::string fromSplitToLengConstraint_ConnectedVar(
			std::pair<std::string, int> a, /* const or regex */
			std::vector<std::pair<std::string, int>> elementNames, /* have connected var, do not have const */
			std::string lhs, std::string rhs,
			std::map<std::string, int> connectedVariables){
		__debugPrint(logFile, "%d const|regex = connected var + ...\n", __LINE__);
		std::string arrayLhs = generateFlatArray(a, lhs);
		std::vector<std::string> resultParts;

		std::string content = "";
		if (a.second == REGEX_CODE)
			content = parse_regex_content(a.first);
		else
			content = a.first;

		for (unsigned int i = 0; i < elementNames.size(); ++i){
			assert(elementNames[i].second >= 0);
			if (elementNames[i].second >= 0) /* not const */ {

				/* connected variable */
				if (connectedVariables.find(elementNames[i].first) != connectedVariables.end()) {

					/* sublen = part_1 + part2 + .. */
					std::string subLen = "";
					int partCnt = find_partsOfConnectedVariablesInAVector(i, elementNames, subLen);
					std::string prefix_rhs = leng_prefix_rhs(elementNames[i], rhs);
					std::string prefix_lhs = leng_prefix_lhs(a, elementNames, lhs, rhs, i);

					if (a.second == REGEX_CODE) {
						std::vector<std::string> conditions;
						if (partCnt == 1) {
							__debugPrint(logFile, ">> %d const|regex = connected var partCnt = 1. NOT TESTED\n", __LINE__);
							/* this part = regex */
							/* prefix mod lenth = 0 */
							conditions.emplace_back(createEqualConstraint(ZERO, createModOperator(prefix_rhs, std::to_string(content.length()))));
							conditions.emplace_back(createEqualConstraint(ZERO, createModOperator(subLen, std::to_string(content.length()))));

#if 0
							std::string partArray = generateFlatArray_forComponent(elementNames[i], rhs);
							for (unsigned int j = 0; j < content.length(); ++j)
								subcase.emplace_back(createEqualConstraint(createSelectConstraint(partArray, std::to_string(j)), std::to_string(content[j])));

#else
							std::string arrName = generateFlatArray(elementNames[i], rhs);
							std::string prefix = leng_prefix_rhs(elementNames[i], rhs);

							std::vector<std::string> cases;
							for (unsigned iter = 0; iter < CONNECTSIZE / content.length(); ++iter) {
								std::vector<std::string> subcase;
								subcase.emplace_back(createEqualConstraint(subLen, std::to_string(iter * content.length())));
								for (unsigned int j = 0; j < iter * content.length(); ++j)
									subcase.emplace_back(createEqualConstraint(createSelectConstraint(arrName, "(+ " + prefix + " " + std::to_string(j) + ")"), std::to_string(content[j % content.length()])));
								cases.emplace_back(andConstraint(subcase));
							}
							conditions.emplace_back(orConstraint(cases));
#endif

							resultParts.emplace_back(andConstraint(conditions));
							__debugPrint(logFile, " --> %s \n", andConstraint(conditions).c_str());
						}
						else {
							__debugPrint(logFile, ">> %d const|regex = connected var partCnt = 2.\n", __LINE__);
							assert(partCnt == 2);

							/* this part = regex */
							/* prefix mod lenth = 0 */
							conditions.emplace_back(createEqualConstraint(ZERO, createModOperator(prefix_rhs, std::to_string(content.length()))));
							conditions.emplace_back(createEqualConstraint(ZERO, createModOperator(subLen, std::to_string(content.length()))));

							std::string arrName = generateFlatArray(elementNames[i], rhs);
							for (unsigned iter = 0; iter < CONNECTSIZE / content.length(); ++iter) {
								for (unsigned int j = 0; j < content.length(); ++j)
									conditions.emplace_back(createEqualConstraint(createSelectConstraint(arrName, std::to_string(j + iter * content.length())), std::to_string(content[j])));
							}
							resultParts.emplace_back(andConstraint(conditions));
							__debugPrint(logFile, " --> %s \n", andConstraint(conditions).c_str());
						}
					}
					else {
						bool lhs_zero = false;
						bool rhs_zero = false;
						if (prefix_lhs.length() == 1 && prefix_lhs[0] == '0')
							lhs_zero = true;
						if (prefix_rhs.length() == 1 && prefix_rhs[0] == '0')
							rhs_zero = true;

						std::string arrayRhs = generateFlatArray(elementNames[i], rhs);

						if (QCONSTMAX == 1) {
							resultParts.emplace_back(createEqualConstraint(subLen, std::to_string(content.length())));
							for (unsigned k = 0; k < content.length(); ++k){
								resultParts.emplace_back(createEqualConstraint(
										createSelectConstraint(arrayLhs, "(+ " + std::to_string(k) + " " + prefix_lhs + ")"),
										createSelectConstraint(arrayRhs, "(+ " + std::to_string(k) + " " + prefix_rhs + ")")));
							}
						}
						else {
							/* exists and forall */
#if 0
							char strTmp[1000];
							sprintf(strTmp, "(exists ((%s Int)) (implies (and (< %s %d) (< %s %d))) (forall ((i Int)) (and (< i %s) (= (select %s i) (select %s i)))))",
									subLen.c_str(),
									subLen.c_str(),
									LOCALSPLITMAX,
									a.first.length(),
									subLen.c_str(),
									arrayLhs.c_str(),
									arrayRhs.c_str());
							__debugPrint(logFile, "%d %s\n", __LINE__, strTmp);
#endif
							std::vector<std::string> possibleCases; /* sublen = 0 || sublen = 1 || .. */

							/* clone to optimise length of generated string */
							if (lhs_zero && rhs_zero) {
								for (int j = 0; j <= std::min(LOCALSPLITMAX, (int)content.length()); j++){
									std::vector<std::string> subpossibleCases; /*at_0 = x && at_1 == y && ..*/
									subpossibleCases.emplace_back(createEqualConstraint(subLen, std::to_string(j)));
									for (int k = 0; k < j; ++k){
										subpossibleCases.emplace_back(createEqualConstraint(
												createSelectConstraint(arrayLhs, std::to_string(k)),
												createSelectConstraint(arrayRhs, std::to_string(k))));
									}
									possibleCases.emplace_back(andConstraint(subpossibleCases));
								}
							}
							else if (lhs_zero && !rhs_zero){
								for (int j = 0; j <= std::min(LOCALSPLITMAX, (int)content.length()); j++){
									std::vector<std::string> subpossibleCases; /*at_0 = x && at_1 == y && ..*/
									subpossibleCases.emplace_back(createEqualConstraint(subLen, std::to_string(j)));
									for (int k = 0; k < j; ++k){
										subpossibleCases.emplace_back(createEqualConstraint(
												createSelectConstraint(arrayLhs, std::to_string(k)),
												createSelectConstraint(arrayRhs,  "(+ " + std::to_string(k) + " " + prefix_rhs + ")")));
									}
									possibleCases.emplace_back(andConstraint(subpossibleCases));
								}
							}
							else if (!lhs_zero && rhs_zero){
								for (int j = 0; j <= std::min(LOCALSPLITMAX, (int)content.length()); j++){
									std::vector<std::string> subpossibleCases; /*at_0 = x && at_1 == y && ..*/
									subpossibleCases.emplace_back(createEqualConstraint(subLen, std::to_string(j)));
									for (int k = 0; k < j; ++k){
										subpossibleCases.emplace_back(createEqualConstraint(
												createSelectConstraint(arrayLhs, "(+ " + std::to_string(k) + " " + prefix_lhs + ")"),
												createSelectConstraint(arrayRhs, std::to_string(k))));
									}
									possibleCases.emplace_back(andConstraint(subpossibleCases));
								}
							}
							else for (int j = 0; j <= std::min(LOCALSPLITMAX, (int)content.length()); j++){
								std::vector<std::string> subpossibleCases; /*at_0 = x && at_1 == y && ..*/
								subpossibleCases.emplace_back(createEqualConstraint(subLen, std::to_string(j)));
								for (int k = 0; k < j; ++k){
									subpossibleCases.emplace_back(createEqualConstraint(
											createSelectConstraint(arrayLhs, "(+ " + std::to_string(k) + " " + prefix_lhs + ")"),
											createSelectConstraint(arrayRhs, "(+ " + std::to_string(k) + " " + prefix_rhs + ")")));
								}
								possibleCases.emplace_back(andConstraint(subpossibleCases));
							}
							resultParts.emplace_back(orConstraint(possibleCases));
						}
					}
					i += (partCnt - 1);
				}
			}
		}

		return andConstraint(resultParts);
	}

	/*
	 *
	 */
	std::string connectedVar_atSpecificLocation(
			std::pair<std::string, int> a, /* const or regex */
			std::vector<std::pair<std::string, int>> elementNames, /* have connected var */
			int connectedVarPos,
			int connectedVarLength,
			std::string lhs, std::string rhs,
			std::map<std::string, int> connectedVariables){


		std::vector<std::string> resultParts;

		std::string content = "";
		if (a.second == REGEX_CODE)
			content = parse_regex_content(a.first);
		else
			content = a.first;

		assert(connectedVariables.find(elementNames[connectedVarPos].first) != connectedVariables.end());

		/* how many parts of that connected variable are in the const | regex */
		/* sublen = part_1 + part2 + .. */
		std::string subLen = "";
		find_partsOfConnectedVariablesInAVector(connectedVarPos, elementNames, subLen);

		std::string prefix_rhs = leng_prefix_rhs(elementNames[connectedVarPos], rhs);
		std::string prefix_lhs = leng_prefix_lhs(a, elementNames, lhs, rhs, connectedVarPos);

		std::string arrayRhs = generateFlatArray(elementNames[connectedVarPos], rhs);
		std::string arrayLhs = generateFlatArray(a, lhs);

		if (connectedVarLength >= 0 && connectedVarLength != MINUSZERO) {
			/* sublen = connectedVarLength */
			/* at_0 = x && at_1 == y && ..*/
			int considerLength = connectedVarLength;
			if (connectedVariables[elementNames[connectedVarPos].first] >= 0)
				considerLength = std::min(connectedVariables[elementNames[connectedVarPos].first], considerLength);

			for (int k = 0; k < considerLength; ++k){
				resultParts.emplace_back(createEqualConstraint(
						createSelectConstraint(arrayLhs, "(+ " + std::to_string(k) + " " + prefix_lhs + ")"),
						createSelectConstraint(arrayRhs, "(+ " + std::to_string(k) + " " + prefix_rhs + ")")));
			}
		}
		else {
			assert(a.second == REGEX_CODE);
			// connectedVarLength == MINUSZERO --> resultParts.emplace_back("(= (mod " + subLen + " " + std::to_string(connectedVarLength) + ") 0)");
			// connectedVarLength < 0 --> resultParts.emplace_back("(= (mod " + subLen + " " + std::to_string(content.length()) + ") " + std::to_string(std::abs(connectedVarLength))+ ")");
			/* at_0 = x && at_1 == y && ..*/
			char strTmp[1000];
			std::string len_connectedVar = generateFlatSize(elementNames[connectedVarPos], rhs);
#if 0
			sprintf(strTmp, "(forall ((i Int)) (implies (and (< i %s) (>= i 0)) (= (select %s (+ i %s)) (select %s (mod (+ i %s) %ld)))))",
					subLen.c_str(),
					arrayRhs.c_str(),
					prefix_rhs.c_str(),
					arrayLhs.c_str(),
					prefix_lhs.c_str(),
					content.length());
			resultParts.emplace_back(strTmp);
#else
			resultParts.emplace_back(createLessConstraint(subLen, std::to_string(CONNECTSIZE)));
			for (int i = 0; i < std::min(CONNECTSIZE, 50); ++i) {
				sprintf(strTmp, "(or (= (select %s (+ %d %s)) (select %s (mod (+ %d %s) %ld))) (< %s %d))",
									arrayRhs.c_str(),
									i,
									prefix_rhs.c_str(),
									arrayLhs.c_str(),
									i,
									prefix_lhs.c_str(),
									content.length(),
									len_connectedVar.c_str(),
									i + 1);
				resultParts.emplace_back(strTmp);
			}
#endif
		}

		return andConstraint(resultParts);
	}

	/*
	 * Connected var belongs to Regex
	 */
	std::string connectedVar_belongToRegex(
			std::pair<std::string, int> a, /* regex */
			std::vector<std::pair<std::string, int>> elementNames, /* have connected var */
			int connectedVarPos,
			int connectedVarLength,
			std::string lhs, std::string rhs,
			std::set<std::string> connectedVariables){

		assert(a.second == REGEX_CODE);
		std::vector<std::string> resultParts;

		std::string content = parse_regex_content(a.first);

		assert(connectedVariables.find(elementNames[connectedVarPos].first) != connectedVariables.end());

		/* how many parts of that connected variable are in the const | regex */
		/* sublen = part_1 + part2 + .. */
		std::string subLen = "";
		find_partsOfConnectedVariablesInAVector(connectedVarPos, elementNames, subLen);

		std::string prefix_rhs = leng_prefix_rhs(elementNames[connectedVarPos], rhs);
		std::string prefix_lhs = leng_prefix_lhs(a, elementNames, lhs, rhs, connectedVarPos);

		std::string arrayRhs = generateFlatArray(elementNames[connectedVarPos], rhs);
		std::string arrayLhs = generateFlatArray(a, lhs);

		if (connectedVarLength >= 0 && connectedVarLength != MINUSZERO) {
			/* sublen = connectedVarLength */
			/* at_0 = x && at_1 == y && ..*/
			for (int k = 0; k < connectedVarLength; ++k){
				resultParts.emplace_back(createEqualConstraint(
						createSelectConstraint(arrayLhs, "(+ " + std::to_string(k) + " " + prefix_lhs + ")"),
						createSelectConstraint(arrayRhs, "(+ " + std::to_string(k) + " " + prefix_rhs + ")")));
			}
		}
		else {
			assert(a.second == REGEX_CODE);
			// connectedVarLength == MINUSZERO --> resultParts.emplace_back("(= (mod " + subLen + " " + std::to_string(connectedVarLength) + ") 0)");
			// connectedVarLength < 0 --> resultParts.emplace_back("(= (mod " + subLen + " " + std::to_string(content.length()) + ") " + std::to_string(std::abs(connectedVarLength))+ ")");
			/* at_0 = x && at_1 == y && ..*/
			char strTmp[1000];
			sprintf(strTmp, "(forall ((i Int)) (implies (and (< i %s) (>= i 0)) (= (select %s (+ i %s)) (select %s (mod (+ i %s) %ld)))))",
					subLen.c_str(),
					arrayRhs.c_str(),
					prefix_rhs.c_str(),
					arrayLhs.c_str(),
					prefix_lhs.c_str(),
					content.length());
			resultParts.emplace_back(strTmp);
		}

		return andConstraint(resultParts);
	}

	/*
	 * Case: X =  "abc"
	 * return: (and X_0 = a && X_[1] = b && X_[1] = c)
	 */
	std::string const_to_var(
			std::pair<std::string, int> a,
			std::string constB) {
		std::string result = "";

		std::string varName = a.first + "_" + std::to_string(a.second) + "_";
		for (unsigned int i = 0; i < constB.length(); ++i) {
			/* constraint */
			result += createEqualConstraint(varName + std::to_string(i), std::to_string(constB[i]));
		}
		return result;
	}

	/*
	 * (a|b|c)*_xxx --> range <a, c>
	 */
	std::pair<int, int> collect_char_range(std::string str){
		std::vector<std::string> components = collectAlternativeComponents(parse_regex_content(str));
		std::vector<int> tmpList;
		for (const auto& s : components)
			if (s.length() != 1)
				return std::make_pair(-1, -1);
			else
				tmpList.emplace_back(s[0]);

		if (tmpList.size() <= 1)
			return std::make_pair(-1, -1);

		std::sort(tmpList.begin(), tmpList.end());
		for (unsigned int i = 0; i < tmpList.size() - 1; ++i)
			if (tmpList[i] + 1 != tmpList[i + 1]) {
				return std::make_pair(-1, -1);
			}

		return std::make_pair(tmpList[0], tmpList[tmpList.size() - 1]);
	}

	/*
	 * Generate constraints for the case
	 * X = T . "abc"* . Y . Z
	 * regexPos: position of regex element
	 * return: forAll (i Int) and (i < |abc*|) (y[i + |T|] == a | b | c)
	 */
	std::string handle_Regex_WithPosition_array(
			std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs_str, std::string rhs_str,
			int regexPos,
			std::string extraConstraint = "" /* length = ? */) {
		assert (elementNames[regexPos].second < 0);

		std::vector<std::string> locationConstraint;
		if (extraConstraint.length() > 0)
			locationConstraint.emplace_back(extraConstraint);

		/* find the start position --> */
		std::string pre_lhs = leng_prefix_lhs(a, elementNames, lhs_str, rhs_str, regexPos);

		/* optimize length of generated string */
		std::string lhs_array = generateFlatArray(a, lhs_str);
		std::string rhs_array = generateFlatArray(elementNames[regexPos], rhs_str);

		std::string regex_length = generateFlatSize(elementNames[regexPos], rhs_str);

		char strTmp[5000];

#if 0
		/* forall ((i Int)) (and (< i a.first.length()))*/
		sprintf(strTmp, "(forall ((i Int)) (implies (and (< i %s) (>= i 0)) (= (select %s (+ i %s)) (select %s (mod i %ld)))))",
				regex_length.c_str(),
				lhs_array.c_str(),
				pre_lhs.c_str(),
				rhs_array.c_str(),
				parse_regex_content(elementNames[regexPos].first).length());
		printf("%d %s\n", __LINE__, strTmp);
		return strTmp;

#else
		std::vector<std::string> andConstraints;
		andConstraints.emplace_back(createLessConstraint(regex_length, std::to_string(CONNECTSIZE)));
		std::pair<int, int> charRange = collect_char_range(elementNames[regexPos].first);
		if (charRange.first != -1) {
			for (int i = 0; i < std::min(CONNECTSIZE, 50); ++i) {
				sprintf(strTmp, "(or (and (>= (select %s (+ %d %s)) %d) (<= (select %s (+ %d %s)) %d)) (> %d %s))",
						lhs_array.c_str(),
						i,
						pre_lhs.c_str(),
						charRange.first,

						lhs_array.c_str(),
						i,
						pre_lhs.c_str(),
						charRange.second,
						i + 1,
						generateFlatSize(elementNames[regexPos], rhs_str).c_str()
						);
				andConstraints.emplace_back(strTmp);
			}
		}
		else {
//			__debugPrint(logFile, "%d %s\n", __LINE__, __FUNCTION__);
			unsigned int tmpNum = parse_regex_content(elementNames[regexPos].first).length();
			for (int i = 0; i < std::min(CONNECTSIZE, 50); ++i) {
				sprintf(strTmp, "(or (= (select %s (+ %d %s)) (select %s %d)) (> %d %s))",
						lhs_array.c_str(),
						i,
						pre_lhs.c_str(),
						rhs_array.c_str(),
						i % tmpNum,
						i + 1,
						generateFlatSize(elementNames[regexPos], rhs_str).c_str());
				andConstraints.emplace_back(strTmp);
			}
		}

		return andConstraint(andConstraints);
#endif
	}

	/*
	 * Generate constraints for the case
	 * X = T . "abc" . Y . Z
	 * constPos: position of const element
	 * return: (or (and length header = i && X_i = a && X_[i+1] = b && X_[i+2] = c))
	 */
	std::string handle_Const_WithPosition_array(
			std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs_str, std::string rhs_str,
			int constPos,
			std::string value, /* value of regex */
			int start, int finish,
			std::string extraConstraint = "" /* length = ? */) {

		assert (elementNames[constPos].second < 0);

		std::vector<std::string> locationConstraint;
		if (extraConstraint.length() > 0)
			locationConstraint.emplace_back(extraConstraint);

		/* find the start position --> */
		std::string startPos = leng_prefix_lhs(a, elementNames, lhs_str, rhs_str, constPos);

		/* optimize length of generated string */
		std::string tmp01 = generateFlatArray(a, lhs_str);

		if (startPos.size() == 1 && startPos[0] == '0') {
			for (int i = start; i < finish; ++i) {
				locationConstraint.emplace_back(createEqualConstraint(createSelectConstraint(tmp01, std::to_string(i - start)), std::to_string(value[i])));
			}
		}

		else for (int i = start; i < finish; ++i) {
			locationConstraint.emplace_back(createEqualConstraint(createSelectConstraint(tmp01, "(+ " + std::to_string(i - start) + " " + startPos + ")"), std::to_string(value[i])));
		}

		assert (locationConstraint.size() > 0);
		std::string result = andConstraint(locationConstraint) + "\n";
		__debugPrint(logFile, "%d %s %s\n %s\n", __LINE__, __FUNCTION__, value.c_str(), result.c_str());
		return result;
	}

	/*
	 * connected = a + connected + b
	 */
	std::string handle_connected_connected_array(
			std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs_str, std::string rhs_str,
			int pos,
			int bound){
		/* find the start position --> */
		std::string startLhs = leng_prefix_lhs(a, elementNames, lhs_str, rhs_str, pos);
		std::string startRhs = leng_prefix_rhs(elementNames[pos], rhs_str);

		/* optimize length of generated string */
		std::string arrLhs = generateFlatArray(a, lhs_str);
		std::string arrRhs = generateFlatArray(elementNames[pos], rhs_str);

		std::vector<std::string> andConstraints;
		std::string lenRhs = "";
		/* combine two parts if it is possible */
		if (elementNames[pos].second % QMAX == 0 && pos < (int)elementNames.size() - 1 && QMAX > 1 && elementNames[pos].second >= 0) {
			assert(elementNames[pos + 1].second % QMAX == 1);
			assert(QMAX == 2);
			lenRhs = generateFlatSize(elementNames[pos], rhs_str);
			if (elementNames[pos + 1].second < 10)
				lenRhs = lenRhs.substr(0, lenRhs.length() - 2);
			else if (elementNames[pos + 1].second < 100)
					lenRhs = lenRhs.substr(0, lenRhs.length() - 3);
			else if (elementNames[pos + 1].second < 1000)
				lenRhs = lenRhs.substr(0, lenRhs.length() - 4);
		}
		else
			lenRhs = generateFlatSize(elementNames[pos], rhs_str);

		int consideredSize = std::min(bound + 1, 99);

		if (startLhs.compare(ZERO) != 0 && startRhs.compare(ZERO) != 0) {
			for (int i = 1; i <= consideredSize; ++i){
				std::vector<std::string> orConstraints;
				orConstraints.emplace_back(createEqualConstraint(
						createSelectConstraint(arrLhs, "(+ " + std::to_string(i - 1) + " " + startLhs + ")"),
						createSelectConstraint(arrRhs, "(+ " + std::to_string(i - 1) + " " + startRhs + ")")));
				orConstraints.emplace_back(createLessConstraint(lenRhs, std::to_string(i)));
				andConstraints.emplace_back(orConstraint(orConstraints));
			}
		}
		else if (startLhs.compare(ZERO) == 0 && startRhs.compare(ZERO) == 0){
			for (int i = 1; i <= consideredSize; ++i){
				std::vector<std::string> orConstraints;
				orConstraints.emplace_back(createEqualConstraint(
										createSelectConstraint(arrLhs, std::to_string(i - 1)),
										createSelectConstraint(arrRhs, std::to_string(i - 1))));
				orConstraints.emplace_back(createLessConstraint(lenRhs, std::to_string(i)));
				andConstraints.emplace_back(orConstraint(orConstraints));
			}
		}
		else if (startLhs.compare(ZERO) == 0){
			for (int i = 1; i <= consideredSize; ++i){
				std::vector<std::string> orConstraints;
				orConstraints.emplace_back(createEqualConstraint(
										createSelectConstraint(arrLhs, std::to_string(i - 1)),
										createSelectConstraint(arrRhs, "(+ " + std::to_string(i - 1) + " " + startRhs + ")")));
				orConstraints.emplace_back(createLessConstraint(lenRhs, std::to_string(i)));
				andConstraints.emplace_back(orConstraint(orConstraints));
			}
		}
		else if (startRhs.compare(ZERO) == 0){
			for (int i = 1; i <= consideredSize; ++i){
				std::vector<std::string> orConstraints;
				orConstraints.emplace_back(createEqualConstraint(
										createSelectConstraint(arrLhs, "(+ " + std::to_string(i - 1) + " " + startLhs + ")"),
										createSelectConstraint(arrRhs, std::to_string(i - 1))));
				orConstraints.emplace_back(createLessConstraint(lenRhs, std::to_string(i)));
				andConstraints.emplace_back(orConstraint(orConstraints));
			}
		}
		andConstraints.emplace_back(createLessConstraint(lenRhs, std::to_string(std::min(CONNECTSIZE, 99))));
		std::string result = andConstraint(andConstraints) + "\n";
		return result;
	}

	/*
	 * Generate constraints for the case
	 * X = T . "abc" . Y . Z
	 * constPos: position of const element
	 * return: (or (and length header = i && X_i = a && X_[i+1] = b && X_[i+2] = c))
	 */
	std::string handle_SubConst_WithPosition_array(
			std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs_str, std::string rhs_str,
			int constPos) {
		assert (elementNames[constPos].second < 0);

		/* regex */
		std::string content = "";
		if (elementNames[constPos].second != REGEX_CODE)
			content = elementNames[constPos].first;
		else
			content = parse_regex_content(elementNames[constPos].first);
//		__debugPrint(logFile, "%d *** %s ***: const pos: %s\n", __LINE__, __FUNCTION__, content.c_str());
		/* find the start position --> */
		std::string startPos = leng_prefix_lhs(a, elementNames, lhs_str, rhs_str, constPos);
		std::string flatArrayName = generateFlatArray(a, lhs_str);

		std::vector<std::string> possibleCases;
		if (elementNames[constPos].second == REGEX_CODE) {
			possibleCases.emplace_back(handle_Regex_WithPosition_array(a, elementNames, lhs_str, rhs_str, constPos));
		}
		else if (elementNames[constPos].second % QCONSTMAX == -1) {
			std::vector<std::string> oneCase;
			for (int i = 1; i <= std::min(LOCALSPLITMAX, (int)content.length()); ++i) {
				std::vector<std::string> locationConstraint;
				/*length = i*/
				locationConstraint.emplace_back(createLessConstraint(generateFlatSize(elementNames[constPos], rhs_str), std::to_string(i)));
				locationConstraint.emplace_back(createEqualConstraint(
						createSelectConstraint(flatArrayName, "(+ " + std::to_string(i - 1) + " " + startPos + ")"),
						std::to_string(content[i - 1])));
				oneCase.emplace_back(orConstraint(locationConstraint));
			}
			possibleCases.emplace_back(andConstraint(oneCase));
		}
		else {
			for (int i = 0; i <= std::min(LOCALSPLITMAX, (int)content.length()); ++i) {
				/*length = i*/
				std::string tmp = createEqualConstraint(generateFlatSize(elementNames[constPos], rhs_str), std::to_string(content.length() - i));
				possibleCases.emplace_back(handle_Const_WithPosition_array(a, elementNames, lhs_str, rhs_str, constPos, content, i, content.length(), tmp));
			}
		}

		std::string result = orConstraint(possibleCases);
		return result;
	}

	/*
	 *
	 */
	std::string unrollConnectedVariable(
			std::pair<std::string, int> a, /* connected variable */
			std::vector<std::pair<std::string, int> > elementNames, /* contain const */
			std::string lhs_str, std::string rhs_str,
			std::map<std::string, int> connectedVariables){
//		__debugPrint(logFile, "%d connected var = const | regex: lhs = %s; rhs = %s \n", __LINE__, lhs_str.c_str(), rhs_str.c_str());

		assert(connectedVariables.find(a.first) != connectedVariables.end());
		/* (and ...) */
		std::vector<std::string> possibleCases;

		for (unsigned int i = 0 ; i < elementNames.size(); ++i){
			if (elementNames[i].second < 0){ /* const || regex */
				/* |lhs| = 1 vs |rhs| = 1*/
				if (elementNames.size() == 1 && QCONSTMAX > 1) {
					possibleCases.emplace_back(handle_SubConst_WithPosition_array(a, elementNames, lhs_str, rhs_str, i));
				}
				else if (elementNames[i].second == REGEX_CODE) {
					possibleCases.emplace_back(handle_SubConst_WithPosition_array(a, elementNames, lhs_str, rhs_str, i));
				}
				/* tail of string, head of elements*/
				else if (i == 0 && elementNames[i].second % QCONSTMAX == 0 && QCONSTMAX > 1) {
					possibleCases.emplace_back(handle_SubConst_WithPosition_array(a, elementNames, lhs_str, rhs_str, i));
				}
				/* head of string, tail of elements */
				else if (i == elementNames.size() - 1 && elementNames[i].second % QCONSTMAX == -1 && QCONSTMAX > 1)  {
					possibleCases.emplace_back(handle_SubConst_WithPosition_array(a, elementNames, lhs_str, rhs_str, i));
				}
				/* only care about first element */
				else if (elementNames[i].second % QCONSTMAX == -1)  {
					possibleCases.emplace_back(handle_Const_WithPosition_array(a, elementNames, lhs_str, rhs_str, i, elementNames[i].first, 0, elementNames[i].first.length()));
				}
			}
			else if (elementNames[i].second >= 0 && connectedVariables.find(elementNames[i].first) != connectedVariables.end()){
				if (elementNames[i].second % QMAX == 1 && i > 0)
					continue;

				possibleCases.emplace_back(handle_connected_connected_array(a, elementNames, lhs_str, rhs_str, i, connectedVariables[elementNames[i].first]));

			}
		}
		return andConstraint(possibleCases);
	}

	/*
	 *
	 * Flat = empty
	 */

	std::string generateConstraint00(
			std::pair<std::string, int> a,
			std::string l_r_hs){

		return createEqualConstraint(ZERO, generateFlatSize(a, l_r_hs));
	}

	/*
	 * Var = Var
	 */
	std::string generateConstraint01_twoVar(
		std::string lhs_str, std::string rhs_str,
		std::pair<std::string, int> a, std::pair<std::string, int> b,
		std::map<std::string, int> connectedVariables){
		__debugPrint(logFile, "%d *** %s ***: %s = %s\n", __LINE__, __FUNCTION__, a.first.c_str(), b.first.c_str());
		bool isConstA = a.second < 0;
		bool isConstB = b.second < 0;

		std::string arrA = generateFlatArray(a, lhs_str);
		std::string arrB = generateFlatArray(b, rhs_str);
		std::string nameA = "";
		std::string nameB = "";
		if (a.second >= 0) {
			nameA += LENPREFIX;
			nameA += a.first;
		}
		else {
			assert (constMap.find(a.first) != constMap.end());
			nameA += LENPREFIX;
			nameA += constMap[a.first];
			if (isRegexStr(a.first))
				nameA += "_100";
		}

		if (b.second >= 0) {
			nameB += LENPREFIX;
			nameB += b.first;
		}
		else {
			assert (constMap.find(b.first) != constMap.end());
			nameB += LENPREFIX;
			nameB += constMap[b.first];
			if (isRegexStr(b.first))
				nameB += "_100";
		}

		std::string result = createEqualConstraint(nameA, nameB);
		result += createEqualConstraint( generateFlatSize(a, lhs_str), generateFlatSize(b, rhs_str));

		if (!isConstA && !isConstB) {
			if (connectedVariables.find(b.first) != connectedVariables.end() &&
					connectedVariables.find(a.first) != connectedVariables.end()){
				int bound = std::max(connectedVariables[b.first], connectedVariables[a.first]);
				int consideredSize = std::min(bound + 1, 99);

				for (int i = 1; i <= consideredSize; ++i){
					std::vector<std::string> orConstraints;
					orConstraints.emplace_back(createEqualConstraint(createSelectConstraint(arrA, std::to_string(i - 1))
											, createSelectConstraint(arrB, std::to_string(i - 1))));
					orConstraints.emplace_back(createLessConstraint(nameA, std::to_string(i)));
					result = result + orConstraint(orConstraints);
				}
			}
		}

		else if (isConstA && isConstB) {
			if (a.first.compare(b.first) != 0)
				return "";
		}

		else if (isConstA) {
			/* record characters for some special variables */
			if (connectedVariables.find(b.first) != connectedVariables.end()){
				int consideredSize = a.first.length();
				result += createEqualConstraint(nameB, std::to_string(consideredSize));
				for (int i = 0; i < consideredSize; ++i){
					result += createEqualConstraint(std::to_string(a.first[i]), createSelectConstraint(arrB, std::to_string(i)));
				}
			}
		}

		else if (isConstB){
			/* record characters for some special variables */
			if (connectedVariables.find(a.first) != connectedVariables.end()){
				int consideredSize = b.first.length();
				result += createEqualConstraint(nameA, std::to_string(consideredSize));
				for (int i = 0; i < consideredSize; ++i){
					result += createEqualConstraint(std::to_string(b.first[i]), createSelectConstraint(arrA, std::to_string(i)));
				}
			}
		}

		__debugPrint(logFile, "%d %s: %s\n", __LINE__, __FUNCTION__, result.c_str());
		return result;
	}

	/*
	 * Flat = Flat
	 * size = size && it = it  ||
	 * size = size && it = 1
	 */
	std::string generateConstraint01(
			std::string lhs_str, std::string rhs_str,
			std::pair<std::string, int> a, std::pair<std::string, int> b,
			int pMax,
			std::map<std::string, int> connectedVariables){
		bool isConstA = a.second < 0;
		bool isConstB = b.second < 0;

		__debugPrint(logFile, "%d %s: %s = %s\n", __LINE__, __FUNCTION__, a.first.c_str(), b.first.c_str());

		std::string nameA = generateFlatSize(a, lhs_str);
		std::string nameB = generateFlatSize(b, rhs_str);

		/* do not need AND */
		std::string result = createEqualConstraint(nameA, nameB);

		if (!isConstA && !isConstB) {
			/* size = size && it = it */
			if (connectedVariables.find(b.first) != connectedVariables.end()){
				std::vector<std::pair<std::string, int>> elements;
				elements.emplace_back(a);
				result = result + " " + unrollConnectedVariable(b, elements, rhs_str, lhs_str, connectedVariables);
			}
		}
		else if (isConstA && isConstB) {
			if (QCONSTMAX == 1 && a.first.compare(b.first) != 0) /* const = const */
				return "";
			std::vector<std::string> possibleCases;

			if (a.second == REGEX_CODE && b.second % QMAX == -1){
				std::string regexContent = parse_regex_full_content(a.first);
				RegEx re;
				re.Compile(regexContent);
				unsigned length = 0;
				if (regexContent[regexContent.length() - 1] == '+')
					length = 1;
				while (length <= b.first.length()) {
					std::string regexValue = b.first.substr(0, length);
					if (re.MatchAll(regexValue) == true) {
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(length)));
					}
					else
						break;
					length++;
					__debugPrint(logFile, "%d accept: %s\n", __LINE__, regexValue.c_str());
				}
			}
			else if (a.second == REGEX_CODE && b.second % QMAX == 0){
				std::string regexContent = parse_regex_full_content(a.first);
				RegEx re;
				re.Compile(regexContent);
				unsigned length = 0;
				if (regexContent[regexContent.length() - 1] == '+')
					length = 1;
				while (length <= b.first.length()) {
					std::string regexValue = b.first.substr(b.first.length() - length, length);
					if (re.MatchAll(regexValue) == true) {
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(length)));
					}
					else
						break;
					length++;
					__debugPrint(logFile, "%d accept: %s\n", __LINE__, regexValue.c_str());
				}
			}
			else if (b.second == REGEX_CODE && a.second % QMAX == -1){
				std::string regexContent = parse_regex_full_content(b.first);
				RegEx re;
				re.Compile(regexContent);
				unsigned length = 0;
				if (regexContent[regexContent.length() - 1] == '+')
					length = 1;
				while (length <= a.first.length()) {
					std::string regexValue = a.first.substr(0, length);
					if (re.MatchAll(regexValue) == true) {
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(length)));
					}
					else
						break;
					length++;
					__debugPrint(logFile, "%d accept: %s\n", __LINE__, regexValue.c_str());
				}
			}
			else if (b.second == REGEX_CODE && a.second % QMAX == 0){
				std::string regexContent = parse_regex_full_content(b.first);
				RegEx re;
				re.Compile(regexContent);
				unsigned length = 0;
				if (regexContent[regexContent.length() - 1] == '+')
					length = 1;
				while (length <= a.first.length()) {
					std::string regexValue = a.first.substr(a.first.length() - length, length);
					if (re.MatchAll(regexValue) == true) {
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(length)));
					}
					else
						break;
					length++;
					__debugPrint(logFile, "%d accept: %s\n", __LINE__, regexValue.c_str());
				}
			}
			else if (a.second == REGEX_CODE && b.second == REGEX_CODE) {
				__debugPrint(logFile, "%d might get error here\n", __LINE__);
				std::string content01 = parse_regex_content(b.first);
				std::string content02 = parse_regex_content(a.first);
				unsigned int lcdLength = lcd(content01.length(), content02.length());

				std::string data01 = "";
				std::string data02 = "";
				while (data01.length() != lcdLength)
					data01 = data01 + content01;
				while (data02.length() != lcdLength)
					data02 = data02 + content02;
				if (data01.compare(data02) == 0) {
					possibleCases.emplace_back(createEqualConstraint(ZERO, createModOperator(nameA, std::to_string(lcdLength))));
				}
				else {
					possibleCases.emplace_back(createEqualConstraint(nameA, ZERO));
				}
			}
			else if (a.second % QMAX == -1 && b.second % QMAX  == -1) /* head vs head */ {
				for (int i = std::min(a.first.length(), b.first.length()); i >= 0; --i) {
					if (a.first.substr(0, i).compare(b.first.substr(0, i)) == 0) {
						/* size can be from 0..i */
						result += createLessEqualConstraint(nameA, std::to_string(i));
						return result;
					}
				}
			}
			else if (a.second  % QMAX == -1 && b.second % QMAX == 0) /* head vs tail */ {
				for (int i = std::min(a.first.length(), b.first.length()); i >= 0; --i) {
					if (a.first.substr(0, i).compare(b.first.substr(b.first.length() - i)) == 0) {
						/* size can be i */
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(i)));
					}
				}
			}
			else if (a.second % QMAX == 0 && b.second % QMAX == -1) /* tail vs head */ {
				for (int i = std::min(a.first.length(), b.first.length()); i >= 0; --i) {
					if (b.first.substr(0, i).compare(a.first.substr(a.first.length() - i)) == 0) {
						/* size can be i */
						possibleCases.emplace_back(createEqualConstraint(nameA, std::to_string(i)));
					}
				}
			}
			else if (a.second % QMAX == 0 && b.second % QMAX == 0) /* tail vs tail */ {
				for (int i = std::min(a.first.length(), b.first.length()); i >= 0; --i) {
					if (a.first.substr(a.first.length() - i, i).compare(b.first.substr(b.first.length() - i, i)) == 0) {
						/* size can be i */
						result += createLessEqualConstraint(nameA, std::to_string(i));
						// printf("%d %s.2 = %s.2 --> %d\n", __LINE__, a.first.c_str(), b.first.c_str(), i);
						return result;
					}
				}
			}
			else {
				assert (false);
			}

			/* create or constraint */
			std::string tmp = orConstraint(possibleCases);
			if (tmp.length() == 0)
				return tmp;
			else
				result = result + " " + tmp;

			return result;
		}

		else if (isConstA) {
			/* size = size && it = 1*/

			/* record characters for some special variables */
			if (connectedVariables.find(b.first) != connectedVariables.end()){
				std::vector<std::pair<std::string, int>> elements;
				elements.emplace_back(a);
				result = result + " " + unrollConnectedVariable(b, elements, rhs_str, lhs_str, connectedVariables);
			}
		}

		else { /* isConstB */
			/* size = size && it = 1*/
			/* record characters for some special variables */
			if (connectedVariables.find(a.first) != connectedVariables.end()){
				std::vector<std::pair<std::string, int>> elements;
				elements.emplace_back(b);
				result = result + " " + unrollConnectedVariable(a, elements, lhs_str, rhs_str, connectedVariables);
			}
		}

		return result;
	}

	/*
	 * Flat = sum (flats)
	 */
	std::string generateConstraint02(
			std::pair<std::string, int> a,
			std::vector<std::pair<std::string, int>> elementNames,
			std::string lhs_str, std::string rhs_str,
			int pMax,
			std::map<std::string, int> connectedVariables){
		__debugPrint(logFile, "%d %s \n", __LINE__, __FUNCTION__);
		std::string result = "";

		if (a.second < 0) { /* const string or regex */
			/* check feasibility */

			if (a.second != REGEX_CODE) {

				int max_lhs = a.first.length();

				int min_rhs = 0;
				for (unsigned i = 0; i < elementNames.size(); ++i) {
					if (elementNames[i].second % QCONSTMAX == -1) {
						if (QCONSTMAX == 2 && i + 1 < elementNames.size() && elementNames[i + 1].second % QCONSTMAX == 0)
							min_rhs += elementNames[i].first.length();
						else if (QCONSTMAX == 1)
							min_rhs += elementNames[i].first.length();
					}
					else if (elementNames[i].second == REGEX_CODE &&
							elementNames[i].first.find('+') != std::string::npos &&
							elementNames[i].first.find('|') == std::string::npos){
						/* regex plus */
						size_t endPos = elementNames[i].first.find(')');
						assert(endPos != std::string::npos);
						min_rhs += endPos - 1;
					}
				}
				if (max_lhs < min_rhs) {
					return "";
				}
			}
			else {
				/* regex */
			}

			/* collect */
			/* only handle the case of splitting const string into two parts*/
			std::vector<std::string> addElements;
			for (unsigned i = 0 ; i < elementNames.size(); ++i)
				addElements.emplace_back(generateFlatSize(elementNames[i], rhs_str));

			result += createEqualConstraint(generateFlatSize(a, lhs_str), addConstraint_full(addElements));

			/*
			 * 0: No const, No connected var
			 * 1: const		No connected var
			 * 2: no const, connected var
			 * 3: have both
			 */
			int splitType = checkTheBestSplitType(elementNames, connectedVariables);

			__debugPrint(logFile, "%d const = sum(flats), splitType = %d\n", __LINE__, splitType);

			if (splitType == 0) {/* rhs only has simple variables */
				/* do not do anything */
			}
			else if (splitType == 1) {
				/* handle const */
				std::vector<std::vector<int>> allPossibleSplits = collectAllPossibleSplits(a, elementNames);
				std::set<std::string> strSplits;
				for (unsigned int i = 0; i < allPossibleSplits.size(); ++i) {
					/* check feasibility */
					strSplits.emplace(fromSplitToLengConstraint_NoConnectedVar(a, elementNames, allPossibleSplits[i], lhs_str, rhs_str));
				}

				if (strSplits.size() > 0)
					result = result + " " + orConstraint(strSplits);
				else
					return "";
			}
			else if (splitType == 2) {
				/* handle connected var */
				result = result + " " + fromSplitToLengConstraint_ConnectedVar(a, elementNames, lhs_str, rhs_str, connectedVariables);
			}
			else {
				/* handle connected var */
				/* handle const */
				std::vector<std::vector<int>> allPossibleSplits = collectAllPossibleSplits(a, elementNames);
				std::set<std::string> strSplits;
				for (unsigned int i = 0; i < allPossibleSplits.size(); ++i) {
					/* check feasibility */
					strSplits.emplace(fromSplitToLengConstraint_havingConnectedVar_andConst(a, elementNames, allPossibleSplits[i], lhs_str, rhs_str, connectedVariables));
				}
				if (strSplits.size() > 0)
					result = result + " " + orConstraint(strSplits);
				else
					return "";
			}
		}

		else {
			/* do not need AND */
			/* result = sum (length) */
			result = result + "(= " + generateFlatSize(a, lhs_str) + " (+ ";
			for (unsigned int i = 0; i < elementNames.size(); ++i) {
				result = result + generateFlatSize(elementNames[i], rhs_str) + " ";
			}

			if (elementNames.size() == 1)
				result = result + " 0";

			result = result + ")) ";

			/* DO NOT care about empty flats or not*/

			/* handle const for connected variables*/
			if (connectedVariables.find(a.first) != connectedVariables.end())
				result = result + unrollConnectedVariable(a, elementNames, lhs_str, rhs_str, connectedVariables);
		}


		return result;
	}

	/*
	 * a_1 + a_2 + b_1 + b_2 = c_1 + c_2 + d_1 + d_2 ---> SMT
	 */
	std::string generateSMT(int pMax,
			std::string lhs_str, std::string rhs_str,
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements,
			std::map<std::string, int> connectedVariables){
		__debugPrint(logFile, "%d %s \n", __LINE__, __FUNCTION__);
		std::vector<std::string> result_element;

		bool checkLeft[lhs_elements.size()];
		bool checkRight[rhs_elements.size()];
		memset(checkLeft, 0, sizeof checkLeft);
		memset(checkRight, 0, sizeof checkRight);

		/* do the left */
		for (unsigned int i = 0; i < left_arr.size(); ++i)
			if (left_arr[i] == SUMFLAT) { /* a = bx + cy */

				checkLeft[i] = true;

				std::vector<std::pair<std::string, int>> elements;
				for (unsigned int j = 0; j < right_arr.size(); ++j)
					if (right_arr[j] == (int)i) {
						elements.emplace_back(rhs_elements[j]);
						checkRight[j] = true;
					}

				std::string tmp = generateConstraint02(lhs_elements[i], elements, lhs_str, rhs_str, pMax, connectedVariables);

				if (tmp.length() == 0) { /* cannot happen due to const */
					__debugPrint(logFile, "\n%d 02 because of lhs[%d]\n", __LINE__, i);
					return "";
				}
				result_element.emplace_back(tmp);

			}
			else if (left_arr[i] == EMPTYFLAT) {

				/* empty */
				/* some first flats can be empty */
				if (lhs_elements[i].second % QCONSTMAX == -1) /* head of const */ {
					if (lhs_elements[i].first.length() <= 0 ||
						(QCONSTMAX == 2 &&
							i + 1 < lhs_elements.size() &&
							left_arr[i + 1] == EMPTYFLAT &&
							lhs_elements[i].first.length() > 0) ||
						(QCONSTMAX == 1 &&
							lhs_elements[i].first.length() > 0)) /* const string is empty */ {
						return "";
					}
				}
				checkLeft[i] = true;
				std::string tmp = generateConstraint00(lhs_elements[i], lhs_str);

				if (tmp.length() == 0) {/* cannot happen due to const */
					return "";
				}
				result_element.emplace_back(tmp);
			}

		/* do the right */
		for (unsigned int i = 0; i < right_arr.size(); ++i)
			if (right_arr[i] == SUMFLAT) { /* a = bx + cy */
				checkRight[i] = true;

				std::vector<std::pair<std::string, int>> elements;
				for (unsigned int j = 0; j < left_arr.size(); ++j)
					if (left_arr[j] == (int)i) {
						elements.emplace_back(lhs_elements[j]);
						checkLeft[j] = true;
					}

				std::string tmp = generateConstraint02(rhs_elements[i], elements, rhs_str, lhs_str, pMax, connectedVariables);
				if (tmp.length() == 0) { /* cannot happen due to const */
					__debugPrint(logFile, "\n%d 02 because of rhs[%d]\n", __LINE__, i);
					return "";
				}
				result_element.emplace_back(tmp);
			}
			else if (right_arr[i] == EMPTYFLAT) {
				/* empty */
				/* some first flats can be empty */
				if (rhs_elements[i].second % QCONSTMAX == -1) /* head of const */ {
					if (rhs_elements[i].first.length() <= SPLIT_LOWER_BOUND - 2 ||
						(QCONSTMAX == 2 &&
							i + 1 < rhs_elements.size() &&
							right_arr[i + 1] == EMPTYFLAT &&
							rhs_elements[i].first.length() > 0) ||
						(QCONSTMAX == 1 &&
							rhs_elements[i].first.length() > 0))/*const string is empty*/
						return "";
				}
				checkRight[i] = true;
				std::string tmp = generateConstraint00(rhs_elements[i], rhs_str);
				if (tmp.length() == 0) {/* cannot happen due to const */
					return "";
				}
				result_element.emplace_back(tmp);
			}

		/* do the rest */
		/* do not need AND */
		std::string constraint01 = "";
		for (unsigned int i = 0 ; i < lhs_elements.size(); ++i)
			if (checkLeft[i] == false) {
				checkLeft[i] = true;
				checkRight[left_arr[i]] = true;

				if (QCONSTMAX == 2){
					/* a1 = b1 && a2 == b2 --> a = b */
					if (checkLeft[i + 1] == false &&
							((lhs_elements[i].second >= 0 && lhs_elements[i].second % QMAX == 0) || (lhs_elements[i].second < 0 && lhs_elements[i].second % QCONSTMAX == -1)) &&
							i + 1 < lhs_elements.size()) {
						if ((rhs_elements[left_arr[i]].second < 0 &&
								rhs_elements[left_arr[i]].second % QCONSTMAX == -1 &&
								rhs_elements[left_arr[i + 1]].second % QCONSTMAX == 0) ||
								(rhs_elements[left_arr[i]].second >= 0 &&
										rhs_elements[left_arr[i]].second % QMAX == 0 &&
										rhs_elements[left_arr[i + 1]].second % QMAX == 1)){
							std::string tmp = generateConstraint01_twoVar(lhs_str, rhs_str, lhs_elements[i], (std::pair<std::string, int>)rhs_elements[left_arr[i]], connectedVariables);
							if (tmp.length() > 0)
								constraint01 = constraint01 + tmp + " ";
							else
								return "";
							checkLeft[i + 1] = true;
							checkRight[left_arr[i + 1]] = true;
							i++;
							continue;
						}
					}
				}
				else
					assert(false);

				std::string tmp = generateConstraint01(lhs_str, rhs_str, lhs_elements[i], (std::pair<std::string, int>)rhs_elements[left_arr[i]], pMax, connectedVariables);
				if (tmp.length() == 0) { /* cannot happen due to const */
					return "";
				}
				constraint01 = constraint01 + tmp + " ";
			}

		if (constraint01.length() > 5) {
			result_element.emplace_back(constraint01);

		}

		for (unsigned int i = 0 ; i < rhs_elements.size(); ++i)
			if (checkRight[i] == false)
				assert (false);

		/* sum up */
		std::string result = "(and \n";
		for (unsigned int i = 0 ; i < result_element.size(); ++i)
			result = result + result_element[i] + "\n";
		result = result + ")";

		return result;
	}

	/*
	 * we do not allow empty flats in the middle
	 */
	bool isPossibleArrangement(){
		if (left_arr[left_arr.size() -1] == EMPTYFLAT &&
				right_arr[right_arr.size() -1] == EMPTYFLAT)
			return false;
		/* not allow empty flats in the middle */
		unsigned int startPos = 0;
		unsigned int endPos = left_arr.size() - 1;
		/* check lhs */
		for (startPos = 0; startPos < left_arr.size(); ++startPos)
			if (left_arr[startPos] != EMPTYFLAT)
				break;
		for (endPos = left_arr.size() - 1; endPos >= 0; --endPos)
			if (left_arr[endPos] != EMPTYFLAT)
				break;
		for (unsigned int i = startPos; i < endPos; ++i)
			if (left_arr[i] == EMPTYFLAT) {
				// printArrangement("ERRORR");
				return false;
			}

		/* check rhs */
		for (startPos = 0; startPos < right_arr.size(); ++startPos)
			if (right_arr[startPos] != EMPTYFLAT)
				break;
		for (endPos = right_arr.size() - 1; endPos >= 0; --endPos)
			if (right_arr[endPos] != EMPTYFLAT)
				break;
		for (unsigned int i = startPos; i < endPos; ++i)
			if (right_arr[i] == EMPTYFLAT) {
				// printArrangement("ERRORR");
				return false;
			}
		return true;
	}

	/*
	 * Pre-Condition: x_i == 0 --> x_i+1 == 0
	 */
	bool isPossibleArrangement(
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements){
		/* bla bla */
		for (unsigned int i = 0; i < left_arr.size(); ++i)
			if (left_arr[i] != -1){
				for (int j = i - 1; j >= 0; --j){
					if (lhs_elements[j].second < lhs_elements[i].second) { /* same var */
						if (left_arr[j] == -1)
							return false;
					}
					else
						break;
				}
			}
		for (unsigned int i = 0; i < right_arr.size(); ++i)
			if (right_arr[i] != -1){
				for (int j = i - 1; j >= 0; --j){
					if (rhs_elements[j].second < rhs_elements[i].second) { /* same var */
						if (right_arr[j] == -1)
							return false;
					}
					else
						break;
				}
			}
		return true;
	}


	void printArrangement(std::string msg = ""){
		if (msg.length() > 0)
			__debugPrint(logFile, "%s\n", msg.c_str());

		for (unsigned int i = 0; i < left_arr.size(); ++i)
			__debugPrint(logFile, "%*d ", 3, left_arr[i]);
		__debugPrint(logFile, "\n");
		for (unsigned int i = 0; i < right_arr.size(); ++i)
			__debugPrint(logFile, "%*d ", 3, right_arr[i]);
		__debugPrint(logFile, "\n");
	}
};


#endif /* ARRANGEMENT_H_ */
