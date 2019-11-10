// Author: wm
// Assignment: Final Project
// File name: main.cpp
// Description: An interactive counselor for the CS department at PCC.
// Last edited: 7/30

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>

const int MAX_SIZE = 5;

using namespace std;

struct Record {
    int number;
    std::string sentence;
};
struct Rank {
    int lineNumber;
    int numberOfMatches;
};

void userQuestion(string& line);
//** get user info set flag for menu
void openFile(ifstream& inFile, string filename);
//** open the files make sure exist / not open
void readFile(ifstream& inFile, vector<Record>& recordVector);
//** create vectors for contents of question
void exactMatch(string& line, bool& matched, vector<Record>& questionsVector, vector<Record>& answersVector);
//** exact match returns result immediately
void parseLine(string& line, vector<Record>& questionsVector, vector<string>& keywordsVector);
//** not an exact match strip the word to keywords store in vector
void sortKeywords(vector<string>& keywordsVector, bool matched);
//** alphabetize keywordsVector
string searchQA(int& lineNumber, vector<Record>& recordVector, vector<string>& keywordsVector, vector<Rank>& rankVector);
//** for menu display
void updateRank(ofstream& outFile, bool matched, vector<Record>& questionsVector);
//** update questionRank.txt with the new rank value based on user requests
void menu(int& choice, bool matched, int& lineNumber, ofstream& outFile, vector<Record>& questionsVector,
     vector<Record>& answersVector, vector<string>& keywordsVector, vector<Rank> rankVector,
     string& info, string line);
//** give the user options for selection and continuning
void leaveInfo(string& info, string line, ofstream& fout);
//** none of the options are good prompt to leave user info create questionsNotAnswered file

// helpers
void intro();
bool restart(char& ans);
void validate();
void newLine();

int main() {
    // in file
    ifstream inQuestion, inAnswer;
    // out file
    ofstream outQuestion;

    // vector structures line # / sentence
    vector<Record> questionsVector, answersVector;
    // vector structure line # / number of matches
    vector<Rank> rankVector;
    // vector for storing parsed user input
    vector<string> keywordsVector;

    // for restart
    char ans(' ');
    // the user input, the files used to compare user input, and answer to user input
    string question, info, questionFile("questionRank.txt"), answerFile("answerQuestionNumber.txt");
    // flag for when a question has been accepted by user
    bool matched(false);
    // for menu and display
    int choice(0), lineNumber(0);

    intro();
    do {
        // INPUT
        userQuestion(question);

        // PRE-PROCESSING
        openFile(inQuestion, questionFile);
        openFile(inAnswer, answerFile);

        // PROCESSING
        readFile(inQuestion, questionsVector);
        readFile(inAnswer, answersVector);
        //// special case (OUTPUT) set flag
        exactMatch(question, matched, questionsVector, answersVector);
        updateRank(outQuestion, matched, questionsVector);
        //// update the file with the new rank info
        parseLine(question, questionsVector, keywordsVector);
        sortKeywords(keywordsVector, matched);

        // OUTPUT
        menu(choice, matched, lineNumber, outQuestion, questionsVector, answersVector, keywordsVector, rankVector,
             question, info);
    } while (restart(ans));
    return 0;
}

