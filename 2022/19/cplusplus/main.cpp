#include <iostream>
#include <vector>  
#include<list>
#include <string>
#include <cstdint>
#include <regex>

using namespace std;

enum ResourceType
{
    RESOURCE_TYPE_START = -1,
    RESOURCE_TYPE_ORE,
    RESOURCE_TYPE_CLAY,
    RESOURCE_TYPE_OBSIDIAN,
    RESOURCE_TYPE_GEM,
    RESOURCE_TYPE_END,
};

class Blueprint {

    public:
        uint8_t robot_cost[RESOURCE_TYPE_END][RESOURCE_TYPE_END];

    public:
        Blueprint() {
            for (int k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) { 
                for (int i = RESOURCE_TYPE_START + 1; i < RESOURCE_TYPE_END; i++) { 
                    robot_cost[k][i] = 0;
                }
            }
        }

};

class State {
    public:
        uint8_t time;

        uint8_t ore;
        uint8_t clay;
        uint8_t obsidian;
        uint8_t gem;

        uint8_t ore_robots;
        uint8_t clay_robots;
        uint8_t obsidian_robots;
        uint8_t gem_robots;

    public:
        State() {
            time = 0;

            ore = 0;
            clay = 0;
            obsidian = 0;
            gem = 0;

            ore_robots = 1;
            clay_robots = 0;
            obsidian_robots = 0;
            gem_robots = 0;
        }

        State(State *state) {
            
            time = state->time;
            
            ore = state->ore;
            clay = state->clay; 
            obsidian = state->obsidian; 
            gem = state->gem; 

            ore_robots = state->ore_robots; 
            clay_robots = state->clay_robots; 
            obsidian_robots = state->obsidian_robots; 
            gem_robots = state->gem_robots; 
        }

        State get_child_state() {
            State child_state(this);

            child_state.time = this->time - 1;
            child_state.ore += this->ore_robots;
            child_state.clay += this->clay_robots;
            child_state.obsidian += this->obsidian_robots;
            child_state.gem += this->gem_robots;

            return child_state;
        }

};

bool can_build_robot(Blueprint blueprint, State state, ResourceType resource) {

    uint8_t *robot_cost = blueprint.robot_cost[resource];

    return robot_cost[RESOURCE_TYPE_ORE] <= state.ore &&
        robot_cost[RESOURCE_TYPE_CLAY] <= state.clay && 
        robot_cost[RESOURCE_TYPE_OBSIDIAN] <= state.obsidian && 
        robot_cost[RESOURCE_TYPE_GEM] <= state.gem;
}

void build_robot(Blueprint blueprint, State *state, ResourceType resource) {

    uint8_t *robot_cost = blueprint.robot_cost[resource];

    state->ore -= robot_cost[RESOURCE_TYPE_ORE];
    state->clay -= robot_cost[RESOURCE_TYPE_CLAY];
    state->obsidian -= robot_cost[RESOURCE_TYPE_OBSIDIAN];
    state->gem -= robot_cost[RESOURCE_TYPE_GEM];

    switch (resource) {
        case RESOURCE_TYPE_ORE:
            state->ore_robots++;
            break;
        case RESOURCE_TYPE_CLAY:
            state->clay_robots++;
            break;
        case RESOURCE_TYPE_OBSIDIAN:
            state->obsidian_robots++;
            break;
        case RESOURCE_TYPE_GEM:
            state->gem_robots++;
            break;
    }
}


list<State> get_child_states(Blueprint blueprint, State state) {
    list<State> childs;

    State wait_state = state.get_child_state();
    childs.push_back(wait_state);

    for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
        if (can_build_robot(blueprint, state, (ResourceType) k)) {
            State build_robot_state = state.get_child_state();
            build_robot(blueprint, &build_robot_state, (ResourceType) k);
            childs.push_back(build_robot_state);
        }
    }

    return childs;
}

uint8_t states_get_max_gems(list<State> states) {
    uint8_t gems = 0;

    for (list<State>::iterator it = states.begin(); it != states.end(); it++) {
        gems = it->gem > gems ? it->gem : gems;
    }

    return gems;
}

uint8_t calculate_max_gems_with_states(Blueprint blueprint, uint8_t time, list<State> states) {

    list<State> current_state = states;

    while (time > 0)
    {
        list<State> next_states;
        for (list<State>::iterator it = current_state.begin(); it != current_state.end(); it++)
        {
            next_states.splice(next_states.end(), get_child_states(blueprint, *it));
        }

        current_state = next_states;
        next_states = list<State>();
        time--;
        printf("%s%d\n", "time is :", time);
    }

    return states_get_max_gems(current_state);
}

uint8_t calculate_max_gems(Blueprint blueprint, uint8_t time) {
    State base_state;
    base_state.time = time;
    list<State> states = {base_state};

    return calculate_max_gems_with_states(blueprint, time, states);
}

int main() {
    // string text = "Blueprint 1: Each ore robot costs 2 ore. Each clay robot costs 4 ore. Each obsidian robot costs 4 ore and 15 clay. Each geode robot costs 2 ore and 20 obsidian.";

    // smatch matches;

    // regex pattern("Blueprint \\d+: Each ore robot costs (?<OreRobotOreCost>\\d+) ore\\. Each clay robot costs (?<ClayRobotOreCost>\\d+) ore\\. Each obsidian robot costs (?<ObsidianRobotOreCost>\\d+) ore and (?<ObsidianRobotClayCost>\\d+) clay\\. Each geode robot costs (?<GeodeRobotOreCost>\\d+) ore and (?<GeodeRobotObsidianCost>\\d+) obsidian\\.");

    // if (regex_search(text, matches, pattern)) {
    //     for (int k = 0; k < matches.size(); k++) {
    //         //printf("%s", matches[k].str());
    //         cout << matches[k].str() << endl;
    //     }
    // }

    uint8_t time = 20;

    Blueprint blueprint;
    blueprint.robot_cost[RESOURCE_TYPE_ORE][RESOURCE_TYPE_ORE] = 4;

    blueprint.robot_cost[RESOURCE_TYPE_CLAY][RESOURCE_TYPE_ORE] = 2;

    blueprint.robot_cost[RESOURCE_TYPE_OBSIDIAN][RESOURCE_TYPE_ORE] = 3;
    blueprint.robot_cost[RESOURCE_TYPE_OBSIDIAN][RESOURCE_TYPE_CLAY] = 14;

    blueprint.robot_cost[RESOURCE_TYPE_GEM][RESOURCE_TYPE_ORE] = 2;
    blueprint.robot_cost[RESOURCE_TYPE_GEM][RESOURCE_TYPE_OBSIDIAN] = 7;

    printf("%s%d", "Max gems : ", calculate_max_gems(blueprint, time));
    printf("\n");
}
