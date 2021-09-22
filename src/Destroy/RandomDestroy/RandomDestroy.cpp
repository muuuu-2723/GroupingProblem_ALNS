#include "RandomDestroy.hpp"
#include <Destroy.hpp>
#include <Solution.hpp>
#include <Person.hpp>
#include <Group.hpp>
#include <MyRandom.hpp>
#include <vector>

using std::vector;

RandomDestroy::RandomDestroy(std::vector<Person>& persons, int destroy_num, int param) : Destroy(persons, param), destroy_num(destroy_num) {
    shuffle_person_ids.reserve(Person::N);
    for (auto&& p : persons) {
        if (!p.is_leader) {
            shuffle_person_ids.push_back(p.id);
        }
    }
}

void RandomDestroy::operator()(Solution& solution) {
    MyRandom::shuffle(shuffle_person_ids);
    vector<MovePerson> move_persons;
    move_persons.reserve(destroy_num);

    for (int i = 0; i < destroy_num; ++i) {
        Person& p = persons[shuffle_person_ids[i]];
        move_persons.push_back(MovePerson(p, solution.get_group_id(p), Group::N));
    }
    solution.move(move_persons);
}