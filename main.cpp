#include "algorithm/algorithm.h"
#include "algorithm/algorithm_impl.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

constexpr char csv_sep = ',';

bool checkCorrectness(const Algorithm::ProblemInstance &instance,
                      const Algorithm::tCapacity solution,
                      Algorithm::tSolution &answer) {
    if(answer.size() > instance.items.size()) {
        return false;
    }
    Algorithm::tCapacity totalWeight = 0;
    Algorithm::tValue totalValue = 0;
    for(const auto &index : answer) {
        if(index >= instance.items.size()) {
            return false;
        }
        totalWeight += instance.items[index].weight;
        totalValue += instance.items[index].value;
    }

    if(totalWeight > instance.capacity || totalValue != solution) {
        return false;
    }

    std::sort(answer.begin(), answer.end());
    return std::adjacent_find(answer.begin(), answer.end()) == answer.end();
}

template<typename LogFunc>
auto runInstance(const Algorithm::ProblemInstance &instance,
                 const unsigned int reps,
                 const unsigned int p_min,
                 const unsigned int p_max,
                 LogFunc log,
                 Algorithm::tValue solution) {
    using namespace std::chrono_literals;
    bool globalCorrectness = true;
    auto total_time = 0ms;

    for(unsigned int p = p_min; p <= p_max; p <<= 1) {
        auto time_per_thread = 0ms;
        bool correct = true;
        for(unsigned int r = 0; r < reps; ++r) {
            auto algorithm = AlgorithmImpl{instance.items.size(), p};
            auto [answer, time] = algorithm.run_timed(instance);
            if(!checkCorrectness(instance, solution, answer)) {
                correct = false;
                globalCorrectness = false;
            }
            time_per_thread += time;
        }
        total_time += time_per_thread;
        log(correct, p, time_per_thread / reps);
    }

    return std::make_pair(globalCorrectness, total_time);
}

std::optional<Algorithm::ProblemInstance> readKPLibInstance(const std::filesystem::path &path) {
    std::ifstream file(path);
    if(!file) {
        return {};
    }
    size_t n;
    file >> n;
    if(!file || file.eof()) {
        return {};
    }

    auto instance = Algorithm::ProblemInstance{};
    instance.items.reserve(n);
    file >> instance.capacity;
    if(!file || (n > 0 && file.eof())) {
        return {};
    }

    for(size_t i = 0; i < n; ++i) {
        if(!file || file.eof()) {
            return {};
        }
        auto item = Algorithm::tItem{};
        file >> item.value;
        if(!file || file.eof()) {
            return {};
        }
        file >> item.weight;
        if(!file) {
            return {};
        }
        instance.items.push_back(item);
    }
    return instance;
}

bool addToPathList(const std::filesystem::path &path_to_list,
                   std::vector<std::pair<std::filesystem::path, Algorithm::tValue>> &instance_list) {
    std::ifstream ss{path_to_list};
    if(!ss) {
        return false;
    }
    std::filesystem::path p;
    bool for_bnb, for_dyn, for_tl;
    Algorithm::tValue s;
    while(ss >> p && ss >> for_bnb && ss >> for_dyn && ss >> for_tl && ss >> s) {
        if (for_bnb == 0 && AlgorithmImpl::type == Algorithm::Type::BranchAndBound) {
          continue;
        }
        if (for_dyn == 0 && AlgorithmImpl::type == Algorithm::Type::DynamicProgramming) {
          continue;
        }
        if (for_tl == 0 && AlgorithmImpl::type == Algorithm::Type::TwoList) {
          continue;
        }
        instance_list.emplace_back(p, s);
    }

    return true;
}

int main(int argc, char *argv[]) {
    const unsigned int reps = 5;
    const unsigned int p_min = 1;
    const unsigned int p_max = 32;

    using namespace std::chrono_literals;
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " INSTANCE_LIST" << std::endl;
        return 1;
    }

    std::vector<std::pair<std::filesystem::path, Algorithm::tValue>> instance_paths;

    bool success = addToPathList(argv[1], instance_paths);

    if(!success) {
        std::cerr << "Unable to open instance list " << argv[1] << std::endl;
        return 2;
    }

    std::cout << "# " << AlgorithmImpl::name << " ";
    switch(AlgorithmImpl::type) {
        case Algorithm::Type::DynamicProgramming: std::cout << "dyn"; break;
        case Algorithm::Type::BranchAndBound: std::cout << "bnb"; break;
        case Algorithm::Type::TwoList: std::cout << "tl"; break;
        default: std::cout << "other";
    }
    std::cout << " " << instance_paths.size() << '\n';

    std::cout << "inst" << csv_sep << "correct" << csv_sep << "n" << csv_sep << "p" << csv_sep << "t" << std::endl;

    unsigned int numInstances = 0;
    unsigned int numInstancesCorrect = 0;
    auto totalDuration = 0ms;
    auto log = [](std::filesystem::path path, size_t num_items) {
        return [=](bool correct, unsigned int num_threads, std::chrono::milliseconds duration) {
            std::cout << path.native() << csv_sep << correct << csv_sep << num_items << csv_sep << num_threads
                      << csv_sep << duration.count() << '\n';
        };
    };

    for(const auto &[path, solution] : instance_paths) {
        auto instance = readKPLibInstance(path);
        if(!instance) {
            std::cerr << "Warning: Skipping invalid kplib instance: " << path << std::endl;
            continue;
        }
        ++numInstances;
        std::cerr << "Running Instance " << numInstances << " ... " << std::endl;
        auto [correct, duration] = runInstance(*instance, reps, p_min, p_max, log(path, instance->items.size()), solution);
        if(correct) {
            ++numInstancesCorrect;
            std::cerr << "Correct solution for instance " << numInstances << std::endl;
        } else {
            std::cerr << "Wrong solution for instance " << numInstances << std::endl;
        }
        totalDuration += duration;
    }
    std::cout.flush();

    std::cerr << "Solved " << numInstancesCorrect << " out of " << numInstances << " correct" << std::endl;
    std::cerr << "Total time for all runs: " << totalDuration.count() << " ms" << std::endl;

    return 0;
}
