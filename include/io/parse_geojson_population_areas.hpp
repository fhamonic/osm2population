#ifndef PARSE_GEOJSON_POPULATION_AREA_HPP
#define PARSE_GEOJSON_POPULATION_AREA_HPP 

#include <string>

#include "io/parse_geojson.hpp"
#include "population_area.hpp"

namespace IO {
    std::vector<PopulationArea> parse_geojson_population_areas(const std::filesystem::path & json_file);
} // namespace IO

#endif //PARSE_GEOJSON_POPULATION_AREA_HPP