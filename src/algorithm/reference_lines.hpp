/* Copyright (c) 2022 Kriszti�n Rug�si. Subject to the MIT License. */

#ifndef GA_ALGORITHM_REFERENCE_LINES_HPP
#define GA_ALGORITHM_REFERENCE_LINES_HPP

#include "../utility/math.hpp"
#include <vector>
#include <cstddef>

namespace genetic_algorithm::algorithm::dtl
{
    using math::Point;

    /* Generate n reference points in dim dimensions. */
    std::vector<Point> generateReferencePoints(size_t dim, size_t n);


    /* Generate n random points on a unit simplex in dim dimensions. */
    std::vector<Point> randomSimplexPoints(size_t dim, size_t n);

    /* Generate n quasirandom points on a unit simplex in dim dimensions. */
    std::vector<Point> quasirandomSimplexPoints(size_t dim, size_t n);

} // namespace genetic_algorithm::algorithm::dtl

#endif // !GA_ALGORITHM_REFERENCE_LINES_HPP