﻿/* Copyright (c) 2022 Krisztián Rugási. Subject to the MIT License. */

#ifndef GA_CROSSOVER_BASE_DECL_HPP
#define GA_CROSSOVER_BASE_DECL_HPP

#include "../population/candidate.hpp"
#include "../utility/bounded_value.hpp"
#include <vector>
#include <utility>

namespace genetic_algorithm
{
    template<typename T>
    class GA;

} // namespace genetic_algorithm

/** Crossover operators used in the algorithms. */
namespace genetic_algorithm::crossover
{
    /**
    * Base class used for all of the crossover operators.
    * Every crossover operator takes 2 Candidates (the parents), and creates 2 children from these parents.
    * The crossover operation is performed on the 2 parents with a set (pc) probability only, the rest of the time
    * the returned children will be the same as the parents. \n
    * The generated children's chromosome sizes may be different from eachother, and also from the parent's chromosomes.
    */
    template<typename T>
    class Crossover
    {
    public:
        using GeneType = T;

        /** Create a crossover operator with a crossover probability of 0.8. */
        constexpr explicit Crossover() noexcept :
            pc_(0.8_p) {}

        /**
        * Create a crossover operator.
        *
        * @param pc The crossover probability. Must be in the closed interval [0.0, 1.0].
        */
        constexpr explicit Crossover(Probability pc) noexcept :
            pc_(pc) {}

        /**
        * Set the crossover rate used by the operator.
        *
        * @param pc The crossover probability. Must be in the closed interval [0.0, 1.0].
        */
        constexpr void crossover_rate(Probability pc) noexcept { pc_ = pc; }

        /** @returns The crossover rate set for the operator. */
        [[nodiscard]]
        constexpr Probability crossover_rate() const noexcept { return pc_; }

        /**
        * Perform the crossover operation on 2 individuals with the set probability.
        *
        * @param ga The genetic algorithm the crossover operator is being used in.
        * @param parent1 The first parent.
        * @param parent2 The second parent.
        * @returns The pair of children resulting from the crossover.
        */
        CandidatePair<T> operator()(const GA<T>& ga, const Candidate<T>& parent1, const Candidate<T>& parent2) const;


        /** Destructor. */
        virtual ~Crossover()                    = default;

    protected:

        Crossover(const Crossover&)             = default;
        Crossover(Crossover&&)                  = default;
        Crossover& operator=(const Crossover&)  = default;
        Crossover& operator=(Crossover&&)       = default;

    private:

        /* The actual crossover function. Generates 2 new Candidates from the parents (not with some probability). */
        virtual CandidatePair<T> crossover(const GA<T>& ga, const Candidate<T>& parent1, const Candidate<T>& parent2) const = 0;

        Probability pc_;   /* The probability of performing the crossover operation on the parents. */
    };

} // namespace genetic_algorithm::crossover

#endif // !GA_CROSSOVER_BASE_DECL_HPP