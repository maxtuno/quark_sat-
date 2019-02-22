/*
Copyright (c) 2012-2019 Oscar Riveros. all rights reserved. oscar.riveros@peqnp.science
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>


typedef std::set<std::set<long>> frm;
typedef std::vector<long> vrs;


void decide(vrs &variables, unsigned long &undefined) {
    long index{0};
    for (auto &&variable : variables) {
        index++;
        if (variable == 0) {
            variable = -index;
            undefined++;
            break;
        }
    }
}

bool backtrack(vrs &variables, vrs &back_space, unsigned long &undefined) {
    for (unsigned long index{0}; index < variables.size(); index++) {
        if (variables[index] != 0) {
            if (variables[index] == back_space[index]) {
                back_space[index] = 0;
                variables[index] = 0;
                undefined--;
            } else {
                back_space[index] = -variables[index];
                variables[index] = -variables[index];
                return true;
            }
        }
    }
    return false;
}

unsigned long to_index(const long &literal) {
    return static_cast<unsigned long>(std::abs(literal) - 1);
}

unsigned long count_conflicts(frm &formula, vrs &variables, unsigned long &undefined, bool boolean = true) {
    unsigned long conflicts{0};
    for (const auto &clause : formula) {
        unsigned long counter{0};
        for (const auto &literal : clause) {
            if (variables[to_index(literal)] != 0) {
                counter += literal == -variables[to_index(literal)];
            } else {
                break;
            }
        }
        if (counter == clause.size()) {
            if (boolean) {
                return 1;
            }
            conflicts++;
        }
    }
    if (boolean) {
        return 0;
    }
    return conflicts;
}

bool run(frm &formula, vrs &variables, vrs &back_space, unsigned long &undefined) {
    unsigned long counter = 0;
    for (;;) {
        if (count_conflicts(formula, variables, undefined) != 0) {
            if (!backtrack(variables, back_space, undefined)) {
                return false;
            }
        } else {
            if (undefined > counter) {
                counter = undefined;
                std::cout << "c " << variables.size() - counter << std::endl;
            }
            if (undefined == variables.size()) {
                return true;
            } else {
                decide(variables, undefined);
            }
        }
    }
}

int main(int, char *argv[]) {
    char *end;
    unsigned long n{0}, m{0}, undefined{0};
    vrs variables, back_space;
    frm formula;
    auto cnf_file = std::ifstream(argv[1]);
    std::string line, buffer;
    while (getline(cnf_file, line)) {
        std::stringstream ss(line);
        std::set<long> cls;
        while (ss.good()) {
            ss >> buffer;
            if (buffer == "c") {
                break;
            }
            if (buffer == "cnf") {
                ss >> n;
                ss >> m;
                variables.resize(n, 0);
                back_space.resize(n, 0);
                break;
            }
            if (buffer != "0" && buffer != "p" && buffer != "cnf" && buffer != "%") {
                cls.insert(std::strtol(buffer.c_str(), &end, 10));
            }
        }
        if (!cls.empty()) {
            formula.insert(cls);
        }
    }

    if (run(formula, variables, back_space, undefined)) {
        std::cout << "SAT" << std::endl;
        for (auto i = 0; i < variables.size(); i++) {
            std::cout << variables[i] << (i < variables.size() - 1 ? " " : "");
        }
        std::cout << " 0" << std::endl;
    } else {
        std::cout << "UNSAT" << std::endl;
    }

    return EXIT_SUCCESS;
}
