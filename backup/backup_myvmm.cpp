#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <unordered_map>

using namespace std;

// Define the processor registers
int registers[32] = { 0 };

// Define the memory space
int memory[1024] = { 0 };

// Define the program counter
int pc = 0;

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

// Define a map from register names to register indices
unordered_map<string, int> register_names = {
    {"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3}, {"$a0", 4},
    {"$a1", 5}, {"$a2", 6}, {"$a3", 7}, {"$t0", 8}, {"$t1", 9},
    {"$t2", 10}, {"$t3", 11}, {"$t4", 12}, {"$t5", 13}, {"$t6", 14},
    {"$t7", 15}, {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19},
    {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23}, {"$t8", 24},
    {"$t9", 25}, {"$k0", 26}, {"$k1", 27}, {"$gp", 28}, {"$sp", 29},
    {"$fp", 30}, {"$ra", 31}
};

// Define the main program
int main(int argc, char* argv[]) {
    // Check that a filename was provided
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " filename" << endl;
        return 1;
    }

    // Open the file
    ifstream file(argv[1]);

    // Check that the file was opened successfully
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    vector < string > ret;

    // Process each line of the file
    string line;
    while (getline(file, line)) {
        // Parse the instruction
        string op, arg1, arg2, arg3;
        string arg, gar;
        
        istringstream iss(line);
        iss >> op >> arg;
        // iss >> op >> arg1 >> arg2 >> arg3;

        // Execute the instruction
        if (op == "add") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] + registers[stoi(ret[2].substr(1))];
            // registers[register_names[arg1]] = registers[register_names[arg2]] + registers[register_names[arg3]];
            pc += 4;
        }
        else if (op == "addi") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] + stoi(ret[2]);
            // registers[register_names[arg1]] = registers[register_names[arg2]] + stoi(arg3);
            pc += 4;
        }
        else if (op == "sub") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] - registers[stoi(ret[2].substr(1))];
            // registers[register_names[arg1]] = registers[register_names[arg2]] - registers[register_names[arg3]];
            pc += 4;
        }
        else if (op == "mul") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] * registers[stoi(ret[2].substr(1))];
            // registers[register_names[arg1]] = registers[register_names[arg2]] * registers[register_names[arg3]];
            pc += 4;
        }
        else if (op == "and") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] & registers[stoi(ret[2].substr(1))];
            // registers[register_names[arg1]] = registers[register_names[arg2]] & registers[register_names[arg3]];
            pc += 4;
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
            
            // registers[register_names[arg1]] = registers[register_names[arg2]] | registers[register_names[arg3]];
            pc += 4;
        }
        else if (op == "xor") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] ^ registers[stoi(ret[2].substr(1))];
            // registers[register_names[arg1]] = registers[register_names[arg2]] ^ registers[register_names[arg3]];
            pc += 4;
        }
        else if (op == "sll") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] << stoi(ret[2]);
        //    registers[register_names[arg1]] = registers[register_names[arg2]] << stoi(arg3);
            pc += 4;
        }
        else if (op == "srl") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = registers[stoi(ret[1].substr(1))] >> stoi(ret[2]);
            // registers[register_names[arg1]] = registers[register_names[arg2]] >> stoi(arg3);
            pc += 4;
        }          
        else if (op == "li") {
            ret = customSplit(arg, ',');
            registers[stoi(ret[0].substr(1))] = stoi(ret[1]);
            pc += 4;
        }
        else if (op == "lw") {
            int address = registers[register_names[arg2]] + stoi(arg3);
            registers[register_names[arg1]] = memory[address];
            pc += 4;
        }
        else if (op == "sw") {
        int address = registers[register_names[arg2]] + stoi(arg3);
        memory[address] = registers[register_names[arg1]];
        pc += 4;
        }
        else if (op == "beq") {
        if (registers[register_names[arg1]] == registers[register_names[arg2]]) {
        pc += stoi(arg3) * 4;
        }
        else {
        pc += 4;
        }
        }
        else if (op == "bne") {
        if (registers[register_names[arg1]] != registers[register_names[arg2]]) {
        pc += stoi(arg3) * 4;
        }
        else {
        pc += 4;
        }
        }
        else if (op == "j") {
        pc = stoi(arg1) * 4;
        }
        else if (op == "jr") {
        pc = registers[register_names[arg1]];
        }
        else if (op == "jal") {
        registers[31] = pc + 4;
        pc = stoi(arg1) * 4;
        }
        else if (op == "jalr") {
        registers[31] = pc + 4;
        pc = registers[register_names[arg1]];
        }
        else if (op == "DUMP_PROCESSOR_STATE") {
            cout << "Registers:" << endl;
            for (int i = 0; i < 32; i++) {
            cout << "$" << i << ": " << registers[i] << endl;
            }
            // cout << "Memory:" << endl;
            // for (int i = 0; i < 1024; i++) {
            // cout << "@" << i << ": " << memory[i] << endl;
            // }
            pc += 4;
        }
        else if (op == "#")
            continue;
        else {
        cerr << "Error: Unknown instruction " << op << endl;
        return 1;
        }
    }

// Close the file
file.close();

// Return success
return 0;

}