void userQuestion(string& line) {
    cout << "<< What would you like to know about Computer Science at PCC?\n";
    getline(cin, line);
    cout << endl;
}
void openFile(ifstream& inFile, string filename) {
    inFile.open(filename);
    if (inFile.fail()) {
        cout << "File not found! ";
        exit(2);    // questions/answers file not found or open
    }
}
void readFile(ifstream& inFile, vector<Record>& recordVector) {
    Record temp;
    char ch;
    while (inFile >> temp.number) {
        inFile.get(ch);
        getline(inFile, temp.sentence);
        recordVector.push_back(temp);
    }
    inFile.close();
}
void exactMatch(string& line, bool& matched, vector<Record>& questionsVector, vector<Record>& answersVector) {
    const int RANK(1);
    unsigned int i(0);
    matched = false;
    for (i; i < questionsVector.size(); i++) {
        if (line == questionsVector[i].sentence && !matched) {
            cout << ">> An exact match to the question, \"" << line << "?\" was found!\n";
            cout << ">> " << answersVector[i].sentence << ".\n";
            cout << endl;
            // increment the rank of the answered question
            questionsVector[i].number += RANK;
//            cout << "***rank: " << questionsVector[i].number;   // stub
            matched = true;
        }
        else {
            continue;
        }
    }
}
void updateRank(ofstream& outFile, bool matched, vector<Record>& questionsVector) {
    if (matched) {
        outFile.open("questionRank.txt", ios::trunc);
        for(unsigned int i = 0; i < questionsVector.size(); i++) {
            outFile << questionsVector[i].number << " " << questionsVector[i].sentence << endl;
        }
    }
    outFile.close();
}
void parseLine(string& line, vector<Record>& questionsVector, vector<string>& keywordsVector) {
    stringstream ss(line);
    string word;
    string temp;
    bool spaceFound;
    unsigned int i = 0;
    // take each character in the string and modify
    while (getline(ss, word)) {
        for (unsigned int i = 0; i < word.size(); i++) {
            if (isalnum(word[i])) {
                if (isalpha(word[i])) {
                    // make letters lowercase
                    word[i] = tolower(word[i]);
                }
            } else if (ispunct(word[i])) {
                // remove punctuation
                word.erase(i, 1);
            } else if (isspace(word[i]) && isspace(word[i + 1])) {
                // flag to mark an extra space was found
                spaceFound = true;
                while (spaceFound) {
                    // remove extra spaces
                    word.erase(i--, 1);
                    spaceFound = false;
                }
            }
        }
        // put the words together
        temp += word;
    }
//    cout << "***after first while: " << temp << endl;
//    cout << "***keywords vector: ";
    stringstream ss2(temp);
    while (ss2 >> word) {
        if (word.length() > 2 || word == "cs")
            keywordsVector.push_back(word);
//            cout << keywordsVector[i] << " ";
//            i++;
        // remove common three letter words
        if (word == "the" || word == "was" || word == "and" || word == "for" || word == "are" || 
            word == "but" || word == "not" || word == "you" || word == "all" || word == "and") {
            keywordsVector.pop_back();
        }
    }
    cout << endl;
}
void sortKeywords(vector<string>& keywordsVector, bool matched) {
    unsigned int j;
    if (!matched) {
        cout << ">> Searching keywords: ";
        for (unsigned int i = keywordsVector.size() - 1; i > 0; i--) {
            for (unsigned int j = 0; j < i; j++) {
                if (keywordsVector[j] < keywordsVector[j + 1]) {
                    // swap
                    string temp = keywordsVector[j + 1];
                    keywordsVector[j + 1] = keywordsVector[j];
                    keywordsVector[j] = temp;
                }
            }
            cout << "<" << keywordsVector[i] << ">" << " ";
        }
        cout << endl;
        cout << endl;
    }
}
string searchQA(int& lineNumber, vector<Record>& recordVector, vector<string>& keywordsVector, vector<Rank>& rankVector) {
    // unfortunately my algorithm for this just does not work
    const int RANK(1);
    unsigned int i(0);
    string result("");
    for (i; i < recordVector.size(); i++) {
        if (keywordsVector[i] == recordVector[i].sentence) {
            rankVector[i].numberOfMatches += RANK;
            cout << "inside search SA" << recordVector[i].sentence << endl;
        }
    }
    lineNumber = i;
    result = recordVector[lineNumber].sentence;
    return result;
}
void menu(int& choice, bool matched, int& lineNumber, ofstream& outFile, vector<Record>& questionsVector,
          vector<Record>& answersVector, vector<string>& keywordsVector, vector<Rank> rankVector,
          string& info, string line) {
    string options[MAX_SIZE];
    if (!matched) {
        cout << ">> Sorry, I couldn't find the answer to that question.\n";
        cout << ">> Are any of these options similar to the question you want answered?\n";
        cout << endl;
        // match the most keywords
        cout << "1. " << searchQA(lineNumber, questionsVector, keywordsVector, rankVector) << endl;
        cout << "2. " << searchQA(lineNumber, questionsVector, keywordsVector, rankVector) << endl;
        cout << "3. " << searchQA(lineNumber, questionsVector, keywordsVector, rankVector) << endl;
        // 2 most popular questions
        cout << "4. " << searchQA(lineNumber, questionsVector, keywordsVector, rankVector) << endl;
        cout << "5. " << searchQA(lineNumber, questionsVector, keywordsVector, rankVector) << endl;
        cout << "0. None of these options are related to my question.\n";
        cout << endl;
        cout << "<< Please make your selection: ";
        cin >> choice;
        cout << endl;
        char ans;
        switch (choice) {
            case 1:
                searchQA(lineNumber, answersVector, keywordsVector, rankVector);
                updateRank(outFile, matched, questionsVector);
                break;
            case 2:
                searchQA(lineNumber, answersVector, keywordsVector, rankVector);
                updateRank(outFile, matched, questionsVector);
                break;
            case 3:
                searchQA(lineNumber, answersVector, keywordsVector, rankVector);
                updateRank(outFile, matched, questionsVector);
                break;
            case 4:
                searchQA(lineNumber, answersVector, keywordsVector, rankVector);
                updateRank(outFile, matched, questionsVector);
                break;
            case 5:
                searchQA(lineNumber, answersVector, keywordsVector, rankVector);
                updateRank(outFile, matched, questionsVector);
                break;
            case 0:
                leaveInfo(info, line, outFile);
                break;
            default:
                restart(ans);
                break;
        }
    }
}
void leaveInfo(string& info, string line, ofstream& fout) {
    cout << ">> Would you like to have someone email you back with the answer? [y]/[n]: ";
    char yesNo;
    cin >> yesNo;
    newLine();
    if (yesNo == 'y' || yesNo == 'Y') {
        cout << "<< Please enter your name and email address separated by spaces: ";
        getline(cin, info);
    } else {
        // user does not want to leave their contact info
    }
    cout << endl;
    fout.open("questionsNotAnswered.txt", ios::app);
    while (fout.is_open()) {
        fout << info;
        fout.close();
    }
}

