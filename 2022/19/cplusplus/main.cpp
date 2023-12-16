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

enum StateResourceType
{
    STATE_RESOURCE_RESOURCE = 0,
    STATE_RESOURCE_ROBOTS,
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

        uint8_t resource[RESOURCE_TYPE_END][2];

    public:
        State() {
            time = 0;

            for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
                resource[k][STATE_RESOURCE_RESOURCE] = 0;
                resource[k][STATE_RESOURCE_ROBOTS] = 0;
            }

            resource[RESOURCE_TYPE_ORE][STATE_RESOURCE_ROBOTS] = 1;
        }

        State(State *state) {
            
            time = state->time;
            
            for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
                resource[k][STATE_RESOURCE_RESOURCE] = state->resource[k][STATE_RESOURCE_RESOURCE];
                resource[k][STATE_RESOURCE_ROBOTS] = state->resource[k][STATE_RESOURCE_ROBOTS];
            }
        }

        State get_child_state() {
            State child_state(this);

            child_state.time = this->time - 1;

            for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
                child_state.resource[k][STATE_RESOURCE_RESOURCE] += this->resource[k][STATE_RESOURCE_ROBOTS];
            }

            return child_state;
        }

};

// bool cant_be_builded_before(uint8_t *robot_cost, State state) {
//     
//     for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
//         if (state.resource[k][STATE_RESOURCE_RESOURCE] - state.resource[k][STATE_RESOURCE_ROBOTS] < robot_cost[k]) {
//             return false;
//         }
//     }
// 
//     return true;
// }

bool can_build_robot(Blueprint blueprint, State state, ResourceType resource) {

    uint8_t *robot_cost = blueprint.robot_cost[resource];

    for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
        if (robot_cost[k] > state.resource[k][STATE_RESOURCE_RESOURCE]) {
            return false;
        }
    }

    return true;
}

void build_robot(Blueprint blueprint, State *state, ResourceType resource) {

    uint8_t *robot_cost = blueprint.robot_cost[resource];


    for (uint8_t k = RESOURCE_TYPE_START + 1; k < RESOURCE_TYPE_END; k++) {
        state->resource[k][STATE_RESOURCE_RESOURCE] -= robot_cost[k];
    }

    state->resource[resource][STATE_RESOURCE_ROBOTS]++;
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
        gems = it->resource[RESOURCE_TYPE_GEM][STATE_RESOURCE_RESOURCE] > gems ? it->resource[RESOURCE_TYPE_GEM][STATE_RESOURCE_RESOURCE] : gems;
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

    uint8_t time = 19;

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
