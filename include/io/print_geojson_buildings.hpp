#ifndef PRINT_GEOJSON_BUILDINGS_HPP
#define PRINT_GEOJSON_BUILDINGS_HPP 

#include <filesystem>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "bg_types.hpp"
#include "building.hpp"

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

namespace IO {
    void print_geojson_buildings(const std::vector<Building> & regions, const std::filesystem::path & json_file);
}
#endif //PRINT_GEOJSON_BUILDINGS_HPP