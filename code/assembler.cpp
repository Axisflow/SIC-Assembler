#include "assembler.hpp"

const unordered_map<string, unsigned char> SICAssembler::opcode_table = {
    {"ADD", 0x18}, {"AND", 0x40}, {"COMP", 0x28}, {"DIV", 0x24}, {"J", 0x3C}, {"JEQ", 0x30}, {"JGT", 0x34}, {"JLT", 0x38}, {"JSUB", 0x48}, {"LDA", 0x00}, {"LDCH", 0x50}, {"LDL", 0x08}, {"LDX", 0x04}, {"MUL", 0x20}, {"OR", 0x44}, {"RD", 0xD8}, {"RSUB", 0x4C}, {"STA", 0x0C}, {"STCH", 0x54}, {"STL", 0x14}, {"STSW", 0xE8}, {"STX", 0x10}, {"SUB", 0x1C}, {"TD", 0xE0}, {"TIX", 0x2C}, {"WD", 0xDC}
};

const unordered_map<string, set<unsigned char>> SICAssembler::format_table = {
    // SIC format
    // format 0: no operand
    // format 1: need an operand
    {"ADD", {1}}, {"AND", {1}}, {"COMP", {1}}, {"DIV", {1}}, {"J", {1}}, {"JEQ", {1}}, {"JGT", {1}}, {"JLT", {1}}, {"JSUB", {1}}, {"LDA", {1}}, {"LDCH", {1}}, {"LDL", {1}}, {"LDX", {1}}, {"MUL", {1}}, {"OR", {1}}, {"RD", {1}}, {"RSUB", {0}}, {"STA", {1}}, {"STCH", {1}}, {"STL", {1}}, {"STSW", {0}}, {"STX", {1}}, {"SUB", {1}}, {"TD", {1}}, {"TIX", {1}}, {"WD", {1}}
};

SICAssembler::instruction SICAssembler::process_instruction(int &locctr, string &label, string &opcode, string &operand) {
    instruction _i;
    _i.label = label;
    _i.opcode = opcode;
    _i.operand = operand;
    _i.address = locctr;
    _i.length = 0;

    if(label != "") {
        if(this->symbol_table.find(label) != this->symbol_table.end()) {
            // duplicate symbol
            this->error_flag |= 4;
        } else {
            this->symbol_table[label] = locctr;
        }
    }

    if(opcode == "WORD") {
        _i.length = 3;
    } else if(opcode == "RESW") {
        _i.length = 3 * stoi(operand, 10);
    } else if(opcode == "RESB") {
        _i.length = stoi(operand);
    } else if(opcode == "BYTE") {
        if(toupper(operand[0]) == 'C') {
            _i.length = operand.length() - 3;
        } else if(toupper(operand[0]) == 'X') {
            _i.length = (operand.length() - 3) / 2;
        } else {
            // invalid operand
            this->error_flag |= 8;
        }
    } else if(opcode_table.find(opcode) != opcode_table.end()) {
        _i.length = 3;
    } else if(opcode == "START") {
        this->start_address = locctr = _i.address = stoi(operand, 16);
    } else if(opcode == "END") {
        this->program_length = locctr - this->start_address;
    } else {
        // invalid opcode
        this->error_flag |= 16;
    }

    locctr += _i.length;

    return _i;
}

void SICAssembler::write_intermediate_line(int &line_number, instruction &processed_instruction) const {
    // every element must align to 10 characters
    string element, line = "";
    line += align_right(to_string(line_number * 5), 10, ' ') + "\t";
    line += align_right((processed_instruction.opcode == "END" ? "" : itos(processed_instruction.address, 16)), 10, ' ') + "\t";
    line += align_right(processed_instruction.label, 10, ' ') + "\t";
    line += align_right(processed_instruction.opcode, 10, ' ') + "\t";
    line += align_right(processed_instruction.operand, 10, ' ') + "\n";
    this->intermediate->write(line.c_str(), line.length());
}

void SICAssembler::write_comment(int &line_number, string &comment) const {
    string element, line = "";
    element = to_string(line_number * 5);
    line += string(10 - element.length(), ' ') + element + "\t";
    line += string(10, ' ') + "\t" + comment + "\n";
    this->intermediate->write(line.c_str(), line.length());
}

