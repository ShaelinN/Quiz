#pragma once
#include <string>
#include "Connection.h"
#include <set>
#include <stdlib.h>
#include<time.h>
#include <random>

using namespace std;

class Quiz
{
private:
	set<Connection::Question> questions;
	set<Connection::Question>::iterator questionIter;

	int* length;
	
	Connection* connection;

	int* progress;
	int* score;


public:
	Quiz(const string& filePath, int length = 10);

	int getScore(); //{ return *score; }
	int getLength(); //{ return *length; }
	int getProgress();// { return *progress; }
	~Quiz();

	//increments the current question
	Connection::Question nextQuestion();

	//determine if the answer index the user provided is the same as the answer index outlined by the current question structure
	bool acceptUserAnswer(int answer, const Connection::Question& question);


	//for debugging
	void printAllQuestions() {
		cout << "printing questions for debugging" << endl;
		for (Connection::Question q: questions)
		{
			cout << "next question" << endl;
			cout << q << endl;
		}
	}
private:
	//using a single quiz file (which holds a single quiz), initialise this quiz to be a subset of the quiz in the file
	void generateSingleSourcedQuizSubSet(const string& sourceFilePath);

};

