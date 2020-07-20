#include <iostream>
#include "Quiz.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>


using namespace std;

/*	
_________________________________________________________________________

FIELDS
_________________________________________________________________________
*/

//SUB-MAIN PROCESS RETURN-CONDITIONS
enum SUBPROCESS_END_CONDITION { USER_CLOSED, TIMEOUT, TASK_COMPLETED, ERROR_BROKE_FROM_WHILE };

//selection moving
short selectedIndex = -1;

enum vertical { UP, DOWN};
enum horizontal { LEFT, RIGHT };

//COMMON FIELDS FOR AUDIO AND GRAPHICS
//font
sf::Font mainFont;
//colors
sf::Color white = sf::Color::White;
sf::Color orange = sf::Color(255, 153, 0,255);
sf::Color darkGray = sf::Color(40,40,40,255);
sf::Color lightOrange = sf::Color(255, 184, 77, 255);
sf::Color paleGreen = sf::Color(0, 255, 153);

//sounds
sf::Sound selectSound;
sf::Sound rightSound;
sf::Sound wrongSound;
	//sf::Sound tickerSound;

//text
//help Prompts
sf::Text help; 


//SETUP DETAILS FOR QUIZ
int length = 10;
QuizXMLFile listOfQuizzes("resources/data/listOfQuizzes.xml");
int quizLineNo = 2;
bool timed = false;


//QUIZ
Connection::Question currentQuestion;


/*
_________________________________________________________________________

SUB-MAIN FUNCTIONS THAT CREATE A WINDOW
_________________________________________________________________________
*/

SUBPROCESS_END_CONDITION MENU();
SUBPROCESS_END_CONDITION PLAYQUIZ(Quiz& quiz);
SUBPROCESS_END_CONDITION SHOWRESULT(Quiz& quiz);
void CREATEQUIZ();


/*
_________________________________________________________________________

AUXILIARY FUNCTIONS
_________________________________________________________________________
*/

/*
	move the highlight from Text to the one above or below it 
	depending on the vertical direction
*/
template<std::size_t size>
void moveHighlight(vertical direction, std::array<sf::Text, size>& items);

/*	
	When in the menu stage, change the value of the source quiz 
	and/or length and/or choose whether or not to add a timer
	No need to be arbitrary about the array of items. 
	only called in MENU on menu options
*/
void incMenuItemValue(horizontal direction, int selectedIndex); 

/*
	When in the quiz stage, call this after changing the currentQuestion field 
	to update the Texts accordingly
*/
template<std::size_t size>
void resetQAtexts(sf::Text& question, std::array<sf::Text, size>&answers, const sf::Window& window);

/*
	Sets position of a Text. 
	Centers it horizontally in a window, and sets the y component to the arguement y
*/
void centerText(sf::Text& text, const sf::Window& window, float y=0);

/*
	Create a Text and sets its font character size, style, and color. 
	Used with the copy constructor of Text to initialise a Text object
*/
sf::Text prepText(const int charSize =24, const sf::Text::Style style = sf::Text::Regular, const sf::Font& font = mainFont, const sf::Color& color = white);

/*
	reset the values of length, timed and quizNo to their initial values
	reduces chances of bugs when playing multiple rounds
*/
void resetQuizOptions();


