#include<string>

using namespace std;

class InputStream {
    public:
        virtual string readline() = 0;
        virtual bool eof() = 0;
};

class OutputStream {
    public:
        virtual void write(string s) = 0;
};
