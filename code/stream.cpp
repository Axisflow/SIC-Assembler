#include "stream.hpp"

FileInputStream::FileInputStream(string filename) {
    file.open(filename);
}

string FileInputStream::readline() {
    string s;
    getline(file, s);
    return s;
}

bool FileInputStream::eof() {
    return file.eof();
}

FileInputStream::~FileInputStream() {
    file.close();
}

FileOutputStream::FileOutputStream(string filename) {
    file.open(filename);
}

void FileOutputStream::write(string s) {
    file.write(s.c_str(), s.length());
    file.flush();
}

FileOutputStream::~FileOutputStream() {
    file.close();
}

ConsoleInputStream::ConsoleInputStream(istream &console): console(console) { }

string ConsoleInputStream::readline() {
    string s;
    getline(console, s);
    return s;
}

bool ConsoleInputStream::eof() {
    return console.eof();
}

ConsoleOutputStream::ConsoleOutputStream(ostream &console): console(console) { }

void ConsoleOutputStream::write(string s) {
    console << s;
}

void NoneOutputStream::write(string s) { }