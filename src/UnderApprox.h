/*
 * UnderApprox.h
 *
 *  Created on: Jan 31, 2017
 *      Author: diepbp
 */

#ifndef UNDERAPPROX_H_
#define UNDERAPPROX_H_

#include "utils.h"
#include "Arrangement.h"
#include "FileConverter.h"

#define OUTPUT "/tmp/fat_str_convert/output.smt2"
#define NONGRM "/tmp/fat_str_convert/_nogrm.smt2"

#define ALLVAR 0

extern std::string orgInput;
extern bool getModel;
static pthread_mutex_t smt_mutex;
static std::map<std::string, std::vector<std::vector<std::string>>> equalitiesMap;
static std::map<std::string, int> global_smtVars;
static std::vector<std::vector<std::string>> global_smtStatements;

#ifdef ALLVAR
static std::set<std::string> allVariables;
#endif

static int regexCnt;
static std::vector<std::string> smtVarDefinition;
static std::vector<std::string> smtLenConstraints;

static std::vector<std::string> repeatedVariableConstraints;
static std::set<std::string> connectedVariables;

//static std::map<std::pair<std::string, std::string>, std::string> constMap;
static std::map<std::string, std::string> constMap;
static std::map<std::string, std::set<std::string>> notMap;
static std::vector<std::string> notConstraints;
static std::map<std::string, int> varLength;
static std::map<std::pair<int, int>, std::vector<Arrangment>> arrangements;

static int noFlatVariables = 0;

void updatePossibleArrangements(
		std::vector<std::pair<std::string, int>> lhs_elements,
		std::vector<std::pair<std::string, int>> rhs_elements,
		std::vector<Arrangment> tmp,
		std::vector<Arrangment> &possibleCases);

void updatePossibleArrangements(
		std::vector<Arrangment> tmp,
		std::vector<Arrangment> &possibleCases);

/*
 *
 */
