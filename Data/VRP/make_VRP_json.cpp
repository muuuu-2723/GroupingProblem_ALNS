#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include "../../src/include/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "arg error" << endl;
        exit(1);
    }
    ifstream ifs(argv[1]);
    string filename;
    int capa;
    int m;
    for (size_t i = 0; i < 3; ++i) {
        string line;
        getline(ifs, line);
        string type;
        stringstream ss(line);
        getline(ss, type, ':');
        type.erase(type.size() - 1);
        if (type == "FILE NAME") {
            getline(ss, filename, ':');
            filename.erase(0, 1);
        }
        else if (type == "CAPACITY") {
            string c_str;
            getline(ss, c_str, ':');
            capa = stoi(c_str);
        }
        else if (type == "NUMBER OF VEHICLES") {
            string m_str;
            getline(ss, m_str, ':');
            m = stoi(m_str);
        }
        else {
            cerr << "type error" << endl;
            exit(1);
        }
    }

    string tmp;
    getline(ifs, tmp);
    vector<vector<double>> dis;
    while (true) {
        getline(ifs, tmp);
        if (tmp == "DEMAND SECTION") break;
        stringstream ss(tmp);
        vector<double> vec;
        double v;
        while (ss >> v) {
            vec.push_back(v);
        }
        dis.push_back(vec);
    }
    
    if (dis.size() != dis[0].size()) {
        cerr << "distance error" << endl;
        exit(1);
    }

    int n = dis.size();
    vector<int> demands;
    int demand;
    while (ifs >> demand) {
        demands.push_back(demand);
    }
    
    ifs.close();

    json j;
    j["nums"]["itemNum"] = n;
    j["nums"]["groupNum"] = m;

    j["Opt"] = "minimize";

    j["params"]["eachItemRelation"]["distance"] = 1;

    j["constraint"]["weight"]["demand"]["equalSize"] = true;
    j["constraint"]["weight"]["demand"]["upper"] = capa;
    j["constraint"]["weight"]["demand"]["lower"] = 0;

    for (auto&& d : demands) {
        json item;
        item["weight"]["demand"] = d;
        j["data"]["item"].push_back(item);
    }

    j["data"]["eachItemRelation"]["distance"] = dis;

    filesystem::path output_path("../" + filename + ".json");
    ofstream out(output_path);
    out << j;
    out.close();
}