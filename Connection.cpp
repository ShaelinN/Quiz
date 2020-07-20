#include "Connection.h"

/*
________________________________________________________________________________________
METHODS FOR THE CONNECTION CLASS
________________________________________________________________________________________
*/
Connection::Connection(const string& filePath)
{
	currentQuizFile = new QuizXMLFile(filePath);

	//debugging: status: Connection Objects successfully return question structs 
	//Question q = getPreparedQuestionStruct(4);
	//cout << q.questionBody << endl << q.answerList[0] << endl << q.answerList[1] << endl << q.answerList[2] << endl << q.answerList[3] << endl << q.correctAnswerIndex << endl;

}

Connection::~Connection()
{
	delete  currentQuizFile;
}

int Connection::getQuestionSourceSize()
{

	return stoi(currentQuizFile->getProp(1,"length"));
}

string Connection::getQuestionTextForIndex(int questionID)
{
	
	for (int i = 0; i < currentQuizFile->getQuizXMLDocLength(); i++)
	{
		//if the current line is a question
		if	(currentQuizFile->getProp(i, "?tag") == "question")
		{
			//if the question has id = index
			if (stoi(currentQuizFile->getProp(i, "id")) == questionID)
			{
				return currentQuizFile->getProp(i, "text");
			}
		}
			
	}
	//cout << "!nosuchquestionindex";
	return string("!nosuchquestionindex");
}

vector<string> Connection::getAnswersForIndex(int questionID, string wrongcorrect)
{
	vector<string> answers;
	if (wrongcorrect != "correct") { wrongcorrect = "wrong"; }
	//navigate to the question
	int questionLineIndex = gotoLineOfQuestion(questionID);
	
	if (questionLineIndex == -1) //!nosuchquestionindex
	{
		cout << "!nosuchquestionindex"<<endl;
	}

	else 	//question exists, find answers
	{
		int* j = new int(currentQuizFile->getEndOfContainerIndex(questionLineIndex));
		for (int i = questionLineIndex; i < *j ; i++)
		{
			//name of the container on this line is answer
			if (currentQuizFile->getProp(i, "?tag") == "answer")
			{
				//type of answer is "wrong"
				if (currentQuizFile->getProp(i, "type") == wrongcorrect)
				{
					//if the question holding the answer
					answers.push_back(currentQuizFile->getProp(i, "text"));
				}
			}
		}
		delete j;
	}


	return answers;

}
/*DEPRECATED

string Connection::getCorrectAnswerForIndex(int questionID)
{
	string correctAnswer;

	//navigate to the question
	int questionLineIndex = gotoLineOfQuestion(questionID);
	if (questionLineIndex == -1) //!nosuchquestionindex
	{
		cout << "!nosuchquestionindex" << endl;
		return "error: nosuchquestionindex";

	}
	else
	{
		int* j = new int(currentQuizFile->getEndOfContainerIndex(questionLineIndex));
		for (int i = questionLineIndex; i < *j; i++)
		{
			//name of the container on this line is answer
			if (currentQuizFile->getProp(i, "?tag") == "answer")
			{
				//type of answer is "wrong"
				if (currentQuizFile->getProp(i, "type") == "correct")
				{
					//if the question holding the answer
					return currentQuizFile->getProp(i, "text");
				}
			}
		}
		delete j;
	}

	return "error";
}

*/

Connection::Question Connection::getPreparedQuestionStruct(int questionID)
{
	struct Question question;

	//populate question struct with data based on index
	question.qID = questionID;
	question.questionBody= getQuestionTextForIndex(questionID);
	question.answerList = getAnswersForIndex(questionID,"wrong");
	string correctAnswer = getAnswersForIndex(questionID,"correct")[0];
	question.answerList.push_back(correctAnswer);
	question.correctAnswerIndex = 3;
	question.shuffleAnswers();

	return question;
}

int Connection::gotoLineOfQuestion(int questionID)
{
	//navigate to the question
	int questionLineIndex = -1;
	for (int i = 0; i < currentQuizFile->getQuizXMLDocLength(); i++)
	{
		if (currentQuizFile->getProp(i, "?tag") == "question")
		{
			if (stoi(currentQuizFile->getProp(i, "id")) == questionID)
			{
				questionLineIndex = i;
				break;
			}
		}
	}
	return questionLineIndex;
}



//setting new quiz



void Connection::questionVectorToQuizFile(const vector<Question>& questions, const string& topic, const string& directory, QuizXMLFile& list)
{
	vector<string>* lines = new vector<string>();
	lines->push_back("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	lines->push_back("<quiz topic=\"" + topic + "\" length=\"" + to_string(questions.size()) + "\">");

	for (size_t i = 0; i < questions.size(); i++)
	{
		lines->push_back("<question id=\"" + to_string(i + 1) + "\" text=\"" + questions[i].questionBody + "\">");
		lines->push_back("<answer type=\"correct\" text=\"" + questions[i].answerList[questions[i].correctAnswerIndex] + "\"/>");
		for (int j = 1; j < MAX_NO_ANSWERS; j++)
		{
			lines->push_back("<answer type=\"wrong\" text=\"" + questions[i].answerList[j] + "\"/>");

		}
		lines->push_back("</question>");

	}
	lines->push_back("</quiz>");
	QuizXMLFile::writeLinesToFile(*lines, directory + topic + ".xml");
	recordInList(topic, questions.size(), (directory + topic)+".xml",list);
	delete lines;
}

void Connection::recordInList(const string& topic, int length, const string& filepath,QuizXMLFile& list)
{
	vector<string> listLines = list.getLines();
	listLines.pop_back();//remove the root container close tag
	//add the new line
	listLines.push_back("<quiz topic=\"" + topic + "\" length =\"" + to_string(length) + "\" filepath=\"" + filepath + "\"/>");

	//re-add close tag
	listLines.push_back("</listFile>");

	QuizXMLFile::writeLinesToFile(listLines, list.getFilePath(), false);
}











/*
________________________________________________________________________________________
METHODS FOR THE QUESTION STRUCTURE
________________________________________________________________________________________
*/

void Connection::Question::shuffleAnswers()
{
	string* tempCorrect = new string(answerList[correctAnswerIndex]);// keep track of the last element i.e the correct answer
	//shuffle
	int* randomSeed = new int(rand()%MAX_NO_ANSWERS);
	shuffle(answerList.begin(), answerList.end(), default_random_engine(*randomSeed));

	//find the correct answer
	for (int i = 0; i < answerList.size(); i++)
	{
		if (answerList[i] == *tempCorrect)
		{
			correctAnswerIndex = i;
			break;
		}
	}
	delete tempCorrect;
	delete randomSeed;
}


//operator overloads

/*NOT NEEDED IN CUURENT VERSION
bool Connection::Question::operator==(const Question& otherQuestion) const {
	//define equality as having the same qID

	return(qID == otherQuestion.qID);
}
bool Connection::Question::operator==(int otherQID) const {
	//define equality as having the same qID
	return(qID == otherQID);
}
*/


bool Connection::Question::operator<(const Question& otherQuestion) const {
	return(qID < otherQuestion.qID);
}


ostream& operator<<(ostream& os, const Connection::Question& question)
{
	os << "QUESTION: " << "#" + to_string(question.qID) + ":" << question.questionBody << endl;

	os << "ANSWERS:" << endl;
	for (size_t i = 0; i < question.answerList.size(); i++)
	{
		os << "#" + to_string(i) + ": " << question.answerList[i] << endl;
	}

	os << "correct answer is #" << question.correctAnswerIndex << endl;
	return os;
}