SICAssembler::SICAssembler(InputStream *input, OutputStream *output_object, fstream *intermediate, OutputStream *output_listing) {
    this->input = input;
    this->output_object = output_object;
    this->intermediate = intermediate;
    intermediate->seekp(0, ios_base::beg);
    this->output_listing = output_listing;
}

OutputStream* SICAssembler::fake_output_stream = new NoneOutputStream();

bool SICAssembler::pass1() {
    string line, opcode, operand, label;
    instruction processed_instruction;
    int locctr, line_number = 0;
    bool first_line = true;

    // restart intermediate file
    this->intermediate->seekp(0, ios::beg);
    this->program_length = 0;
    this->error_flag = 0;
    this->symbol_table = unordered_map<string, int>();
    while(true) {
        if(input->eof()) { // empty file
            this->error_flag |= 1;
            return false;
        }
        line = input->readline();

        if(!this->input_is_comment(line)) break;
        else {
            line_number++;
            this->write_comment(line_number, line);
        }
    }

    if(parse_input_line(line, label, opcode, operand)){
        if(opcode == "START"){
            first_line = false;
            processed_instruction = this->process_instruction(locctr, label, opcode, operand);
            if(this->error_flag) return false;
            line_number++;
            this->write_intermediate_line(line_number, processed_instruction);
        } else if(opcode == "END") { // empty program
            this->error_flag |= 1;
            return false;
        } else {
            locctr = this->start_address = 0;
        }
    } else { // invalid line
        this->error_flag |= 2;
        return false;
    }

    if(first_line) {
        processed_instruction = this->process_instruction(locctr, label, opcode, operand);
        if(this->error_flag) return false;
        line_number++;
        this->write_intermediate_line(line_number, processed_instruction);
    }

    while(!input->eof()) {
        line = input->readline();
        if(!this->input_is_comment(line)) {
            if(parse_input_line(line, label, opcode, operand)){
                if(opcode == "END"){
                    processed_instruction = this->process_instruction(locctr, label, opcode, operand);
                    if(this->error_flag) return false;
                    line_number++;
                    this->write_intermediate_line(line_number, processed_instruction);
                    return true;
                } else {
                    processed_instruction = this->process_instruction(locctr, label, opcode, operand);
                    if(this->error_flag) return false;
                    line_number++;
                    this->write_intermediate_line(line_number, processed_instruction);
                }
            } else { // invalid line
                this->error_flag |= 2;
                return false;
            }
        } else {
            line_number++;
            this->write_comment(line_number, line);
        }
    }

    // no END statement
    this->error_flag |= 32;
    return false;
}

bool SICAssembler::pass2() {
    int line_number, address;
    string line, opcode, operand, label, object_code, h_record, e_record, tmp_s;
    text_record t_record;
    instruction processed_instruction;
    bool first_line = true;

    // restart intermediate file
    this->intermediate->seekp(0, ios::beg);
    this->error_flag = 0;
    while(true) {
        if(intermediate->eof()) { // empty file
            this->error_flag |= 64 | 1;
            return false;
        }
        getline(*this->intermediate, line);

        if(!this->intermediate_is_comment(line)) break;
        else this->output_listing->write(line + '\n');
    }

    if(parse_intermediate_line(line, line_number, address, label, opcode, operand)){
        if(opcode == "START"){
            first_line = false;

            h_record = "H" + sep() + label + '\t' + sep() + align_right(operand, 6, '0') + sep() + align_right(itos(this->program_length, 16), 6, '0') + '\n';
            this->output_object->write(h_record);
            this->write_listing_line(line, object_code);
        } else if(opcode == "END") { // empty program
            this->error_flag |= 64 | 1;
            return false;
        } else {
            h_record = "H" + sep() + "      " + '\t' + sep() + "000000" + sep() + align_right(itos(this->program_length, 16), 6, '0') + '\n';
            this->output_object->write(h_record);
        }
    } else { // invalid line
        this->error_flag |= 64 | 2;
        return false;
    }

    t_record = initialize_text_record(address);

    if(first_line) {
        object_code = this->toObjCode(opcode, operand);
        this->process_text_record(t_record, address, object_code);
        this->write_listing_line(line, object_code);
        if(this->error_flag) return false;
    }

    while(!intermediate->eof()) {
        getline(*this->intermediate, line);
        if(!this->intermediate_is_comment(line)) {
            if(parse_intermediate_line(line, line_number, address, label, opcode, operand)){
                if(opcode == "END"){
                    object_code = "";
                    if(t_record.length > 0) {
                        this->write_text_record(t_record);
                    }
                    this->write_listing_line(line, object_code);

                    e_record = "E" + sep() + align_right(itos(this->start_address, 16), 6, '0') + '\n';
                    this->output_object->write(e_record);
                    return true;
                } else {
                    object_code = this->toObjCode(opcode, operand);
                    this->process_text_record(t_record, address, object_code);
                    this->write_listing_line(line, object_code);
                    if(this->error_flag) return false;
                }
            } else { // invalid line
                this->error_flag |= 64 | 2;
                return false;
            }
        } else {
            this->output_listing->write(line + '\n');
        }
    }

    // no END statement
    this->error_flag |= 64 | 32;
    return false;
}

