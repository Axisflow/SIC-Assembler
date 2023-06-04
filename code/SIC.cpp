#include<assembler.hpp>
#include<fstream>
#include<iostream>

using namespace std;

int main(int argc, char** argv) {
    InputStream* input;
    fstream* intermediate;
    OutputStream* output_object;
    OutputStream* output_listing;

    if (argc == 1) {
        // Use stdin and stdout for input and output
        input = new ConsoleInputStream(cin);
        intermediate = new fstream("intermediate.int", ios_base::in | ios_base::out | ios_base::trunc);
        output_object = new ConsoleOutputStream(cout);
        output_listing = new ConsoleOutputStream(cout);
    } else if(argc == 3) {
        // Use argv[1] as input file and argv[2] as output file
        input = new FileInputStream(argv[1]);
        intermediate = new fstream(string(argv[2]) + ".int", ios_base::in | ios_base::out | ios_base::trunc);
        output_object = new FileOutputStream(string(argv[2]) + ".obj");
        output_listing = new FileOutputStream(string(argv[2]) + ".lst");
    } else {
        cout << "Usage: " << argv[0] << " [input file] [output file]" << endl;
        return 1;
    }

    SICAssembler assembler(input, output_object, intermediate, output_listing);
    cout << "Assembling..." << endl;
    cout << (assembler.assemble() ? "Assembled successfully" : "Failed to assemble") << endl;
    cout << "Error flag: " << assembler.getErrorFlag() << endl;

    intermediate->close();
    delete input;
    delete intermediate;
    delete output_object;
    delete output_listing;

    cout << "Exiting..." << endl;
        
    return 0;
}