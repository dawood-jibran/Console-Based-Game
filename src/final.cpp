#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

const int MAX_Q = 100;
const int MAX_OPT = 4;

const int TIME_LIMIT = 10;
const int EXTRA_TIME = 10;

const int LEVEL_EASY = 1;
const int LEVEL_MEDIUM = 2;
const int LEVEL_HARD = 3;

string q_text[MAX_Q];
string q_opt[MAX_Q][MAX_OPT];
int q_ans[MAX_Q];
string playerName;

bool isNumber(const string& s) 
{
    if (s == "") 
        return false;

    for (int i = 0; s[i] != '\0'; i++) 
    {
        if (s[i] < '0' || s[i] > '9') 
            return false;
    }
    return true;
}


void showMenu() 
{
    cout << "===========" << endl;
    cout << "QUIZ MASTER" << endl;
    cout << "===========" << endl;
    cout << "1. start quiz" << endl;
    cout << "2. view high scores" << endl;
    cout << "3. exit" << endl;
}

int loadQuestions(const string& fileName, int maxQ) 
{
    ifstream in(fileName);
    if (!in) 
    {
        cout << "cannot open file: " << fileName << endl;
        return 0;
    }

    int n;
    in >> n;
    in.ignore(1000, '\n');

    if (n > maxQ) 
        n = maxQ;

    for (int i = 0; i < n; i++) 
    {
        getline(in, q_text[i]);
        for (int j = 0; j < MAX_OPT; j++) 
        {
            getline(in, q_opt[i][j]);
        }
        in >> q_ans[i];
        in.ignore(1000, '\n');
    }

    return n;
}

void saveHighScore(const string& name, int marks) 
{
    ofstream out("high_scores.txt", ios::app);
    if (!out) 
    {
        cout << "cannot open high_scores.txt" << endl;
        return;
    }
    out << name << " " << marks << endl;
}

void viewHighScores() 
{
    ifstream high("high_scores.txt");
}

void runQuiz(int total, int level, const string& fileName) 
{
    if (total <= 0) 
    {
        cout << "no questions available." << endl;
        return;
    }

    int idxList[MAX_Q];
    for (int i = 0; i < total; i++) 
        idxList[i] = i;

    for (int i = 0; i < total; i++) 
    {
        int r = rand() % total;
        int t = idxList[i];
        idxList[i] = idxList[r];
        idxList[r] = t;
    }

    int neg = (level == LEVEL_EASY ? 2 : level == LEVEL_MEDIUM ? 3 : 5);
    int quizCount = (total < 10 ? total : 10);

    int marks = 0;
    int correct = 0;
    int wrong = 0;
    int streak = 0;

    bool used50 = false;
    bool usedSkip = false;
    bool usedReplace = false;
    bool usedExtra = false;

    ofstream log("quiz_logs.txt", ios::app);

    if (log) 
    {
        log << "=========" << endl;
        log << "Player: " << playerName << endl;
        log << "Category: " << fileName << endl;
        log << "Difficulty: " << (level == 1 ? "Easy" : level == 2 ? "Medium" : "Hard") << endl;
        log << "-----------" << endl;
    }


    for (int q = 0; q < quizCount; q++) 
    {
        int idx = idxList[q];
        bool removed[MAX_OPT] = { false, false, false, false };
        int limit = TIME_LIMIT;

        cout << endl;
        cout << "question " << q + 1 << " of " << quizCount << endl;
        cout << "lifelines: ";
        cout << "[1:50/50 " << (used50 ? "used" : "available") << "] ";
        cout << "[2:skip " << (usedSkip ? "used" : "available") << "] ";
        cout << "[3:replace " << (usedReplace ? "used" : "available") << "] ";
        cout << "[4:extra time " << (usedExtra ? "used" : "available") << "] ";
        cout << endl;

        cout << "enter lifeline (1-4) or 0: ";

        string lifStr;
        int lif = 0;

        cin >> lifStr;
        if (isNumber(lifStr)) 
            lif = stoi(lifStr);

        if (lif == 1 && !used50) 
        {
            used50 = true;
            int c = q_ans[idx] - 1;
            int wrongList[3], wc = 0;
            for (int i = 0; i < MAX_OPT; i++) if (i != c) 
                wrongList[wc++] = i;
            int r1 = rand() % wc;
            int r2;
            do 
            { 
                r2 = rand() % wc; 
            } 
            while (r2 == r1);
            removed[wrongList[r1]] = true;
            removed[wrongList[r2]] = true;
            cout << "50/50 used." << endl;
        }
        else if (lif == 2 && !usedSkip) 
        {
            usedSkip = true;
            cout << "question skipped." << endl;
            if (log) 
            {
                log << "Q" << q + 1 << ": " << q_text[idx] << endl;
                log << "ACTION: SKIPPED" << endl;
                log << "---------------" << endl;
            }
            continue;
        }
        else if (lif == 3 && !usedReplace) 
        {
            usedReplace = true;
            int newIdx;
            do 
            { 
                newIdx = rand() % total; 
            } 
            while (newIdx == idx);
            idx = newIdx;
            cout << "question replaced." << endl;
        }
        else if (lif == 4 && !usedExtra) 
        {
            usedExtra = true;
            limit = TIME_LIMIT + EXTRA_TIME;
            cout << "extra time granted." << endl;
        }

        cout << endl;
        cout << "Q: " << q_text[idx] << endl;
        if (log) 
            log << "Q" << q + 1 << ": " << q_text[idx] << endl;

        for (int i = 0; i < MAX_OPT; i++) 
        {
            cout << i + 1 << ". ";
            if (removed[i]) 
            {
                cout << "----" << endl;
                if (log) 
                    log << i + 1 << ". ----" << endl;
            } else 
            {
                cout << q_opt[idx][i] << endl;
                if (log) 
                    log << i + 1 << ". " << q_opt[idx][i] << endl;
            }
        }

        cout << "you have " << limit << " seconds." << endl;
        cout << "your answer: ";

        time_t start = time(0);

        string ansStr;
        int ans = -1;

        cin >> ansStr;
        if (isNumber(ansStr)) ans = stoi(ansStr);

        time_t endt = time(0);
        int taken = (endt - start);

        cout << "time taken: " << taken << " seconds." << endl;

        string result;

        if (taken > limit) 
        {
            cout << "time up." << endl;
            wrong++;
            marks -= neg;
            streak = 0;
            result = "TIME_UP";
        }
        else 
        {
            if (ans >= 1 && ans <= MAX_OPT && removed[ans - 1])
                 ans = -1;

            if (ans == q_ans[idx]) 
            {
                cout << "correct." << endl;
                correct++;
                marks += 5;
                streak++;
                if (streak == 3) 
                {
                    cout << "streak bonus +5" << endl;
                    marks += 5;
                }
                else if (streak == 5) 
                {
                    cout << "big streak bonus +15" << endl;
                    marks += 15;
                }
                result = "CORRECT";
            } 
            else 
            {
                cout << "wrong. correct answer: " << q_ans[idx] << endl;
                wrong++;
                marks -= neg;
                streak = 0;
                result = "WRONG";
            }
        }

        if (log) 
        {
            log << "Player answer: " << ans << endl;
            log << "Correct option: " << q_ans[idx] << endl;
            log << "Result: " << result << endl;
            log << "Time taken: " << taken << endl;
            log << "Marks now: " << marks << endl;
            log << "--------------" << endl;
        }
    }

    cout << endl;
    cout << "quiz finished." << endl;
    cout << "correct answers: " << correct << endl;
    cout << "wrong answers  : " << wrong << endl;
    cout << "total marks    : " << marks << endl;

    if (log) 
    {
        log << "SUMMARY: correct=" << correct << " wrong=" << wrong << " marks=" << marks << endl;
        log << "=================" << endl << endl;
    }

    saveHighScore(playerName, marks);
}

