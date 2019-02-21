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
#include <set>
#include <sstream>
#include <vector>

enum TERNARY { FALSE, TRUE, UNDEFINED };

typedef std::set<std::set<long>> frm;
typedef std::vector<TERNARY> vrs;

TERNARY operator!(TERNARY variables) {
    if (variables == TERNARY::UNDEFINED) {
        return variables;
    }
    return variables == TERNARY::TRUE ? TERNARY::FALSE : TERNARY::TRUE;
}

void decide(vrs &variables, unsigned long &undefined) {
    for (auto &&variable : variables) {
        if (variable == TERNARY::UNDEFINED) {
            variable = TERNARY::FALSE;
            undefined++;
            break;
        }
    }
}

bool backtrack(vrs &variables, vrs &back_space, unsigned long &undefined) {
    for (unsigned long index{0}; index < variables.size(); index++) {
        if (variables[index] != TERNARY::UNDEFINED) {
            if (variables[index] == back_space[index]) {
                back_space[index] = TERNARY::UNDEFINED;
                variables[index] = TERNARY::UNDEFINED;
                undefined--;
            } else {
                back_space[index] = !variables[index];
                variables[index] = !variables[index];
                return true;
            }
        }
    }
    return false;
}

unsigned long to_index(const long &literal) { return static_cast<unsigned long>(std::abs(literal) - 1); }

long to_literal(const unsigned long &index, const vrs &variables) { return variables[index] == TRUE ? +(index + 1) : -(index + 1); }

bool exist_conflict(frm &formula, vrs &variables, unsigned long &undefined) {
    for (const auto &clause : formula) {
        unsigned long counter{0};
        for (const auto &literal : clause) {
            if (variables[to_index(literal)] != TERNARY::UNDEFINED) {
                counter += literal == -to_literal(to_index(literal), variables);
            }
        }
        if (counter == clause.size()) {
            return true;
        }
    }
    return false;
}

bool run(frm &formula, vrs &variables, vrs &back_space, unsigned long &undefined) {
    unsigned long counter{0};
    for (;;) {
        if (exist_conflict(formula, variables, undefined)) {
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
                variables.resize(n, TERNARY::UNDEFINED);
                back_space.resize(n, TERNARY::UNDEFINED);
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
        for (auto i{0}; i < variables.size(); i++) {
            std::cout << (variables[i] ? (i + 1) : -(i + 1)) << (i < variables.size() - 1 ? " " : "");
        }
        std::cout << " 0" << std::endl;
    } else {
        std::cout << "UNSAT" << std::endl;
    }

    return EXIT_SUCCESS;
}