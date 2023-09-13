#include <gapp.hpp>
#include <iostream>

using namespace gapp;

class Sum : public FitnessFunction<RealGene, 2>
{
    FitnessVector invoke(const Chromosome<RealGene> &x) const override
    {
        // std::cout << "invoke " << x[0] << "," << x[1] << std::endl;
        return {
            100.0 - std::abs(1.0 * x[0] + 1.0 * x[1] - 10.0)
        };
    }
};

int main()
{
    RCGA ga{100};

//    ga.algorithm(algorithm::NSGA3{});
    ga.crossover_method(crossover::real::BLXa{});
    ga.mutation_method(mutation::real::Boundary{ 0.01 });

    for (int i = 0; i < 10; i++)
    {
        auto solutions = ga.solve(Sum{}, Bounds{1.0, 7.0});

        std::cout << "x[0],x[1] = " << solutions[0].chromosome[0] << "," << solutions[0].chromosome[1] << std::endl;
    }
}