bool isRegexStr(std::string str);

	/*
	 * First base case
	 */
	void handleCase_0_0(
			bool usingFilter = true);

	void handleCase_0_0_general();
	/*
	 * 2nd base case (sum lhs...) = [0]
	 */
	void handleCase_n_0(
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements,
			bool usingFilter = true);

	void handleCase_n_0_general(
				std::vector<std::pair<std::string, int>> lhs_elements,
				std::vector<std::pair<std::string, int>> rhs_elements);
	/*
	 * 2nd base case [0] = (sum rhs...)
	 */
	void handleCase_0_n(
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements,
			bool usingFilter = true);

	void handleCase_0_n_general(
				std::vector<std::pair<std::string, int>> lhs_elements,
				std::vector<std::pair<std::string, int>> rhs_elements);

	/*
	 * general case
	 */
	void handleCase_n_n(
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements,
			bool usingFilter = true);

	void handleCase_n_n_general(
				std::vector<std::pair<std::string, int>> lhs_elements,
				std::vector<std::pair<std::string, int>> rhs_elements);

	/*
	 * lhs: size of the lhs
	 * rhs: size of the rhs
	 * lhs_elements: elements of lhs
	 * rhs_elements: elements of rhs
	 *
	 * Pre-Condition: x_i == 0 --> x_i+1 == 0
	 *
	 */
	std::vector<std::string> collectAllPossibleArrangements(
			std::string lhs_str, std::string rhs_str,
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements,
			std::map<std::string, int> &newVars);

	/*
	 * startswith a b
	 * startwith "a" b
	 * startwith a "b"
	 */
	void create_constraints_StartsWith(
			std::string str00,
			std::string str01,
			std::vector<std::string> &constraints);

	/*
	 * define array for connected variable
	 */
	void create_constraints_array(std::vector<std::string> &defines, std::vector<std::string> &constraints);

	/*
	 * define array for connected variable
	 */
	void create_const_array(std::vector<std::string> &defines, std::vector<std::string> &constraints);

	/*
	 *
	 * (declare-const __regex_x Int)
	 *
	 * __regex_x >= 0
	 *
	 */
	void create_constraints_RegexCnt(std::vector<std::string> &defines, std::vector<std::string> &constraints);

	/*
	 * var name -> define var
	 * (declare-const len_const_0_0 Int)
	 * (declare-const len_const_1_0 Int)
	 *
	 * const = sum(len_const_0_0)
	 * len_const_0_0 >= 0
	 *
	 */
	void create_constraints_const(std::vector<std::string> &defines, std::vector<std::string> &constraints);

	/*
	 * var name -> define var
	 * (declare-const len_var_0 Int)
	 * (declare-const len_var_0 Int)
	 *
	 * len_var = sum(len_var_0)
	 * len_var_0 >= 0
	 *
	 */
	void create_constraints_strVar(std::vector<std::string> &defines, std::vector<std::string> &constraints);

	/*
	 * len_const = (a * 1) + (b * 2) + ...
	 */
	void create_constraints_regex(std::vector<std::string> &defines, std::vector<std::string> &constraints, std::string regex, std::string name);

	/*
	 * len_x = sum(len_y)
	 */
	std::string createLengthConstraintForAssignment(std::string x, std::vector<std::string> components);


	/*
	 * print to screen
	 */
	void printSMT();

	/*
	 * write to file output
	 */
	void writeOutput02(std::string outFile);

	void writeOutput_basic(std::string outFile);

	/*
	 * write to file output
	 */
	void writeLengthOutput(std::string outFile,
			std::map<std::string, std::vector<std::vector<std::string>>> _equalMap,
			std::map<std::string, std::string> lengthResultMap);



	/*
	 * convert lhs == rhs to SMT formula
	 */
	std::pair<std::vector<std::string>, std::map<std::string, int>> equalityToSMT(
			std::string lhs, std::string rhs,
			std::vector<std::pair<std::string, int>> lhs_elements,
			std::vector<std::pair<std::string, int>> rhs_elements);

	/*
	 * print input
	 */
	void printEqualMap(std::map<std::string, std::vector<std::vector<std::string>>> equalMap);


	/*
	 * AutomataDef to const
	 */
	std::string collectConst(std::string str);

	std::string generate_simple_constraint(std::vector<std::vector<std::string>> eqVector);

	/*
	 * read equality file
	 * save data to equalMap
	 * save all variables to allVariables
	 */
	void readFileEquality(std::string inputFile);

	/**
	 * collect all variables we need to handle
	 */
	void parseEqualityMap(std::map<std::string, std::vector<std::vector<std::string>>> _equalMap);

	/*
	 * Create a general value that the component belongs to
	 */
	std::string sumStringVector(std::vector<std::string> list);

	/*
	 * (not (= T4_2 "LLutmaC169413169."))
	 */
	void extractNotConstraints();

	/*
	 * sum const strings
	 * "a" . "b" = "ab"
	 */
	void sumConstString();


	/*
	 * Assign a name for each const string,
	 * We need that name when we split const
	 */
	void createConstMap();

	/*
	 * Find connected variables
	 * They are variables that are used by more than one variables
	 */

	void collectConnectedVariables();

	/*
	 * Remove all equalities without connected variables and consts
	 */

	void refineEqualMap();

	/*
	 *
	 */
	std::vector<std::string> collectAlternativeComponents(std::string str);

	/*
	 *
	 */
	std::string underApproxRegex(std::string str);

	/*
	 * (a) --> a
	 */
	void removeExtraParentheses(std::string &s);

	/*
	 * (a)|(b) --> a|b
	 */
	std::set<std::string> extendComponent(std::string s);

	/**
	 * (abc|cde|ghi)*
	 */
	void optimizeFlatAutomaton(std::string &s);

	/*
	 *
	 */
	std::vector<std::vector<std::string>> parseRegexComponents(std::string str);

	/*
	 *
	 */
	bool equalVector(std::vector<std::string> a, std::vector<std::string> b);

	/*
	 *
	 */
	std::vector<std::vector<std::string>> refineVectors(std::vector<std::vector<std::string>> list);

	/*
	 * a b c (abc)* --> abc (abc)*
	 */
	std::vector<std::vector<std::string>> combineConstStr(std::vector<std::vector<std::string>> regexElements);

	/*
	 * Input: x . y
	 * Output: flat . flat . flat . flat . flat . flat
	 */
	std::vector<std::pair<std::string, int>> createEquality(std::vector<std::string> list);

	std::vector<std::string> createSetOfFlatVariables(int flatP);

	/*
	 * Pthread
	 * Each thread handles a part in the global map from start -> end
	 */
	void *convertEqualities(void *tid);

	/*
	 * Just do sequential to compare performance
	 */
	void testSequential();

	/*
	 *
	 */
	void testEqualityToSMT();

	/*
	 *
	 */
	bool Z3_run(std::map<std::string, std::vector<std::vector<std::string>>> _equalMap, bool finalCall);

	/*
	 *
	 */
	bool S3_assist(std::string fileName);

	/*
	 * Pthread Caller
	 */
	void pthreadController();

	void init();

	bool underapproxController(
			std::map<std::string, std::vector<std::vector<std::string>>> _equalMap,
			std::map<std::string, bool> _containStrMap,
			std::map<std::string, std::string> rewriterStrMap,
			std::map<std::string, int> _currentLength,
			std::string fileDir);

#endif /* UNDERAPPROX_H_ */