/*
_________________________________________________________________________

MAIN
_________________________________________________________________________
*/
int main() {
	string* answer = new string("0");
	cout << "WHICH WOULD YOU LIKE TO DO? \n";
	cout << "PRESS 0 TO PLAY EXISTING QUIZZES (GUI)\n" ;
	cout << "PRESS 1 TO CREATE A NEW QUIZ (CONSOLE)\n";
	getline(cin,*answer);
	cout << "you chose " << (*answer=="1"?"create":"play")<<"\n";
	//run createQuiz only if input is 1;
	//in any other case, even if not 0 play the quiz
	if (*answer == "1") {
		delete answer;
		//string* topic = new string();
		//cout << "Quiz Topic:" << endl;
		//cin >> *topic;
		//getline(cin, *topic);
		//cout << *topic << endl;
		CREATEQUIZ();
		return 0;
	}

	else {
		delete answer;
		//SETUP
	//fonts
		mainFont.loadFromFile("resources/fonts/open-sans/OpenSans-Regular.ttf");

		//sounds
		sf::SoundBuffer selectBuffer;
		sf::SoundBuffer rightBuffer;
		sf::SoundBuffer wrongBuffer;
		//sf::SoundBuffer tickerBuffer;

		selectBuffer.loadFromFile("resources/sounds/SELECT.wav");
		rightBuffer.loadFromFile("resources/sounds/RIGHT_ANSWER.wav");
		wrongBuffer.loadFromFile("resources/sounds/WRONG_ANSWER.wav");
		//tickerBuffer.loadFromFile("resources/sounds/TIMETICKER.wav");

		selectSound.setBuffer(selectBuffer);
		rightSound.setBuffer(rightBuffer);
		wrongSound.setBuffer(wrongBuffer);
		//tickerSound.setBuffer(tickerBuffer);

		selectSound.setVolume(40);
		//tickerSound.setPitch(1.0f);
		//tickerSound.setLoop(true);

	//texts
		help = prepText(15);


		//RUN WITH LOGGING
		SUBPROCESS_END_CONDITION latestState;
		while (true)
		{
			resetQuizOptions();
			cout << "selectedIndex is " << selectedIndex << endl;
			latestState = MENU();


			if (latestState == ERROR_BROKE_FROM_WHILE)
			{
				cout << "error, MENU event loop unexpectedly exited" << endl;
				return -1;
			}
			else if (latestState == USER_CLOSED)
			{
				cout << "user exited application" << endl;
				return 0;
			}
			else if (latestState == TASK_COMPLETED)
			{
				//proceed to quiz
				cout << "user proceeded to quiz" << endl;

				Quiz quiz(listOfQuizzes.getProp(quizLineNo, "filepath"), length);
				latestState = PLAYQUIZ(quiz);
				quiz.printAllQuestions();

				if (latestState == USER_CLOSED)
				{
					cout << "user exited quiz, loop to MENU" << endl;
					continue;
					//no return, loop to beginning of while
				}
				else if (latestState == ERROR_BROKE_FROM_WHILE)
				{
					cout << "error, PLAYQUIZ event loop unexpectedly exited" << endl;
					return -1;
				}

				//ELSE
				if (latestState == TIMEOUT)
				{
					cout << "timeout!" << endl;
				}
				if (latestState == TASK_COMPLETED)
				{
					cout << "end of quiz reached before/without timeout" << endl;
				}
				latestState = SHOWRESULT(quiz);
				if (latestState == USER_CLOSED) { return 0; }
				else if (latestState == TASK_COMPLETED) { continue; }
				else { return -1; }

			}
		}

		cout << "unexpected end of main loop" << endl;
		return -1;
	}
	


}


/*
_________________________________________________________________________

FUNCTIONS DEFINITIONS
_________________________________________________________________________
*/

SUBPROCESS_END_CONDITION MENU() {
	//RENDER WINDOW
	sf::RenderWindow window(sf::VideoMode(550, 350), "MENU", sf::Style::Close | sf::Style::Titlebar );
	//SET UP Options MENU
	std::array<sf::Text, 4> menuOptions;
	//cout << "menOptsSize " << menuOptions.size() << endl;
	for (int i = 0; i < 4; i++)
	{
		menuOptions[i] = prepText(24, sf::Text::Bold);
	}
	menuOptions[0].setString("Quiz: " + listOfQuizzes.getProp(quizLineNo, "topic"));
	menuOptions[1].setString("Length: " + to_string(length));

	string yes_no = timed ? "10 seconds/question" : "none";
	menuOptions[2].setString("Timer: " + yes_no);
	menuOptions[3].setString("START!");
	menuOptions[3].setCharacterSize(50);
	for (size_t i = 0; i < 3; i++)
	{
		centerText(menuOptions[i], window, float(30 + 50 * i));
	}
	centerText(menuOptions[3], window, float(200));


	help.setString("*UP/DOWN-navigate, LEFT/RIGHT-cycle options, ENTER to press START");

	centerText(help, window, 320.0f);

	//EVENT LOOP
	while (window.isOpen())
	{
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) 
			{
			window.close();
			return USER_CLOSED;
			}
				
			else
			{
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code)
					{
					case sf::Keyboard::Up:
						moveHighlight(UP, menuOptions);
						break;
					case sf::Keyboard::Down:
						moveHighlight(DOWN,menuOptions);
						break;
					case sf::Keyboard::Right:
					case sf::Keyboard::Left:
						incMenuItemValue((event.key.code == sf::Keyboard::Right ? RIGHT : LEFT), selectedIndex);

						menuOptions[0].setString("Quiz: " + listOfQuizzes.getProp(quizLineNo, "topic"));
						menuOptions[1].setString("Length: " + to_string(length));

						yes_no = timed ? "10 seconds per question" : "none";
						menuOptions[2].setString("Timer: " + yes_no);
						for (size_t i = 0; i < menuOptions.size(); i++)
						{
							centerText(menuOptions[i], window, menuOptions[i].getPosition().y);
						}
						break;

					case sf::Keyboard::Enter:
						if (selectedIndex == 3) //start button
						{
							rightSound.play();

							window.close();
							return TASK_COMPLETED;
							//return 0;
						}
						break;
					default:
						break;
					}
				}
			}

		}

		window.clear(darkGray);

		for (size_t i = 0; i < 4; i++)
		{
			window.draw(menuOptions[i]);
		}
		window.draw(help);

		window.display();
	}
	return ERROR_BROKE_FROM_WHILE;
}

