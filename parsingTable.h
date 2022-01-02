//
// Created by Frederick Mo on 2021/12/31.
//

#ifndef INTERMEDIATE_CODE_GENERATOR_PARSINGTABLE_H
#define INTERMEDIATE_CODE_GENERATOR_PARSINGTABLE_H

#include <string>
#include <vector>
#include <set>

using std::vector;
using std::string;
using std::pair;
using std::set;

class parsingTable {
public:
    parsingTable() = default;
    explicit parsingTable(vector<pair<string, vector<string>>> &table) : parsing_table(table) {}
    pair<string, vector<string>> call(int index) {
        return parsing_table.at(index);
    }

private:

    vector<pair<string, vector<string>>> parsing_table{};
};


#endif //INTERMEDIATE_CODE_GENERATOR_PARSINGTABLE_H