void startQuiz() 
{
    cout << "enter your name: ";
    cin.ignore(1000, '\n');
    getline(cin, playerName);
    if (playerName.empty()) 
        playerName = "player";

    cout << endl;
    cout << "select difficulty:" << endl;
    cout << "1. easy" << endl;
    cout << "2. medium" << endl;
    cout << "3. hard" << endl;
    cout << "enter choice: ";

    string lvlStr;
    cin >> lvlStr;
    if (!isNumber(lvlStr)) 
    {
        cout << "invalid difficulty." << endl;
        return;
    }

    int lvl = stoi(lvlStr);
    if (lvl != 1 && lvl != 2 && lvl != 3) 
    {
        cout << "invalid difficulty." << endl;
        return;
    }

    cout << endl;
    cout << "select category:" << endl;
    cout << "1. science" << endl;
    cout << "2. computer" << endl;
    cout << "3. sports" << endl;
    cout << "4. history" << endl;
    cout << "5. iq / logic" << endl;
    cout << "enter choice: ";

    string catStr;
    cin >> catStr;
    if (!isNumber(catStr)) 
    {
        cout << "invalid category." << endl;
        return;
    }

    int cat = stoi(catStr);
    string fileName;

    if (cat == 1) 
        fileName = "science.txt";
    else if (cat == 2) 
        fileName = "computer.txt";
    else if (cat == 3) 
        fileName = "sports.txt";
    else if (cat == 4) 
        fileName = "history.txt";
    else if (cat == 5) 
        fileName = "iq.txt";
    else 
    {
        cout << "invalid category." << endl;
        return;
    }

    int total = loadQuestions(fileName, MAX_Q);

    if (total == 0) 
    {
        cout << "no questions loaded." << endl;
        return;
    }

    cout << "loaded " << total << " questions." << endl;
    runQuiz(total, lvl, fileName);
}

int main() {
    srand(time(0));

    while (true) 
    {
        showMenu();
        cout << "enter your choice: ";

        int choiceStr;
        cin >> choiceStr;

        if (choiceStr == 1) 
            startQuiz();
        else if (choiceStr == 2) 
            viewHighScores();
        else if (choiceStr == 3) 
        {
            cout << "exiting." << endl;
            return 0;
        }
        else 
        {
            cout << "invalid choice." << endl;
        }

        cout << endl;
    }

    return 0;
}
