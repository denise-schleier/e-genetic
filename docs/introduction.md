﻿
1. **Introduction**  
2. [Fitness functions](fitness-functions.md)  
3. [Encodings](encodings.md)  
4. [Algorithms](algorithms.md)  
5. [Genetic operators](genetic-operators.md)  
6. [Stop conditions](stop-conditions.md)  
7. [Metrics](metrics.md)  
8. [Miscellaneous](miscellaneous.md)  

------------------------------------------------------------------------------------------------

# Introduction

This short introduction will present the basic usage of the library through an example
of solving a simple single-objective optimization problem.

## The problem

In this example, our goal will be to find the minimum of a simple sine function in a given
interval:

```math
\min_{x}\ f(x) = \sin x
```
```math
x \in [-3.14,\ 0.0]
```

We know that the minimum of the function in this interval is
$` f(\hat{x}) = -1 `$ at $` \hat{x} = -\frac{\pi}{2} \approx -1.5708 `$.

Since the library assumes that the objective function should be maximized,
the problem needs to be rewritten as an equivalent maximization problem. This can be
done by simply multiplying the function by -1:

```math
\max_{x}\ \hat{f}(x) = -\sin x
```
```math
x \in [-3.14,\ 0.0]
```

## The fitness function

The first step of solving the problem is defining a fitness function that will be used
in the genetic algorithm. There are a number of things to consider when defining this function:

* the encoding/representation of the solutions
* the number of variables
* the length of the chromosomes
* the number of objectives

For this problem, we could either represent the solutions as binary strings and use the
binary-encoded genetic algorithm, or represent them directly using floating-point numbers
and use the real-encoded GA. Since the second is a more natural representation
of the problem, we will use that in this example.

The only variable we have is *x*, so the number of variables will be 1, and the only objective
function is *f(x)*, so the number of objectives will also be 1.

Because we will be using the real-encoded algorithm, there is no difference between
the number of variables and the chromosome length, as each gene of the chromosome will represent
a single variable.  
(If we were using the binary-encoded GA, each variable would be represented
by multiple binary genes in the chromosome, making the chromosome length different from the
number of variables.)

```cpp
#include <gapp/gapp.hpp> // include everything from the library
#include <cmath>         // for std::sin

using namespace gapp;

// fitness functions should be derived from the FitnessFunction class and
// specify the encoding type and the chromosome length as template parameters
// (or use the FitnessFunctionBase class if the chromosome length is not known at compile time)
class SinX : public FitnessFunction<RealGene, /* chrom_length = */ 1> 
{
    // define the fitness function by overriding the invoke method
    FitnessVector invoke(const Chromosome<RealGene>& x) const override
    {
        // the number of objectives will be deduced from the size of the returned fitness vector
        return { -std::sin(x[0]) };
    }
};
```

## The genetic algorithm

After the objective function is defined, the next step is to create an instance of the
appropriate genetic algorithm class. The main GA classes correspond to the available
encoding types, and they are:

* BinaryGA (binary-encoded GA)
* RCGA (real-encoded GA)
* IntegerGA (integer-encoded GA)
* PermutationGA (for combinatorial problems)

Since the fitness function we defined uses real-encoding, we need to use the RCGA class.  
All of the parameters of the GA classes have default values, which should be sufficient
for this problem, but we will specify a population size to use:

```cpp
// create a real-encoded genetic algorithm
RCGA GA{ /* population_size = */ 100 };
```

## Running the algorithm

Once we have the GA, we can use it to find the maximum of our fitness function by using its
`solve` method. The function expects an instance of the fitness function and the interval
to look for the maximum on in the case of the real-encoded GA:

```cpp
// find the maximum of the SinX fitness function in the interval [-3.14, 0]
auto solutions = GA.solve(SinX{}, Bounds{ -3.14, 0.0 });
```

The method returns a `Population` containing the optimal solutions found by the GA.
This population is guaranteed to contain at least one solution, but it may also have
more.
We can print the solution to verify it:

```cpp
std::cout << "The minimum of sin(x) in [-3.14, 0.0] is at x = " << solutions[0].chromosome[0] << "\n";
std::cout << "The minimal value of sin(x) in [-3.14, 0.0] is f(x) = " << -solutions[0].fitness[0];
```

Possible console output:

