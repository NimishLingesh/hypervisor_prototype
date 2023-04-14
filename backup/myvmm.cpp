#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <unordered_map>

using namespace std;

vector<string> customSplit(string str, char separator) {
    vector < string > strings;
    int startIndex = 0, endIndex = 0;
    for (int i = 0; i <= str.size(); i++) {
        
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) {
            endIndex = i;
            string temp;
            temp.append(str, startIndex, endIndex - startIndex);
            strings.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
    return strings;
}

void handle_instructions(string file_name, int inst_limit) {
    int registers[32] = { 0 };
    ifstream file(file_name);

    // validating the file
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_name << endl;
        return;
    }

    vector < string > ret;

    // Read the file line by line
    string line;

    int inst_count = 0;
    while (getline(file, line)) {
        inst_count += 1;
        if (inst_count > inst_limit)
            break;
        // Parse the instruction
        string arg, op;
        istringstream iss(line);
        iss >> op >> arg;

        // Conditions for the instructions
        if (op == "add") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] + registers[stoi(ret[2].substr(1))];
        }
        else if (op == "addi") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] + stoi(ret[2]);
        }
        else if (op == "sub") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] - registers[stoi(ret[2].substr(1))];
        }
        else if (op == "mul") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] * registers[stoi(ret[2].substr(1))];
        }
        else if (op == "and") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] & registers[stoi(ret[2].substr(1))];
        }
        else if (op == "or") {
            ret = customSplit(arg, ',');
            if ('$' == ret[2][0]){
                registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] | registers[stoi(ret[2].substr(1))];
            }
            else
            {
                registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] | stoi(ret[2]);
            }
        }
        else if (op == "xor") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] ^ registers[stoi(ret[2].substr(1))];
        }
        else if (op == "sll") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] << stoi(ret[2]);
        }
        else if (op == "srl") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] >> stoi(ret[2]);
        }          
        else if (op == "li") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = stoi(ret[1]);
        }
        else if (op == "DUMP_PROCESSOR_STATE") {
            cout << "Registers:" << endl;
            for (int i = 0; i < 32; i++) {
            cout << "$" << i << ": " << registers[i] << endl;
            }
        }
        else if (op == "#")
            continue;
        else {
            cerr << "Error: Unknown instruction " << op << endl;
        return;
        }
    }
    cout << "Number of instructions executed: " << inst_count << endl;

// Close the file
file.close();

}

pair<string, string> read_config_file(string config_file_name){
    map<string, string> config;
    pair<string, string> ret_val;
    ifstream config_file(config_file_name);

    if (!config_file.is_open()) {
        cerr << "Error opening configuration file: " << config_file_name << endl;
        ret_val.first =  "Invalid";
        ret_val.second =  "1";
        return ret_val;
    }

    string line;
    while (getline(config_file, line)) {
        size_t pos = line.find('=');
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            if (key != "vm_exec_slice_in_instructions" && key != "vm_binary") {
                cerr << "Invalid contents of configuration file !!" << key << endl;
                ret_val.first =  "Invalid";
                ret_val.second =  "1";
                return ret_val;
            }
            config[key] = value;
        }
    }

    config_file.close();
    ret_val.first =  config["vm_binary"];
    ret_val.second = config["vm_exec_slice_in_instructions"];
    return ret_val;
}


// main program
int main(int argc, char* argv[]) {
    try{
        // if no arguments passed, then program waits for the context switch
        if (argc < 2) {
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            long int prev_cs_count = 0;
            while (true) {
                if (usage.ru_nivcsw > prev_cs_count) {
                    cout << "Involuntary context switch to another process or VM - " << usage.ru_nivcsw << endl;
                    prev_cs_count = usage.ru_nivcsw;
                }
                sleep(1);
            }
        }
        for(int i = 1; i < argc; ++i) {
            string file_name, inst_count;
            pair<string, string> ret_val;
            
            ret_val = read_config_file(argv[i]);
            file_name = ret_val.first;
            inst_count = ret_val.second;
            if (file_name == "Invalid")
                continue;
            else{
                cout << "------------------------------------------------" << endl;
                cout << "Handling the instructions of " << file_name << endl;
                handle_instructions(file_name, stoi(inst_count));
                cout << "Execution success !!!" << endl << endl;
            }
        }


    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
