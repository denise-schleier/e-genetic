#include <gapp.hpp>
#include <iostream>

using namespace gapp;

class F1 : public FitnessFunction<BinaryGene, 4>
{
    FitnessVector invoke(const Chromosome<BinaryGene> &x) const override
    {
        /*
                std::cout << "invoke " <<
                    (x[0]?1:0) << ", " <<
                    (x[1]?1:0) << ", " <<
                    (x[2]?1:0) << ", " <<
                    (x[3]?1:0) << std::endl;
        */
        return {
            100.0 - x[0] + x[1] - x[2] + x[3]};
    }
};

int main()
{
    BinaryGA ga{100};

    //    ga.algorithm(algorithm::NSGA3{});
    //    ga.crossover_method(crossover::binary::BLXa{});
    //    ga.mutation_method(mutation::binary::Boundary{ 0.01 });

    for (int i = 0; i < 10; i++)
    {
        auto solutions = ga.solve(F1{});

        std::cout << " = " << solutions[0].chromosome[0] << ", " << solutions[0].chromosome[1] << ", " << solutions[0].chromosome[2] << ", " << solutions[0].chromosome[3] << std::endl;
    }
}
