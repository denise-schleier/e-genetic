﻿/* Copyright (c) 2022 Krisztián Rugási. Subject to the MIT License. */

#ifndef GA_ENCODING_INTEGER_HPP
#define GA_ENCODING_INTEGER_HPP

#include "gene_types.hpp"
#include "../core/ga_base.hpp"
#include "../core/fitness_function.hpp"
#include "../population/candidate.hpp"
#include "../crossover/integer.hpp"
#include "../mutation/integer.hpp"
#include "../utility/bounded_value.hpp"
#include <concepts>
#include <memory>
#include <utility>
#include <cstddef>

namespace genetic_algorithm
{
    /**
    * Integer encoded genetic algorithm. \n
    * Similar to the @ref BinaryGA, but the values of the genes can be any integer in a closed interval, not just 0 or 1.
    * The interval is specified by the gene bounds. \n
    * The algorithm also uses a modified mutation function with swaps and inversions.
    */
    class IntegerGA final : public GA<IntegerGene>
    {
    public:
        /**
        * Construct an integer encoded genetic algorithm.
        *
        * @param fitness_function The fitness function used in the algorithm. Can't be a nullptr.
        * @param bounds The boundaries of the genes (their min and max values).
        * @param population_size The number of candidates in the population. Must be at least 1.
        */
        IntegerGA(std::unique_ptr<FitnessFunctionBase<IntegerGene>> fitness_function, GeneBounds<GeneType> bounds, Positive<size_t> population_size = DEFAULT_POPSIZE);

        /**
        * Construct an integer encoded genetic algorithm.
        *
        * @param fitness_function The fitness function used in the algorithm.
        * @param bounds The boundaries of the genes (their min and max values).
        * @param population_size The number of candidates in the population. Must be at least 1.
        */
        template<typename F>
        requires std::derived_from<F, FitnessFunctionBase<GeneType>> && std::is_final_v<F>
        IntegerGA(F fitness_function, GeneBounds<GeneType> bounds, Positive<size_t> population_size = DEFAULT_POPSIZE) :
            GA(std::make_unique<F>(std::move(fitness_function)), population_size)
        {
            gene_bounds(bounds);
            crossover_method(std::make_unique<crossover::integer::TwoPoint>());
            mutation_method(std::make_unique<mutation::integer::Uniform>(1.0 / chrom_len()));
        }

        /**
        * Set the lower and upper bounds of the integer genes used. \n
        * The lower bound can't be higher than the upper bound. \n
        * 
        * With bounds = { 0, 1 }, the IntegerGA is effectively the same as the BinaryGA.
        *
        * @param bounds The lower and upper boundaries of the genes.
        */
        void gene_bounds(GeneBounds<GeneType> bounds) noexcept;

        using GA::gene_bounds;

    private:
        void initialize() override;
        Candidate<GeneType> generateCandidate() const override;
    };

} // namespace genetic_algorithm

#endif // !GA_ENCODING_INTEGER_HPP