SUBPROCESS_END_CONDITION PLAYQUIZ(Quiz& quiz) {
	selectedIndex = -1; //enter key triggers integer comparison. this reset prevents accidental points. 

	selectSound.setPitch(1.0f);

	//RENDER WINDOW
	sf::RenderWindow window(sf::VideoMode(1000, 500), listOfQuizzes.getProp(quizLineNo, "topic"), sf::Style::Close | sf::Style::Titlebar);
	
	//PREPARE TEXTS
	help.setString("*UP/DOWN to move, ENTER to select/skip");
	centerText(help, window, 470);


	sf::Text question= prepText(27, sf::Text::Bold, mainFont, lightOrange);
	question.setPosition(0, 100.0f);

	std::array<sf::Text, 4> answers;
	for (int i = 0; i < 4; i++)
	{
		answers[i] = prepText();
		answers[i].setPosition(0, float(170 + 70 * i));
	}

		//SET INITIAL QUESTION
	currentQuestion = quiz.nextQuestion();
	resetQAtexts(question, answers,window);

		//INITIALISE PROGRESS METER
	sf::Text progress = prepText(24, sf::Text::Bold, mainFont, paleGreen);
	progress.setPosition(0, 50);
	progress.setString("question: " + to_string(quiz.getProgress()) + " of " + to_string(quiz.getLength()));
	centerText(progress, window, 50.0f);

	//INITIALISE TIMER MECHANISM
	sf::RectangleShape timerBar;
	int* timer = new int(0);
	int* timerMax = new int(length * 10); //10seconds per question
	//delete the timer mechanism if it is not being used
	if (!timed)
	{
		delete timer;
		delete timerMax;
	}
	else 
	{
		timerBar.setSize(sf::Vector2f(float(window.getSize().x),20.0f));
		timerBar.setFillColor(paleGreen);
		timerBar.setPosition(0, 0);
	}
	sf::Clock clock;

	//EVENT LOOP
	while (window.isOpen())
	{

		//LOOP TIMER HANDLING
		if (timed) 
		{
			*timer = clock.getElapsedTime().asMilliseconds();

			float* winX = new float(window.getSize().x);//reduces impact of calling window.getSize() 3 times in the following statement
			timerBar.setSize(sf::Vector2f(*winX-(	*timer	* (*winX) / (*timerMax))/(*winX),	20.0f));

			delete winX;
			
			window.draw(timerBar);
			if (*timer >= *timerMax*1000)
			{
				cout << "timeout" << endl;
				delete timer;
				delete timerMax;
				window.close();
				return TIMEOUT;
			}
		}

		sf::Event event;
		while (window.pollEvent(event))
		{

			//CLOSE BUTTON CLICKED
			if (event.type == sf::Event::Closed) 
			{
				window.close();
				return USER_CLOSED;
			}


			else
			{
				//QUIZ COMPLETED
				if (quiz.getProgress() > quiz.getLength()) 
				{
					window.close();
					return TASK_COMPLETED;

				}

				//KEY PRESSED EVENT HANDLERS
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code)
					{
					case sf::Keyboard::Up:
						moveHighlight(UP, answers);
						break;
					case sf::Keyboard::Down:
						moveHighlight(DOWN, answers);
						break;

					case sf::Keyboard::Enter:
						cout << quiz.getProgress() << " of " << quiz.getLength() << endl;

						if (quiz.acceptUserAnswer(selectedIndex, currentQuestion)) 
						{ 
							rightSound.play();
						}
						else 
						{
							wrongSound.play();
						}
						currentQuestion = quiz.nextQuestion();
						currentQuestion.shuffleAnswers();
						resetQAtexts(question,answers,window);
						progress.setString("question: " + to_string(quiz.getProgress()) + " of " + to_string(quiz.getLength()));
						centerText(progress, window, 50.0f);
						break;
					default:
						break;
					}
				}
			}

		}

		//RESET UI
		window.clear(darkGray);

		window.draw(question);
		window.draw(progress);
		window.draw(timerBar);
		for (size_t i = 0; i < 4; i++)
		{
			window.draw(answers[i]);
		}
		window.draw(help);

		window.display();
	}
	return ERROR_BROKE_FROM_WHILE;
}

