#include<utility.hpp>
#define SEP ""
#define LOGGING false

string itos(int n, int radix = 10) {
    string result = "";
    bool negative = false;
    if(n == 0) return "0";
    else if(n < 0) {
        negative = true;
        n = -n;
    }

    if(radix < 2 || radix > 36) return result;

    while (n > 0) {
        int digit = n % radix;
        if (digit < 10) {
            result = (char)('0' + digit) + result;
        } else {
            result = (char)('A' + digit - 10) + result;
        }
        n /= radix;
    }

    return negative ? "-" + result : result;
}

int stoi(string s, int radix = 10) {
    int result = 0;
    if(radix < 2 || radix > 36) return 0;

    for (unsigned int i = (s[0] == '-' ? 1 : 0); i < s.length(); i++) {
        result *= radix;
        if (s[i] >= '0' && s[i] <= '9') {
            result += s[i] - '0';
        } else if (s[i] >= 'A' && s[i] <= 'Z') {
            result += s[i] - 'A' + 10;
        } else if (s[i] >= 'a' && s[i] <= 'z') {
            result += s[i] - 'a' + 10;
        } else {
            return -1;
        }
    }

    if(s[0] == '-') result = -result;
    return result;
}

bool isSpace(char c) {
    return c == ' ' || c == '\t';
}

void log(string message) {
    if(LOGGING) cout << message << endl;
}

string sep() {
    return SEP;
}

string align_right(string s, int width, char fill) {
    if(s.length() >= width) return s;
    else return string(width - s.length(), fill) + s;
}

string dealign_right(string s, char fill) {
    for(unsigned int i = 0; i < s.length(); i++) {
        if(s[i] != fill) {
            s.erase(0, i);
            return s;
        }
    }
    return "";
}

string dealign_left(string s, char fill) {
    for(unsigned int i = s.length() - 1; i >= 0; i--) {
        if(s[i] != fill) {
            s.erase(i + 1, s.length() - i - 1);
            return s;
        }
    }
    return "";
}

vector<string> split(string &s, string delimiter) {
    vector<string> result;
    size_t pos = 0;
    while((pos = s.find(delimiter)) != string::npos) {
        result.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    result.push_back(s);
    return result;
}

string upper(string &s) {
    string result = "";
    for(unsigned int i = 0; i < s.length(); i++) {
        result += toupper(s[i]);
    }
    return result;
}