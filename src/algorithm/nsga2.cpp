/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#include "nsga2.hpp"
#include "nd_sort.hpp"
#include "../core/ga_info.hpp"
#include "../population/population.hpp"
#include "../utility/algorithm.hpp"
#include "../utility/functional.hpp"
#include "../utility/math.hpp"
#include "../utility/rng.hpp"
#include "../utility/utility.hpp"
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>
#include <utility>
#include <cstddef>
#include <cassert>
#include <stdexcept>

namespace genetic_algorithm::algorithm
{
    using namespace dtl;

    /* Calculate the crowding distances of the solutions in pfronts. */
    static std::vector<double> crowdingDistances(FitnessMatrix::const_iterator fmat, FitnessMatrix::const_iterator last, ParetoFronts pfronts)
    {
        assert(std::distance(fmat, last) >= 0);

        std::vector<double> cdistances(last - fmat);

        using Iter     = ParetoFronts::iterator;
        using IterPair = std::pair<Iter, Iter>;

        auto front_bounds = paretoFrontBounds(pfronts);

        for (size_t dim = 0; dim < fmat->size(); dim++)
        {
            for (const auto& [front_first, front_last] : front_bounds)
            {
                std::sort(front_first, front_last, [&](const FrontInfo& lhs, const FrontInfo& rhs) noexcept
                {
                    return fmat[lhs.idx][dim] < fmat[rhs.idx][dim]; // ascending
                });

                size_t first_idx = front_first->idx;
                size_t last_idx = (front_last - 1)->idx;

                double finterval = std::max(fmat[last_idx][dim] - fmat[first_idx][dim], 1E-6);

                cdistances[first_idx] = math::inf<double>;
                for (auto it = front_first + 1; it < front_last - 1; ++it)
                {
                    size_t next = std::next(it)->idx;
                    size_t prev = std::prev(it)->idx;
                    cdistances[it->idx] += (fmat[next][dim] - fmat[prev][dim]) / finterval;
                }
                cdistances[last_idx] = math::inf<double>;
            }
        }

        return cdistances;
    }

    void NSGA2::initializeImpl(const GaInfo& ga)
    {
        if (ga.num_objectives() <= 1)
        {
            GA_THROW(std::logic_error, "The number of objectives must be greater than 1 for the NSGA-II algorithm.");
        }

        auto& fmat = ga.fitness_matrix();
        auto pfronts = nonDominatedSort(fmat.begin(), fmat.end());

        ranks_ = paretoRanks(pfronts);
        dists_ = crowdingDistances(fmat.begin(), fmat.end(), std::move(pfronts));
    }

    size_t NSGA2::selectImpl(const GaInfo&, const FitnessMatrix& pop) const
    {
        assert(!pop.empty() && pop.size() == ranks_.size());

        const size_t idx1 = rng::randomIdx(pop);
        const size_t idx2 = rng::randomIdx(pop);

        const bool first_is_better =
            (ranks_[idx1] != ranks_[idx2]) ?
                ranks_[idx1] < ranks_[idx2] :
                dists_[idx1] > dists_[idx2];

        return first_is_better ? idx1 : idx2;
    }

    std::vector<size_t> NSGA2::nextPopulationImpl(const GaInfo& ga,
                                              FitnessMatrix::const_iterator parents_first,
                                              FitnessMatrix::const_iterator children_first,
                                              FitnessMatrix::const_iterator children_last)
    {
        const size_t popsize = ga.population_size();

        assert(ga.num_objectives() > 1);
        assert(size_t(children_first - parents_first) == popsize);
        assert(std::all_of(parents_first, children_last, [&ga](const FitnessVector& f) { return f.size() == ga.num_objectives(); }));

        GA_UNUSED(children_first);

        auto pfronts = nonDominatedSort(parents_first, children_last);
        auto [partial_front_first, partial_front_last] = findPartialFront(pfronts.begin(), pfronts.end(), popsize);

        /* Crowding distances of just the partial front for sorting. */
        dists_ = crowdingDistances(parents_first, children_last, ParetoFronts(partial_front_first, partial_front_last));

        std::sort(partial_front_first, partial_front_last,
        [this](const FrontInfo& lhs, const FrontInfo& rhs) noexcept
        {
            return dists_[lhs.idx] > dists_[rhs.idx]; // descending
        });

        /* Crowding distances of all of the next generation's solutions. */
        dists_ = crowdingDistances(parents_first, children_last, ParetoFronts(pfronts.begin(), pfronts.begin() + popsize));
        dists_.resize(popsize);

        std::vector<size_t> new_pop(popsize);

        /* Add the first popsize elements from pfronts to the next pop. */
        for (size_t i = 0; i < popsize; i++)
        {
            new_pop[i] = pfronts[i].idx;
            ranks_[i] = pfronts[i].rank;
        }

        return new_pop;
    }

    std::optional<std::vector<size_t>> NSGA2::optimalSolutionsImpl(const GaInfo&) const
    {
        return detail::find_indices(ranks_, detail::equal_to(0_sz));
    }

} // namespace genetic_algorithm::algorithm