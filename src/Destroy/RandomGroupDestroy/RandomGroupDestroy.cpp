#include "RandomGroupDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Person.hpp>
#include <MyRandom.hpp>
#include <Group.hpp>
#include <vector>
#include <numeric>

using std::vector;

RandomGroupDestroy::RandomGroupDestroy(std::vector<Person>& persons, int destroy_num, int param) : Destroy(persons, param), destroy_num(destroy_num) {
    shuffle_group_ids.resize(Group::N);
    std::iota(shuffle_group_ids.begin(), shuffle_group_ids.end(), 0);
}

void RandomGroupDestroy::operator()(Solution& solution) {
    MyRandom::shuffle(shuffle_group_ids);
    vector<MovePerson> move_persons;
    move_persons.reserve(((Person::N / Group::N) + 1) * destroy_num);

    for (int i = 0; i < destroy_num; ++i) {
        const Group& g = solution.get_groups()[shuffle_group_ids[i]];
        for (const auto& id : g.get_member_list()) {
            if (persons[id].is_leader) continue;
            move_persons.push_back(MovePerson(persons[id], solution.get_group_id(persons[id]), Group::N));
        }
    }

    solution.move(move_persons);
}