string SICAssembler::toObjCode(string &opcode, string &operand) {
    string objCode, tmp_s;
    int x = 0, tmp_i;

    if(opcode_table.find(opcode) != opcode_table.end()) {
        objCode = align_right(itos(opcode_table.at(opcode), 16), 2, '0');
        if(operand == "") {
            if(format_table.at(opcode).find(0) != format_table.at(opcode).end()) {
                objCode += "0000";
            } else { // invalid operand
                this->error_flag |= 64 | 8;
                return "";
            }
        } else {
            // if operand have ",X" suffix, then set x = 1
            if(operand[operand.length() - 2] == ',' && operand[operand.length() - 1] == 'X') {
                x = 1 << 15;
                tmp_s = operand.substr(0, operand.length() - 2);
            }
            else tmp_s = operand;

            if(symbol_table.find(tmp_s) != symbol_table.end()) {
                objCode += align_right(itos(symbol_table.at(tmp_s) | x, 16), 4, '0');
            } else { // can't find symbol
                log("can't find symbol: " + tmp_s);
                this->error_flag |= 64 | 4;
                return "";
            }
        }
        
    } else if(opcode == "BYTE") {
        if(toupper(operand[0]) == 'C') {
            for(int i = 2; i < operand.length() - 1; i++) {
                objCode += align_right(itos(operand[i], 16), 2, '0');
            }
        } else if(toupper(operand[0]) == 'X') {
            objCode = operand.substr(2, operand.length() - 3);
        } else { // invalid operand
            this->error_flag |= 64 | 8;
            return "";
        }
    } else if(opcode == "WORD") {
        tmp_i = stoi(operand, 10);
        if(tmp_i < 0) tmp_i += 1 << 24;
        objCode = align_right(itos(tmp_i, 16), 6, '0');
    } else if(opcode == "RESB" || opcode == "RESW") {
        objCode = "";
    } else { // invalid opcode
        this->error_flag |= 64 | 16;
        return "";
    }

    return objCode;
}

void SICAssembler::process_text_record(text_record &t_record, int &address, string &obj_code) {
    if(t_record.start_address + t_record.length < address) {
        if(obj_code != "") {
            this->write_text_record(t_record);
            t_record = initialize_text_record(address);
        } else return;
    }

    string obj_code_tmp = obj_code;
    int tmp;
    while(t_record.length * 2 + obj_code_tmp.length() > 60) {
        tmp = 60 - t_record.length * 2;
        if(tmp > 6) {
            t_record.object_codes += obj_code_tmp.substr(0, tmp);
            obj_code_tmp = obj_code_tmp.substr(tmp, obj_code_tmp.length() - tmp);
            t_record.length += tmp / 2;
        }
        this->write_text_record(t_record);
        tmp = t_record.start_address + t_record.length;
        t_record = initialize_text_record(tmp);
    }
    t_record.object_codes += obj_code_tmp;
    t_record.length += obj_code_tmp.length() / 2;
}

void SICAssembler::write_text_record(text_record &t_record) const {
    this->output_object->write("T" + sep() + align_right(itos(t_record.start_address, 16), 6, '0')
    + sep() + align_right(itos(t_record.length, 16), 2, '0') + sep() + t_record.object_codes + '\n');
}

