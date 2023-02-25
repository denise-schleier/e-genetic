﻿/* Copyright (c) 2022 Krisztián Rugási. Subject to the MIT License. */

#ifndef GA_ENCODING_PERMUTATION_HPP
#define GA_ENCODING_PERMUTATION_HPP

#include "gene_types.hpp"
#include "../core/ga_base.hpp"
#include "../population/candidate.hpp"
#include "../crossover/permutation.hpp"
#include "../mutation/permutation.hpp"
#include <memory>
#include <utility>
#include <cstddef>

namespace genetic_algorithm
{
    /**
    * Genetic algorithm in which the chromosomes encode permutations. \n
    * The genes of the chromosomes are unique unsigned integers on the closed interval [0, chrom_len - 1].
    * 
    * The first and last elements of the permutations are assumed to be unrelated, eg. the permutation
    * A-B-C-D will not be considered equal to the permutation B-C-D-A by the algorithm.
    */
    class PermutationGA final : public GA<PermutationGene>
    {
    public:
        /**
        * Construct a permutation encoded genetic algorithm.
        *
        * @param fitness_function The fitness function used in the algorithm.
        * @param population_size The number of candidates in the population.
        */
        explicit PermutationGA(std::unique_ptr<FitnessFunction<PermutationGene>> fitness_function, size_t population_size = DEFAULT_POPSIZE);

        /**
        * Construct a permutation encoded genetic algorithm.
        *
        * @param fitness_function The fitness function used in the algorithm.
        * @param population_size The number of candidates in the population.
        */
        template<typename F>
        requires FitnessFunctionType<F, PermutationGene> && std::is_final_v<F>
        explicit PermutationGA(F fitness_function, size_t population_size = DEFAULT_POPSIZE) :
            GA(std::make_unique<F>(std::move(fitness_function)), population_size)
        {
            bounds_ = BoundsVector(this->chrom_len(), GeneBounds{ 0_sz, this->chrom_len() - 1 });
            crossover_method(std::make_unique<crossover::perm::Order2>());
            mutation_method(std::make_unique<mutation::perm::Inversion>(0.2));
        }

    private:
        void initialize() override;
        Candidate<GeneType> generateCandidate() const override;
    };

} // namespace genetic_algorithm

#endif // !GA_ENCODING_PERMUTATION_HPP