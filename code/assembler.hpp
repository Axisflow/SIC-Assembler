#include<stream.hpp>
#include<utility.hpp>
#include<set>
#include<unordered_map>

using namespace std;

class SICAssembler {
    struct instruction {
        string label;
        string opcode;
        string operand;
        int address;
        int length;
    };

    struct text_record {
        int start_address;
        int length;
        string object_codes;
    };

    private:
        InputStream* input;
        OutputStream* output_object;
        fstream* intermediate;
        OutputStream* output_listing;
        unordered_map<string, int> symbol_table;
        int start_address;
        int program_length;
        int error_flag;

        void write_comment(int &line_number, string &comment) const;
        // pass 1
        instruction process_instruction(int &locctr, string &label, string &opcode, string &operand);
        void write_intermediate_line(int &line_number, instruction &processed_instruction) const;
        // pass 2
        string toObjCode(string &opcode, string &operand);
        void process_text_record(text_record& t_record, int &address, string &obj_code);
        void write_text_record(text_record& t_record) const;
        void write_listing_line(string &intermediate_line, string &obj_code) const;

        static OutputStream *fake_output_stream;
        static const unordered_map<string, unsigned char> opcode_table;
        static const unordered_map<string, set<unsigned char>> format_table;

    public:
        SICAssembler(InputStream* input, OutputStream* output_object, fstream* intermediate, OutputStream* output_listing = fake_output_stream);
        bool pass1();
        bool pass2();
        bool assemble();

        void setInputStream(InputStream* input);
        void setOutputObjectStream(OutputStream* output_object);
        void setIntermediateStream(fstream* intermediate);
        void setOutputListingStream(OutputStream* output_listing);
        void setSymbolTable(unordered_map<string, int> symbol_table);
        void setProgramLength(int program_length);

        InputStream* getInputStream();
        OutputStream* getOutputObjectStream();
        fstream* getIntermediateStream();
        OutputStream* getOutputListingStream();
        unordered_map<string, int> getSymbolTable();
        int getProgramLength();
        int getErrorFlag();

        // pass 1
        static bool parse_input_line(string line, string& label, string& opcode, string& operand);
        static bool input_is_comment(string line);
        // pass 2
        static bool parse_intermediate_line(string line, int &line_number, int& address, string& label, string& opcode, string& operand);
        static bool intermediate_is_comment(string line);
        static text_record initialize_text_record(int address);
};