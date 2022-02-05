#include "Input.hpp"
#include <Item.hpp>
#include <Group.hpp>
#include <json.hpp>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <cmath>

using std::vector;
using json = nlohmann::json;

Input::Input(const std::filesystem::path& problem_file, const std::filesystem::path& exe_path) {
    exe_directory = std::filesystem::canonical(exe_path).parent_path();
    std::filesystem::path problem_file_path = problem_file;
    if (!std::filesystem::exists(problem_file)) {
        auto dir = exe_directory;
        while (!std::filesystem::exists(dir / "Data")) {
            if (dir.root_path() == dir) {
                throw std::filesystem::filesystem_error(
                    "Data directory doesn't found",
                    std::make_error_code(std::errc::no_such_file_or_directory)
                );
            }
            dir = dir.parent_path();
        }
        dir.append("Data");
        problem_file_path = dir.append(problem_file.filename().string());
    }

    if (!std::filesystem::exists(problem_file_path)) {
        throw std::filesystem::filesystem_error(
            "problem file doesn't found",
            problem_file_path,
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    read_problem_file(problem_file_path);
}

void Input::read_problem_file(const std::filesystem::path& problem_file_path) {
    std::ifstream ifs;
    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs.open(problem_file_path);
    json j;
    ifs >> j;
    ifs.close();

    Item::N = j["nums"]["itemNum"].get<int>();

    if (j["nums"].find("groupNum") != j["nums"].end()) {
        Group::N = j["nums"]["groupNum"].get<int>();
    }
    else {
        Group::N = Item::N;
    }

    std::string opt_str = j["Opt"].get<std::string>();
    if (opt_str == "minimize") {
        opt = Opt::MIN;
    }
    else if (opt_str == "maximize") {
        opt = Opt::MAX;
    }
    else {
        ifs.close();
        throw std::runtime_error("minimize maximize error");
    }

    size_t item_r_size;
    vector<std::string> item_r_name;
    if (j["params"].find("eachItemRelation") != j["params"].end()) {
        auto& eir = j["params"]["eachItemRelation"];
        item_r_size = std::distance(eir.begin(), eir.end());
        item_r_name.reserve(item_r_size);
        item_relation_params.reserve(item_r_size);

        for (auto itr = eir.begin(), end = eir.end(); itr != end; ++itr) {
            item_r_name.push_back(itr.key());
            item_relation_params.push_back(itr.value().get<double>());
        }
    }
    else {
        item_r_size = 0;
    }

    size_t group_r_size;
    vector<std::string> group_r_name;
    if (j["params"].find("itemGroupRelation") != j["params"].end()) {
        auto& igr = j["params"]["itemGroupRelation"];
        group_r_size = std::distance(igr.begin(), igr.end());
        group_r_name.reserve(group_r_size);
        group_relation_params.reserve(group_r_size);

        for (auto itr = igr.begin(), end = igr.end(); itr != end; ++itr) {
            group_r_name.push_back(itr.key());
            group_relation_params.push_back(itr.value().get<double>());
        }
    }
    else {
        group_r_size = 0;
    }

    vector<std::string> value_name;
    if (j["params"].find("value") != j["params"].end()) {
        auto& v = j["params"]["value"];
        Item::v_size = std::distance(v.begin(), v.end());
        value_name.reserve(Item::v_size);
        value_ave_params.reserve(Item::v_size);
        //Item::item_r_size += Item::v_size;

        for (auto itr = v.begin(), end = v.end(); itr != end; ++itr) {
            value_name.push_back(itr.key());
            value_ave_params.push_back(itr.value()["ave"].get<double>());
            item_relation_params.push_back(itr.value()["distance"].get<double>());
        }
    }

    if (j["params"].find("constant") != j["params"].end()) {
        constant = j["params"]["constant"].get<double>();
    }
    else {
        constant = 0;
    }

    if (j["params"].find("groupCost") != j["params"].end()) {
        group_cost.resize(Group::N);
        if (j["params"]["groupCost"]["equalCost"].get<bool>()) {
            double cost = j["params"]["groupCost"]["cost"].get<double>();
            for (auto&& gc : group_cost) {
                gc = cost;
            }
        }
        else {
            for (size_t i = 0; i < Group::N; ++i) {
                group_cost[i] = j["params"]["groupCost"]["cost"][i].get<double>();
            }
        }
    }

    vector<std::string> weight_name;
    if (j["constraint"].find("weight") != j["constraint"].end()) {
        auto& weight = j["constraint"]["weight"];
        Item::w_size = std::distance(weight.begin(), weight.end());
        weight_name.reserve(Item::w_size);
        weight_upper.resize(Group::N, vector<double>(Item::w_size));
        weight_lower.resize(Group::N, vector<double>(Item::w_size));

        for (auto itr = weight.begin(), end = weight.end(); itr != end; ++itr) {
            weight_name.push_back(itr.key());
            if (itr.value()["equalSize"].get<bool>()) {
                double upper = itr.value()["upper"].get<double>();
                double lower = itr.value()["lower"].get<double>();
                for (size_t i = 0; i < Group::N; ++i) {
                    weight_upper[i][std::distance(weight.begin(), itr)] = upper;
                    weight_lower[i][std::distance(weight.begin(), itr)] = lower;
                }
            }
            else {
                for (size_t i = 0; i < Group::N; ++i) {
                    weight_upper[i][std::distance(weight.begin(), itr)] = itr.value()["upper"][i].get<double>();
                    weight_lower[i][std::distance(weight.begin(), itr)] = itr.value()["lower"][i].get<double>();
                }
            }
        }
    }
    else {
        weight_upper.resize(Group::N, vector<double>());
        weight_lower.resize(Group::N, vector<double>());
    }

    if (opt == Opt::MIN) {
        std::for_each(item_relation_params.begin(), item_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(group_relation_params.begin(), group_relation_params.end(), [](auto& p) { p *= -1; });
        std::for_each(value_ave_params.begin(), value_ave_params.end(), [](auto& p) { p *= -1; });
        std::for_each(group_cost.begin(), group_cost.end(), [](auto& p) { p *= -1; });
        constant *= -1;
    }

    items.reserve(Item::N);
    auto& item_json = j["data"]["item"];
    for (size_t i = 0; i < Item::N; ++i) {
        Item item;
        item.id = i;
        item.predefined_group = -1;
        item.weight.resize(Item::w_size);
        for (size_t j = 0; j < Item::w_size; ++j) {
            item.weight[j] = item_json[i]["weight"][weight_name[j]].get<double>();
        }

        if (Item::v_size == 0) {
            item.values.assign(1, 0);
        }
        else {
            item.values.resize(Item::v_size);
            for (size_t j = 0; j < Item::v_size; ++j) {
                item.values[j] = item_json[i]["value"][value_name[j]].get<double>();
            }
        }

        item.item_relations.assign(Item::N, 0);
        item.group_relations.assign(Group::N, 0);
        item.item_penalty.assign(Item::N, 0);
        item.group_penalty.assign(Group::N, 0);

        items.push_back(item);
    }

    for (size_t i = 0; i < item_r_size; ++i) {
        auto& item_relations = j["data"]["eachItemRelation"][item_r_name[i]];
        for (size_t j = 0; j < Item::N; ++j) {
            for (size_t k = 0; k < Item::N; ++k) {
                items[j].item_relations[k] += item_relations[j][k].get<double>() * item_relation_params[i];
            }
        }
    }

    for (size_t i = 0; i < group_r_size; ++i) {
        auto& group_relations = j["data"]["itemGroupRelation"][group_r_name[i]];
        for (size_t j = 0; j < Item::N; ++j) {
            for (size_t k = 0; k < Group::N; ++k) {
                items[j].group_relations[k] += group_relations[j][k].get<double>() * group_relation_params[i];
            }
        }
    }

    for (size_t i = item_r_size; i < item_r_size + Item::v_size; ++i) {
        for (auto&& item : items) {
            for (size_t j = 0; j < Item::N; ++j) {
                item.item_relations[j] = std::abs(item.values[i - item_r_size] - items[j].values[i - item_r_size]) * item_relation_params[i];
            }
        }
    }

    item_penalty_num = 0;
    if (j["constraint"].find("banItem") != j["constraint"].end()) {
        item_penalty_num = j["constraint"]["banItem"].size();
        for (auto&& ban_pair : j["constraint"]["banItem"]) {
            size_t id1 = ban_pair["id1"].get<size_t>();
            size_t id2 = ban_pair["id2"].get<size_t>();
            items[id1].item_penalty[id2] = 1;
            items[id2].item_penalty[id1] = 1;
        }
    }

    group_penalty_num = 0;
    if (j["constraint"].find("banGroup") != j["constraint"].end()) {
        group_penalty_num = j["constraint"]["banGroup"].size();
        for (auto&& ban_ids : j["constraint"]["banGroup"]) {
            size_t item = ban_ids["item"].get<size_t>();
            size_t group = ban_ids["group"].get<size_t>();
            items[item].group_penalty[group] = 1;
        }
    }

    if (j["constraint"].find("specifyGroup") != j["constraint"].end()) {
        for (auto&& sg : j["constraint"]["specifyGroup"]) {
            size_t item = sg["item"].get<size_t>();
            size_t group = sg["group"].get<size_t>();
            items[item].predefined_group = group;
        }
    }
}