#ifndef POPULATION_AREA_HPP
#define POPULATION_AREA_HPP 

#include "bg_types.hpp"

struct PopulationArea {
    MultipolygonGeo multipolygon;
    double population;
    double buldingAreaSum;

    template <typename Multipolygon>
    PopulationArea(Multipolygon&& multipolygon, double population)
        : multipolygon(std::forward<Multipolygon>(multipolygon))
        , population(population)
        , buldingAreaSum(0.0) {}
};

#endif // POPULATION_AREA_HPP