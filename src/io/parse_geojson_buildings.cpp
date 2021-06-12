#include "io/parse_geojson_buildings.hpp"

namespace IO {
    std::vector<Building> parse_geojson_buildings(const std::filesystem::path & json_file) {
        std::vector<Building> buildings;
        simdjson::ondemand::object obj = detail::open_geojson(json_file);

        if(obj.find_field("type") != "FeatureCollection")
            throw std::runtime_error(json_file.filename().string() + " is not of type FeatureCollection");

        for(auto region : obj.find_field("features").get_array()) {
            auto geometry = region.find_field("geometry");
            if(geometry.find_field("type") != "MultiPolygon")
                throw std::runtime_error("region geometry with type != MultiPolygon");
            buildings.emplace_back(detail::parse_geojson_multipolygon<MultipolygonGeo>(geometry["coordinates"].get_array()));
        }
        return buildings;
    }
} // namespace IO