```text
The minimum of sin(x) in [-3.14, 0.0] is at x = -1.57081
The minimal value of sin(x) in [-3.14, 0.0] is f(x) = 1
```

## Customizing the algorithm

In a lot of cases, running the algorithm with the default parameters might not be
what you want. You can customize all parts of the GAs, we will show some examples here.

For the single-objective algorithms, you can specify both a selection method and a
population replacement method that should be used:

```cpp
GA.algorithm(algorithm::SingleObjective{ selection::Tournament{}, replacement::Elitism{} });
```

The crossover and mutation methods can also be changed, but these must be appropriate for
the encoding method that is used:

```cpp
GA.crossover_method(crossover::real::Wright{ /* crossover_rate = */ 0.8 });
GA.mutation_method(mutation::real::Gauss{ /* mutation_rate = */ 0.1 });
```

When you run the algorithm, you might also want to specify a different limit for
how many generations it should run for. You can do this either in the `solve` method
or just use the setter the GAs provide:

```cpp
GA.max_gen(1000); // run the GA for 1000 generations at most
```

In addition to specifying the maximum number of generations, you can also specify
an early-stop condition that will stop the run early if some condition is met.
For example we could stop the run if the best fitness value of the population hasn't
improved significantly for over 10 consecutive generations, and the mean fitness of the
population has also not improved for 2 generations:

```cpp
GA.stop_condition(stopping::FitnessBestStall{ 10 } && stopping::FitnessMeanStall{ 2 });
```

We can run the GA again with these modifications:

```cpp
auto solutions = GA.solve(SinX{}, Bounds{ -3.14, 0.0 });

std::cout << "The genetic algorithm ran for " << GA.generation_cntr() << " generations.\n";
std::cout << "The minimum of sin(x) in [-3.14, 0.0] is at x = " << solutions[0].chromosome[0] << "\n";
std::cout << "The minimal value of sin(x) in [-3.14, 0.0] is f(x) = " << -solutions[0].fitness[0];
```

Possible console output:

```text
The genetic algorithm ran for 15 generations.
The minimum of sin(x) in [-3.14, 0.0] is at x = -1.5708
The minimal value of sin(x) in [-3.14, 0.0] is f(x) = -1
```

## Tracking the evolution process

In addition to simply running a genetic algorithm, you might also want to track some
properties of the population throughout the generations of a run. You can do this using
the metrics provided by the library.  
For example to track the mean and maximum fitness value of each generation:

```cpp
GA.track(metrics::FitnessMean{}, metrics::FitnessMax{});
```

and then after the run you can access these metrics:

```cpp
auto fmean = GA.get_metric<metrics::FitnessMean>();
auto fmax = GA.get_metric<metrics::FitnessMax>();
// fmean[0][0] -> The mean value of the first fitness function in the first generation.
```

These metrics are returned as a matrix, each row representing a generation,
and each column representing an objective. In this case the matrix only has a single column
since we are solving a single-objective problem.

Instead of looking at the metrics after the run has finished, we can also print them out
in each generation using a callback function:

```cpp
std::cout << "Generation | Fitness mean | Fitness max\n";
GA.on_generation_end([](const GaInfo& ga)
{
    const size_t generation = ga.generation_cntr();
    const double fmean = ga.get_metric<metrics::FitnessMean>()[generation][0];
    const double fmax  = ga.get_metric<metrics::FitnessMax>()[generation][0];
    std::cout << std::format("   {}\t|\t{:.6f} |\t{:.6f}\n", generation, fmean, fmax);
});
```

Possible console output:

```text
Generation | Fitness mean | Fitness max
   0    |       0.627118 |      0.999994
   1    |       0.724853 |      0.999994
   2    |       0.875917 |      0.999994
   3    |       0.949242 |      0.999996
   4    |       0.956357 |      1.000000
   5    |       0.970961 |      1.000000
   6    |       0.982970 |      1.000000
   7    |       0.978700 |      1.000000
   8    |       0.992256 |      1.000000
   9    |       0.978460 |      1.000000
   10   |       0.979815 |      1.000000
   11   |       0.991481 |      1.000000
   12   |       0.995733 |      1.000000
   13   |       0.982088 |      1.000000
   14   |       0.989876 |      1.000000
```

------------------------------------------------------------------------------------------------
