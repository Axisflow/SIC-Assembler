#include<stream_interface.hpp>
#include<fstream>
#include<iostream>

using namespace std;

class FileInputStream: public InputStream {
    private:
        ifstream file;
    public:
        FileInputStream(string filename);
        string readline();
        bool eof();
        ~FileInputStream();
};

class FileOutputStream: public OutputStream {
    private:
        ofstream file;
    public:
        FileOutputStream(string filename);
        void write(string s);
        ~FileOutputStream();
};

class ConsoleInputStream: public InputStream {
    private:
        istream &console;
    public:
        ConsoleInputStream(istream &console);
        string readline();
        bool eof();
};

class ConsoleOutputStream: public OutputStream {
    private:
        ostream &console;
    public:
        ConsoleOutputStream(ostream &console);
        void write(string s);
};

class NoneOutputStream: public OutputStream {
    public:
        void write(string s);
};
