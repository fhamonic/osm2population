#ifndef PARSE_GEOJSON_BUILDINGS_HPP
#define PARSE_GEOJSON_BUILDINGS_HPP 

#include "io/parse_geojson.hpp"
#include "building.hpp"

namespace IO {
    std::vector<Building> parse_geojson_buildings(const std::filesystem::path & json_file);
} // namespace IO

#endif //PARSE_GEOJSON_BUILDINGS_HPP