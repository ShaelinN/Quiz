#pragma once
#include <string>
#include <vector>
#include "QuizXMLFile.h"

#include <algorithm>
#include <random>
#include <stdlib.h>
#include<time.h>
#include<chrono>



using namespace std;
//A user friendly way to access a quiz xml file. 
//Serves as an way to access the file whose path it is given during construction and use it to draw up a quiz
class Connection
{
private:
	QuizXMLFile* currentQuizFile;
public:
	//consistency check to ensure all quizzes accessed through this Connection have a standardised number of answers per question
	static const int MAX_NO_ANSWERS = 4;
	//Question structure for dependent programs to use

	struct Question
	{
		int qID=-1;
		string questionBody;
		vector<string> answerList;
		int correctAnswerIndex = 0;

		//shuffle asnwers
		void shuffleAnswers();


		//operator overloads
		//to compare Question to other questions
		bool operator<(const Question& otherQuestion) const;

		//to send question to output stream
		friend ostream& operator<<(ostream& os, const Question& question);



	};

public:
	//constructor
	Connection(const string& filePath);
	//destructor
	~Connection();
	int getQuestionSourceSize();

public://conceptually all of these should be public. in practice, only getPrepared question needs to be used outside this class
	//searches a quizXML file for the question with the property "id" equal to questionID
	string getQuestionTextForIndex(int questionID);

	//searches a quizXML file for the answers with property "type"="wrong",within a container with the property "id" equal to questionID
	vector<string> getAnswersForIndex(int questionID, string wrongright);

	//searches a quizXML file for the answers with property "type"="correct" within a container with the property "id" equal to questionID
	//assumes there is only one correct answer for the question
	//string getCorrectAnswerForIndex(int questionID);

	//handles the preparation steps so caller doesn't need to call multiple functions to get a question from the datasource
	//takes a questionID, prepares a Connection::Question struct from it and returns the prepared Question struct
	Question getPreparedQuestionStruct(int questionID);



private://aux funtions
	//encapsulates the task of finding the initial line of a question element in a quiz xml file
	//used to find both correct and incorrect answers
	int gotoLineOfQuestion(int questionID);




//
//for writing new quizzes
//
public:
	//takes a list of question structs, along with a topic, and creates a .xml file out of them in the given directory
	static void questionVectorToQuizFile(const vector<Question>& questions, const string& topic, const string& directory, QuizXMLFile& list);
private:
	static void recordInList(const string& topic,int length, const string& filepath, QuizXMLFile& list);
};