SUBPROCESS_END_CONDITION SHOWRESULT(Quiz& quiz)
{
	selectedIndex = -1;
	sf::RenderWindow window(sf::VideoMode(550, 350), "RESULTS", sf::Style::Close | sf::Style::Titlebar);
	
	sf::Text topic = prepText(26,sf::Text::Bold, mainFont, lightOrange);
	sf::Text scoreLabel = prepText(26, sf::Text::Bold, mainFont, paleGreen);
	sf::Text scoreNumber = prepText(40, sf::Text::Bold, mainFont, paleGreen);

	help.setString("*UP/DOWN to move, ENTER to select");
	topic.setString(listOfQuizzes.getProp(quizLineNo, "topic"));
	scoreLabel.setString("score:");
	scoreNumber.setString(to_string(quiz.getScore()) + " out of " + to_string(quiz.getLength()));

	centerText(topic, window, 30.0f);
	centerText(scoreLabel, window, 80.0f);
	centerText(scoreNumber, window, 110.0f);
	centerText(help, window, 320.0f);

	std::array<sf::Text,2> closeOptions;

	for (int i = 0; i < 2; i++)
	{
		closeOptions[i] = prepText(28, sf::Text::Bold);
	}
	closeOptions[0].setString("BACK TO MENU");
	closeOptions[1].setString("CLOSE");

	for (int i = 0; i < 2; i++)
	{
		centerText(closeOptions[i], window,200+50*i);
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type==sf::Event::Closed)
			{
				window.close();
				return USER_CLOSED;
			}
			else
			{
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code)
					{
					case sf::Keyboard::Up:
						moveHighlight(UP, closeOptions);
						break;
					case sf::Keyboard::Down:
						moveHighlight(DOWN, closeOptions);
						break;
					case sf::Keyboard::Enter:
						if (selectedIndex==0)//menu
						{
							return TASK_COMPLETED;
						}
						else if(selectedIndex == 1)
						{
							return USER_CLOSED;
						}
					default:
						break;
					}
				}
			}
		}



		window.clear(darkGray);

		window.draw(topic);
		window.draw(scoreLabel);

		window.draw(scoreNumber);
		window.draw(help);
		for (int i = 0; i < 2; i++)
		{
			window.draw(closeOptions[i]);
		}


		window.display();
	}
	return ERROR_BROKE_FROM_WHILE;
}

//AUXILIARY FUNCTION DEFINITIONS
template<std::size_t size>
void moveHighlight(vertical direction, std::array<sf::Text, size>& items) 
{
	cout << "size: " << items.size() << endl;
	if(selectedIndex>=0 &&selectedIndex<items.size())
	items[selectedIndex].setFillColor(white);

	selectedIndex += direction == UP ? -1 : 1;
	selectedIndex = selectedIndex % items.size();
	cout << "selected: " << selectedIndex << endl;
	items[selectedIndex].setFillColor(orange);
	selectSound.setPitch(1.0f);
	selectSound.play();
}

template<std::size_t size>
void resetQAtexts(sf::Text& question, std::array<sf::Text, size>& answers, const sf::Window& window)
{
	question.setString(currentQuestion.questionBody);
	centerText(question, window, question.getPosition().y);


	for (int i = 0; i < answers.size(); i++)
	{
		answers[i].setString(currentQuestion.answerList[i]);
		centerText(answers[i], window, answers[i].getPosition().y);

	}

}


