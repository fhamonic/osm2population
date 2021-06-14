#include "io/parse_geojson_buildings.hpp"

namespace IO {
    std::vector<Building> parse_geojson_buildings(const std::filesystem::path & json_file) {
        std::vector<Building> buildings;
        simdjson::ondemand::parser parser;
        auto json = simdjson::padded_string::load(json_file);
        auto doc = parser.iterate(json);

        if(doc.find_field("type") != "FeatureCollection")
            throw std::runtime_error(json_file.filename().string() + " is not of type FeatureCollection");

        for(auto region : doc.find_field("features").get_array()) {
            auto geometry = region.find_field("geometry");
            if(geometry.find_field("type") != "MultiPolygon")
                throw std::runtime_error("region geometry with type != MultiPolygon");
            buildings.emplace_back(detail::parse_geojson_multipolygon<MultipolygonGeo>(geometry.find_field("coordinates").get_array()));
        }
        return buildings;
    }
} // namespace IO