void intro() {
    cout << "PCC C++ CS Counselor v 1.0.0\n";
    cout << "----------------------------\n";
}
bool restart(char& ans) {
    cout << "Exit? [y/n]: ";
    while (!(cin >> ans) || (ans != 'y' && ans != 'Y' && ans != 'n' && ans != 'N')) {
        cout << "[y]es / [n]o: ";
        validate();
    }
    // why is this so necessary it seems like a huge oversight
    newLine();
    if (ans == 'y' || ans == 'Y') {
        cout << endl;
        return false;
    } else {
        cout << endl;
        return true;
    }
}
void validate() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void newLine() {
    char symbol;
    do {
        cin.get(symbol);
    } while (symbol != '\n');
}
/* sample run
 * PCC C++ CS Counselor v 1.0.0
----------------------------
<< What would you like to know about Computer Science at PCC?
Does the Computer Science department offer tutoring services
Does the Computer Science department offer tutoring services

>> An exact match to the question, "Does the Computer Science department offer tutoring services?" was found!
>> Yes, see Math Success Center: https://pasadena.edu/academics/support/success-centers/math-success-center/tutoring.php
.


Exit? [y/n]:n
 n

<< What would you like to know about Computer Science at PCC?
Hi how are you whats your name?
Hi how are you whats your name?


>> Searching keywords: <computer> <department> <does> <how> <name> <offer> <science> <services> <tutoring> <whats>

>> Sorry, I couldn't find the answer to that question.
>> Are any of these options similar to the question you want answered?

1.
2.
3.
4.
5.
0. None of these options are related to my question.

<< Please make your selection:0
 0

>> Would you like to have someone email you back with the answer? [y]/[n]:y
 y
<< Please enter your name and email address separated by spaces:Boba Fett bobafett@gmail.com
 Boba Fett bobafett@gmail.com

Exit? [y/n]:y
 y
 */