//#include "algorithm.h"
#include "algorithm_omp.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <cmath>
#include <set>
#include <algorithm>
#include <math.h>

// Derive from Algorithm{TBB,OMP}, if you are using either
class AlgorithmImpl : public AlgorithmOmp{

private:
    tCapacity** solutionMatrix;

    // This is the entrypoint of your implementation.
    // Feel free to define new memberfunctions and data members
    // The definition of ProblemInstance can be found in algorithm/algorithm.
    tSolution do_run([[maybe_unused]] const ProblemInstance &instance) override {

        //sort the items in decending order (profit/weight)
        tItemList items = instance.items;
        /*

        //dominance functions return true if item x is dominated by item y
        auto standardDominanceFunction = [](tItem x, tItem y) -> bool {return  x.weight >= y.weight && x.value <= y.value;};
        auto multiplyDominanceFunction = [] (tItem x, tItem y) -> bool {size_t weightRatio = (x.weight / y.weight);
                                                                        return (y.weight * weightRatio <= x.weight) && (y.value * weightRatio >= x.value);};
 
        findDominatedItems(indicesOfDominatedItems, items, standardDominanceFunction);
        */
        size_t itemCount, capacity;
        getItemCountAndCapacity(instance, items, itemCount, capacity);

        //Create matrix for the solution table
        for(size_t i = 0; i < itemCount; i++){
            solutionMatrix[i] = new tCapacity[capacity + 1];
        }
        
        //Start the algorithm which fills the entries in the solution table
        knapsack(items, capacity, itemCount);
        
        //Generate the solution vector for the items to take
        tSolution solution;
        getSolution(solution, capacity, itemCount, items);

        //free the allocated memory for the solution table
        for(size_t i = 0; i < itemCount; i++){
            delete [] solutionMatrix[i];
        }

        delete[] solutionMatrix;
        return solution;
    }

    void knapsack(tItemList& items, size_t maxCapacity, size_t itemCount){
        for (size_t row = 0; row < itemCount; row++){
            tWeight itemWeight = items[row].weight;
            tValue itemValue = items[row].value;
        
            // Calculate all entries in the current row
            #pragma omp parallel for
            for (size_t capacity = 0; capacity <=  maxCapacity; capacity++)
            {
                // this is the first item
                if(row == 0){
                    if(itemWeight > capacity){
                        solutionMatrix[row][capacity] = 0;
                    }
                    else{
                        solutionMatrix[row][capacity] = itemValue;
                    }
                }
                else{
                    tCapacity maximumValue = solutionMatrix[row-1][capacity];
                    if(itemWeight > capacity){
                        //here we either can't take the item or it is dominated by another item and we just copy the value before
                    }
                    else{
                        auto currentValue = solutionMatrix[row-1][capacity - itemWeight] + itemValue;

                        if (currentValue > maximumValue){
                            maximumValue = currentValue;
                        }   
                    }
                    solutionMatrix[row][capacity] = maximumValue;

                }
            }
        }
    }

    void getSolution(tSolution &solution, size_t capacity, size_t itemCount, tItemList items){
        for(int itemNr = itemCount-1; itemNr > 0; itemNr--){
            if (capacity == 0)
                break;

            if(solutionMatrix[itemNr][capacity] == solutionMatrix[itemNr-1][capacity]){
                //here we just copied so we did not take the item
            }
            else{
                //if the value is diffrent we took the item
                tWeight itemWeight = items[itemNr].weight;
                if(capacity >= itemWeight){
                    solution.push_back(itemNr);
                    capacity -= itemWeight;
                }
            }
        }

        if(capacity >= items[0].weight)
            solution.push_back(0);
    }

    void getItemCountAndCapacity(const ProblemInstance &instance,tItemList items, size_t& itemCount, size_t& capacity){
        itemCount = items.size();
        capacity = instance.capacity;
    }

  public:
    // Customize the name of the algorithm without whitespaces
    static constexpr auto name = "ClassicDPwithOMP(uqeep)";
    // Change this variable to the AlgorithmType. Possible values are
    // BranchAndBound, DynamicProgramming and TwoList
    static constexpr auto type = Algorithm::Type::DynamicProgramming;

    // Startup work like allocating memory can be done in the constructor.
    // The constructor call is not included in the experiments.
    explicit AlgorithmImpl([[maybe_unused]] const size_t n, const unsigned int threads) : AlgorithmOmp{n, threads} {
        solutionMatrix = new tCapacity*[n+1];
    }
};
