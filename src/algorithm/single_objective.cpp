/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#include "single_objective.hpp"
#include "../core/ga_info.hpp"
#include "../utility/functional.hpp"
#include "../utility/utility.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace genetic_algorithm::algorithm
{
    SingleObjective::SingleObjective(SelectionFunction selection) :
        SingleObjective(std::make_unique<selection::Lambda>(std::move(selection)))
    {}

    SingleObjective::SingleObjective(SelectionFunction selection, UpdateFunction updater) :
        SingleObjective(std::make_unique<selection::Lambda>(std::move(selection)), std::make_unique<update::Lambda>(std::move(updater)))
    {}

    void SingleObjective::selection_method(SelectionFunction f)
    {
        if (!f) GA_THROW(std::invalid_argument, "The selection method can't be a nullptr.");

        selection_ = std::make_unique<selection::Lambda>(std::move(f));
    }

    void SingleObjective::update_method(UpdateFunction f)
    {
        if (!f) GA_THROW(std::invalid_argument, "The population update method can't be a nullptr.");

        updater_ = std::make_unique<update::Lambda>(std::move(f));
    }

    void SingleObjective::initializeImpl(const GaInfo& ga)
    {
        if (ga.num_objectives() != 1)
        {
            GA_THROW(std::logic_error, "The number of objectives must be 1 for the single-objective algorithms.");
        }

        selection_->initializeImpl(ga);
    }

    std::vector<size_t> SingleObjective::nextPopulationImpl(const GaInfo& ga, FitnessMatrix::const_iterator first, FitnessMatrix::const_iterator children_first, FitnessMatrix::const_iterator last)
    {
        assert(std::all_of(first, last, detail::is_size(1)));

        if (ga.num_objectives() != 1)
        {
            GA_THROW(std::logic_error, "The number of objectives must be 1 for the single-objective algorithms.");
        }

        return updater_->nextPopulationImpl(ga, first, children_first, last);
    }

} // namespace genetic_algorithm::algorithm