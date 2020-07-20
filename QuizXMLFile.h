#pragma once
#include <vector>
#include <string>
#include <iostream>
#include<fstream>
#include <regex>

using namespace std;
//Allows for controlled access to xml files
//provides limited interactions to allow the Quiz Connection to interact with the quizzes only as far as will be needed by other processes
//is tailored to the very specific format of the quiz files, and will most probably fail if used on other XML files
class QuizXMLFile
{
private:
	vector<string>* Lines;
	ifstream quizFile;
	string* filePath;
public:
	//constructor to initialise the vector field using this file
	QuizXMLFile(const string& filePath);
	//destructor
	~QuizXMLFile();
	//QuizXMLFile(QuizXMLFile& originalFile);
	QuizXMLFile& operator =(const QuizXMLFile& originalFile);

	//get a copy of the lines  in this file
	vector<string> getLines();

	//get a copy of the file path this file was created from
	string getFilePath();

	//getter to find how many lines in the (formatted) document
	int getQuizXMLDocLength();

	//return the index of the line that opens the container that this line is a member of
	//getContainerOf(int line) is specific to the quiz file format and will fail if the file the object it is a member of is based on is any other xml file
	int getParentContainerOf(int line);
	
	//return the line that closes the container opened on the line in the arguement
	//specific to the quiz xml format, in which an element is never nested inside another element of the same type
	int getEndOfContainerIndex(int line);

	//return the string value of a property based on the property name, given that this line opens a container and a such a property exists for that container
	string getProp(int line, const string& propName); 



	/*FUNCTIONS INVOLVED IN WRITING TO A FILE*/
	static void writeLinesToFile(const vector<string>& lines, const string& fileName, bool append = true);

private://AUX FUNCTIONS
	//trim whitespace off the ends of strings
	string trim(string& str);
};


