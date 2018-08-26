/*
 * Config.h
 *
 *  Created on: Aug 26, 2018
 *      Author: admin
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "Utils.h"
#include "GrmUnderApprox.h"

class Config{
public:
	std::string orgInput;
	std::string inputFile;
	std::string inputFile_converted;
	bool getModel = false;
	bool beReviewed = false;
	bool cleanLog = true;
	bool printingConstraints = false;
	bool skipOverapprox;
	char escapeChar = ESCAPECHAR20;
	int languageVersion = 20;
	bool aggresiveBoolRefinement = true;
	bool prioritySearch = false;
	bool lazySearch = false;
	std::string verifyingSolver = "";
	std::string trauVersion = "1.2";
	bool writeLog = false;

	std::map<int, std::string> languageMap = languageMap20;
	std::map<std::string, std::vector<std::string>> ourGrm;

	const std::vector<std::string> supportedLanguage = {"SMT2.0", "SMT2.5", "SMT2.6", "2.6","2.0", "2.5", "smt2.6","smt2.0", "smt2.5", "20", "25", "26"};
	const std::vector<std::string> availableSolvers = {"cvc4", "z3str3", "s3"};

	Config (){

	}

	void parse(int argc, char* argv[]){
		std::string grammarFile = "";

		#ifdef DEBUGLOG
			logFile = fopen(LOGFILE, "w");
			logAxiom = fopen(LOGAXIOMFILE, "w");
		#endif

			bool foundGrm = false;
			for (int i = 1; i < argc; ++i) {
				std::string tmp = std::string(argv[i]);
				if (tmp.compare("-grm") == 0){
					i++;
					grammarFile = argv[i];
					foundGrm = true;
				}
				else if (tmp.compare("-language") == 0 || tmp.compare("-lang") == 0){
					i++;
					std::string language = "";
					if (std::find(supportedLanguage.begin(), supportedLanguage.end(), std::string(argv[i])) != supportedLanguage.end()) {
						language = argv[i];
					}
					if (language.length() == 0) {
						printf("Input language is not supported.\nTrau supports ");
						for (const auto& s: supportedLanguage)
							printf("%s ", s.c_str());
						printf("!\n");
						throw std::runtime_error("Cannot recognize the input language!");
					}
					else if (language.find("2.6") != std::string::npos || language.find("26") != std::string::npos) {
						languageMap = languageMap26;
						languageVersion = 26;
						escapeChar = ESCAPECHAR26;
						if (verifyingSolver.length() == 0)
							verifyingSolver = "z3str3";
					}
					else if (language.find("2.5") != std::string::npos || language.find("25") != std::string::npos) {
						languageMap = languageMap25;
						languageVersion = 25;
						escapeChar = ESCAPECHAR25;
						if (verifyingSolver.length() == 0)
							verifyingSolver = "cvc4";
					}
					else if (language.find("2.0") != std::string::npos || language.find("20") != std::string::npos) {
						languageMap = languageMap20;
						languageVersion = 20;
						escapeChar = ESCAPECHAR20;
						if (verifyingSolver.length() == 0)
							verifyingSolver = "s3";
					}
				}
				else if (tmp.compare("-doublecheck") == 0 || tmp.compare("-crosscheck") == 0){
					getModel = true;
					beReviewed = true;

					if (i + 1 < argc){
						std::string checker = std::string(argv[i + 1]);
						if (checker[0] != '-') {
							if (std::find(availableSolvers.begin(), availableSolvers.end(), checker) != availableSolvers.end()) {
								verifyingSolver = argv[i + 1];
								i++;
							}
						}
					}
				}
				else if (tmp.compare("-model") == 0){
					getModel = true;
				}
				else if (tmp.compare("-noAgressiveRefinement") == 0){
					aggresiveBoolRefinement = false;
				}
				else if (tmp.compare("-prioritySearch") == 0){
					prioritySearch = true;
				}
				else if (tmp.compare("-lazy") == 0) {
					lazySearch = true;
				}
				else if (tmp.compare("-noCleanLog") == 0){
					cleanLog = false;
				}
				else if (tmp.compare("-log") == 0){
					writeLog = true;
				}
				else if (tmp.compare("-help") == 0){
					printHelp();
					return;
				}
				else if (tmp.compare("-printConstraints") == 0){
					printingConstraints = true;
				}
				else if (tmp[0] == '-') {
					printHelp();
					inputFile = "";
					orgInput = "";
					throw std::runtime_error("Arguments Errors.\n");
					return;
				}
				else {
					inputFile = argv[i];
					orgInput = inputFile;
				}
			}

			if (verifyingSolver.length() == 0)
				verifyingSolver = "s3";

			if (foundGrm == true){
				loadGrammar(grammarFile);
			}

			if (inputFile.length() == 0)
				std::cerr << "Specify the input file!\n";

		#ifdef DEBUGLOG
			__debugPrint(logFile, "Input file: %s\n\n", inputFile.c_str());
			if (foundGrm)
				__debugPrint(logFile, "Grammar file: %s\n\n", grammarFile.c_str());
		#endif
	}

	void loadGrammar(std::string grammarFile) {
		__debugPrint(logFile, "%d *** %s ***\n", __LINE__, __FUNCTION__);

	#if 0
		ourGrm = OverApproxCFG::overapprox_CFG(grammarFile);
	#else
		ourGrm = UnderApproxCFG::underapprox_CFG(grammarFile);
	#endif
		for (const auto& s : ourGrm) {
			__debugPrint(logFile, "%d %s: \n", __LINE__, s.first.c_str());
			for (const auto& ss : s.second)
				__debugPrint(logFile, "%d\t%s\n", __LINE__, ss.c_str());
		}
	}

	/**
	 *
	 */
	void printHelp(){
		printf("Trau string solver [version %s]\n", trauVersion.c_str());
		printf("Usage ./Trau [option] file.smt2\n\n");

		printf("Input format:\n");
		printf("\t -model 	\t\t print satisfying assignments if the result is SAT.\n");
		printf("\t -language [..]	\t\t define the input language. Trau supports ");
		for (const auto& s: supportedLanguage)
			printf("%s ", s.c_str());
		printf("!\n");
		printf("\t -grm [..]\t\t\t specify the grammar file.\n");
		printf("\t -doublecheck 	\t\t use the S3P string solver to double-check satisfying assignments.\n");

		printf("\n");
	}
};



#endif /* CONFIG_H_ */
