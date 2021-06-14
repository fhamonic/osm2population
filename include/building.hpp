#ifndef BUILDING_HPP
#define BUILDING_HPP 

#include "bg_types.hpp"

struct Building {
    MultipolygonGeo multipolygon;
    double area;
    double population;

    template <typename Multipolygon>
    Building(Multipolygon&& multipolygon)
        : multipolygon(std::forward<Multipolygon>(multipolygon))
        , area(boost::geometry::area(this->multipolygon))
        , population(0.0) {}
};

#endif // BUILDING_HPP