#include "QuizXMLFile.h"

QuizXMLFile::QuizXMLFile(const string& filePath)
{
	this->filePath = new string(filePath);
	quizFile.open(filePath);
	Lines = new vector<string>();

	string currentLine;
	while (getline(quizFile, currentLine)) {
		//trim whitespaces
		currentLine = trim(currentLine);
		if (currentLine == "\n") {
			//ignore blank lines
			continue;
		}
		
		Lines->push_back(currentLine);
	}
}

QuizXMLFile::~QuizXMLFile()
{
	delete Lines;
	delete filePath;
}

vector<string> QuizXMLFile::getLines()
{
	return *Lines;
}

string QuizXMLFile::getFilePath()
{
	return *filePath;
}

int QuizXMLFile::getQuizXMLDocLength()
{
	return Lines->size();
}

int QuizXMLFile::getParentContainerOf(int line) {
	string* expectedParentContainerTag = new string();

	if (line < Lines->size()-1 && line > 1) 
	{
		if (getProp(line, "?tag") == "question")
		{
			*expectedParentContainerTag = "quiz";
		}
		else if (getProp(line, "?tag") == "answer")
		{
			*expectedParentContainerTag = "question";
		}
		else//any other value of ?tag
		{
			//if the line is in the document
			//if the line is not an answer or question, assume its parent is the root container
			//due to the limited nature of the files that may be represented by this class, this is not an issue
			return 1;
		}
		//if the tag was actually question or answer
		for (int i = line; i > 0; i--)//line 0 is the xml header
		{
			if (getProp(i, "?tag") == *expectedParentContainerTag)
			{
				delete expectedParentContainerTag;
				return i;
			}
		}
		
	}
	delete expectedParentContainerTag;
	//if the line is not wihtin the document, or it is the root container, or it is the xml header
	return -1;
}

int QuizXMLFile::getEndOfContainerIndex(int line)
{
	//if line is self closing return line
	regex* selfClosing = new regex(".+\/>");
	smatch smatch_selfClosing;
	if (regex_search(Lines->at(line),smatch_selfClosing,*selfClosing)) 
	{
		delete selfClosing;
		return line;
	}
	//else
	delete selfClosing;
	string containerTag = getProp(line, "?tag");

	for (int i = line; i < Lines->size(); i++)
	{
		if (getProp(i, "?tag") == "/" + containerTag) {
			return i;
		}
	}
	return -1;
	
}

string QuizXMLFile::getProp(int line, const string& propName)
{
	const string& strLine = Lines->at(line);
	string* regInit = new string();
	if (propName == "?tag") {//for the name of the container
		*regInit = "<([/A-z]+)";
	}

	else {//for any other property
	//in all quiz XML files, this sequence will identify a property
		*regInit = ")+ *=[\"]([^\"]+)[\"]";
		*regInit = "(?:" + propName + *regInit;
	}
	regex* reg = new regex(*regInit);
	smatch propField;

	regex_search(strLine, propField, *reg);


	delete reg;
	delete regInit;

	return propField.str(1);
	
}

void QuizXMLFile::writeLinesToFile(const vector<string>& lines , const string& fileName, bool append)
{
	std::ofstream outfile;

	outfile.open(fileName);


	for (size_t i = 0; i < lines.size(); i++)
	{
		outfile << (lines.at(i)+"\n");
	}

}

//aux function
string QuizXMLFile::trim(string& str)
{
		size_t first = str.find_first_not_of(' ');
		size_t last = str.find_last_not_of(' ');
		return str.substr(first, (last - first + 1));
}