void SICAssembler::write_listing_line(string &intermediate_line, string &obj_code) const {
    this->output_listing->write(intermediate_line + '\t' + align_right(obj_code, 10, ' ') + '\n');
}

bool SICAssembler::assemble() {
    if (!pass1()) {
        return false;
    }

    if (!pass2()) {
        return false;
    }

    return true;
}

bool SICAssembler::parse_input_line(string line, string& label, string& opcode, string& operand) {
    // split 'line' into 'label', 'opcode', and 'operand'
    // return true if parsing is successful, false otherwise
    // if 'line' is empty, return false
    
    vector<string> tokens;
    string token = "";
    for(int i = 0; i < line.length(); i++) {
        if(!isSpace(line[i])) {
            token += line[i];
        } else {
            if(tokens.size() >= 2) {
                token += line[i];
            } else if(token != "") {
                tokens.push_back(token);
                token = "";
            }
        }
    }

    token = dealign_left(dealign_right(token, ' '), ' ');
    if(token != "") tokens.push_back(token);

    if(tokens.size() == 0) return false;
    if(tokens.size() == 1) {
        label = "";
        opcode = upper(tokens[0]);
        operand = "";
    } else if(tokens.size() == 2) {
        if(opcode_table.find(upper(tokens[0])) != opcode_table.end() || upper(tokens[0]) == "START" || upper(tokens[0]) == "END") {
            label = "";
            opcode = upper(tokens[0]);
            operand = tokens[1];
        } else {
            label = tokens[0];
            opcode = upper(tokens[1]);
            operand = "";
        }
    } else {
        label = tokens[0];
        opcode = upper(tokens[1]);
        operand = tokens[2];
    }

    return true;
}

bool SICAssembler::input_is_comment(string line) {
    // return true if 'line' is a comment, false otherwise
    return line[0] == '.' || line == "";
}

bool SICAssembler::parse_intermediate_line(string line, int &line_number, int& address, string& label, string& opcode, string& operand) {
    vector<string> tokens = split(line, "\t");
    if(tokens.size() != 5) return false;
    line_number = stoi(dealign_right(tokens[0], ' '), 10);
    address = stoi(dealign_right(tokens[1], ' '), 16);
    label = dealign_right(tokens[2], ' ');
    opcode = dealign_right(tokens[3], ' ');
    operand = dealign_right(tokens[4], ' ');
    if(opcode == "") return false;
    return true;
}

bool SICAssembler::intermediate_is_comment(string line) {
    // return true if 'line' is a comment, false otherwise
    int i = 0;
    for(; i < line.length() && line[i] != '\t'; i++);
    if(i >= line.length()) return true;
    else {
        i++;
        for(; i < line.length() && line[i] != '\t'; i++);
        if(i >= line.length()) return true;
        else {
            return line[i+1] == '.';
        }
    }
}

SICAssembler::text_record SICAssembler::initialize_text_record(int address) {
    // return the initialized text record
    text_record t_record;
    t_record.start_address = address;
    t_record.length = 0;
    t_record.object_codes = "";
    return t_record;
}

void SICAssembler::setInputStream(InputStream *input) {
    this->input = input;
}

void SICAssembler::setOutputObjectStream(OutputStream *output_object) {
    this->output_object = output_object;
}

void SICAssembler::setIntermediateStream(fstream *intermediate) {
    this->intermediate = intermediate;
}

void SICAssembler::setOutputListingStream(OutputStream *output_listing) {
    this->output_listing = output_listing;
}

void SICAssembler::setSymbolTable(unordered_map<string, int> symbol_table) {
    this->symbol_table = symbol_table;
}

void SICAssembler::setProgramLength(int program_length) {
    this->program_length = program_length;
}

InputStream *SICAssembler::getInputStream() {
    return this->input;
}

OutputStream *SICAssembler::getOutputObjectStream() {
    return this->output_object;
}

fstream *SICAssembler::getIntermediateStream() {
    return this->intermediate;
}

OutputStream *SICAssembler::getOutputListingStream() {
    return this->output_listing;
}

unordered_map<string, int> SICAssembler::getSymbolTable() {
    return this->symbol_table;
}

int SICAssembler::getProgramLength() {
    return this->program_length;
}

int SICAssembler::getErrorFlag() {
    return this->error_flag;
}
