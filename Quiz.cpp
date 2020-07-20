#include "Quiz.h"

Quiz::Quiz(const string& filePath, int length)
{

	this->length = new int(length);


	this->score = new int(0);
	this->progress = new int(0);


	generateSingleSourcedQuizSubSet(filePath);

}

Quiz::~Quiz()
{
	delete length;
	delete progress;
	delete score;
}

int Quiz::getScore() { return *score; }
int Quiz::getLength() { return *length; }
int Quiz::getProgress() { return *progress; }

void Quiz::generateSingleSourcedQuizSubSet(const string& sourceFilePath)
{
	connection = new Connection(sourceFilePath);

	set<int>* usedIDs = new set<int>();

	int* randomID = new int(0);
	
	while (usedIDs->size() < *length)
	{
		
		*randomID = 1+ rand() % (connection->getQuestionSourceSize());// 1 to source length

		if (usedIDs->count(*randomID) == 0) 
		{
			Connection::Question question = connection->getPreparedQuestionStruct(*randomID);
			questions.insert(question);
			usedIDs->insert(*randomID);
		}

	}

	delete randomID;
	delete usedIDs;
	delete connection;

	questionIter = questions.begin();
}

Connection::Question Quiz::nextQuestion()
{	
	advance(questionIter,1 );
	(*progress)++;
	//cout << *progress << endl;
	if (questionIter==questions.end())
	{
		questionIter = questions.begin();
	}
	return *questionIter;
}

bool Quiz::acceptUserAnswer(int answer, const Connection::Question& question)
{
	//debugging:
	//cout << "expected: " << question.correctAnswerIndex << endl;
	//cout << "given: " << answer << endl;

	bool isCorrectAnswer = question.correctAnswerIndex == answer;

	//debugging
	cout << (isCorrectAnswer ? "Correct" : "incorrect" )<< endl;;

	//progress incremented in nextQuestion()	
	*score += isCorrectAnswer ? 1 : 0;
	
	return isCorrectAnswer;
}