void incMenuItemValue(horizontal direction, int selectedIndex) 
{
	//this is only used in case 1 of the switch
	int maxAllowedLength = 10 * (stoi(listOfQuizzes.getProp(quizLineNo, "length")) / 10);

	switch (selectedIndex)
	{
	case 0://quiz
		selectSound.setPitch(20.0f);
		selectSound.play();


		quizLineNo += (direction == LEFT) ? -1 : 1;

		if (listOfQuizzes.getParentContainerOf(quizLineNo) == -1)
		{
			//quizNo is NOT within the range of the document
			if (quizLineNo >= listOfQuizzes.getQuizXMLDocLength() - 1) //after the last entry
			{
				//the first quiz
				quizLineNo = 2;//simple way of getting the line of the first quiz
			}
			else //before the first entry
			{

				//the last quiz
				quizLineNo = listOfQuizzes.getQuizXMLDocLength() - 2;
			}

		}
		std::cout << quizLineNo << endl;
		std::cout << listOfQuizzes.getParentContainerOf(quizLineNo) << endl;
		std::cout << listOfQuizzes.getProp(quizLineNo, "topic") << endl;
		length = 10;
		timed = false;
		break;




	case 1://length
		selectSound.setPitch(20.0f);
		selectSound.play();

		length += (direction == LEFT) ? -10 : 10;
		length = length < 10 ? maxAllowedLength : length>maxAllowedLength ? 10 : length;

		std::cout << length << endl;
		break;

	case 2://timed
		selectSound.setPitch(20.0f);
		selectSound.play();
		timed = !timed;
		std::cout << timed << endl;

		break;
	default:
		break;
	}
}


void centerText(sf::Text& text, const sf::Window& window,float y)
{
	text.setPosition(window.getSize().x / 2 - text.getLocalBounds().width / 2, y);
}

sf::Text prepText(const int charSize, const sf::Text::Style style ,const sf::Font& font, const sf::Color& color)
{
	sf::Text text;
	text = sf::Text();
	text.setFont(font);
	text.setCharacterSize(charSize);
	text.setStyle(style);
	text.setFillColor(color);
	return text;
}

void resetQuizOptions()
{
	//prevents irregular, unexpected effects from arising in between iterations of the main loop

	selectedIndex = -1;
	length = 10;
	quizLineNo = 2;
	timed = false;

}

//TO CREATE A NEW QUIZ. RUNS A CONSOLE BASED PROGRAM
void CREATEQUIZ()
{
	string* topic = new string();
	int* length = new int();

	//MAKING A NEW QUIZ FILE

	cout << "Quiz Topic:" << endl;
	//cin >> *topic;
	getline(cin, *topic);

	cout << "you set Topic to " << *topic << endl;


SETLENGTH:
		cout << "Quiz Length( must be at least 10 ):" << endl;
		//cast to int to ensure numeric value
		string* lengthAsString = new string();	
		getline(cin,*lengthAsString);
		*length = stoi(*lengthAsString);
		delete lengthAsString;

		if (*length < 10) //pick another length
		{
			goto SETLENGTH; 
		}

	
	//questions and answers
	string* anAnswer = new string();
	vector<Connection::Question>* questions = new vector<Connection::Question>();
	Connection::Question* q;

	for (size_t i = 0; i < *length; i++)
	{
		q = new Connection::Question();

		//prepare a struct of the new question
		cout << "enter question number:"<<(i+1) << endl;
		getline(cin,q->questionBody);

		cout << "enter the correct answer to the question:  " << q->questionBody << endl;
		getline(cin, *anAnswer);
		q->answerList.push_back(*anAnswer);
		q->correctAnswerIndex = 0;

		for (int j = 1; j < Connection::MAX_NO_ANSWERS; j++)//indices 1 to 3 of the answerlist
		{
			cout << "enter an incorrect answer to the question:" << q->questionBody << endl;
			getline(cin, *anAnswer);
			q->answerList.push_back(*anAnswer);
		}

		questions->push_back(*q);

		delete q;

	}
	delete anAnswer;
	
	//turn into xml file, including referencing in list
	Connection::questionVectorToQuizFile(*questions,*topic,"resources/data/",listOfQuizzes);


	delete questions;
	delete topic;
	delete length;
}

