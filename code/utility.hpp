#include<iostream>
#include<vector>

using namespace std;

string itos(int n, int radix);

int stoi(string s, int radix);

bool isSpace(char c);

void log(string message);

string sep();

string align_right(string s, int width, char fill = ' ');

string dealign_right(string s, char fill = ' ');

string dealign_left(string s, char fill = ' ');

vector<string> split(string &s, string delimiter);

string upper(string &s);