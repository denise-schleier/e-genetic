#include <gapp.hpp>
#include <iostream>

using namespace gapp;

static const double v[] = { 1.0, 2.0, 3.0, 4.0 };

class F1 : public FitnessFunction<PermutationGene, 4>
{
    FitnessVector invoke(const Chromosome<PermutationGene> &x) const override
    {
        return {
            100.0 - 2.0 * v[x[0]] - 4.0 * v[x[1]] - 7.0 * v[x[2]] - 9.0 * v[x[3]]
        };
    }
};

int main()
{
    PermutationGA ga{100};

    for (int i = 0; i < 10; i++)
    {
        auto solutions = ga.solve(F1{});

        std::cout << " = " << solutions[0].chromosome[0] << ", " << solutions[0].chromosome[1] << ", " << solutions[0].chromosome[2] << ", " << solutions[0].chromosome[3] << std::endl;
    }
}
