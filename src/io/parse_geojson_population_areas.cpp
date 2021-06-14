#include "io/parse_geojson_population_areas.hpp"

namespace IO {
    std::vector<PopulationArea> parse_geojson_population_areas(const std::filesystem::path & json_file) {
        std::vector<PopulationArea> population_areas;
        simdjson::ondemand::parser parser;
        auto json = simdjson::padded_string::load(json_file);
        auto doc = parser.iterate(json);

        if(doc.find_field("type") != "FeatureCollection")
            throw std::runtime_error(json_file.filename().string() + " is not of type FeatureCollection");

        for(auto region : doc.find_field("features").get_array()) {
            auto geometry = region["geometry"];
            if(geometry.find_field("type") != "MultiPolygon")
                throw std::runtime_error("region geometry with type != MultiPolygon");

            population_areas.emplace_back(detail::parse_geojson_multipolygon<MultipolygonGeo>(geometry.find_field("coordinates").get_array()));
            population_areas.back().population = std::stod(std::string(region["properties"].find_field("population").get_string().value()));
        }
        return population_areas;
    }
} // namespace IO
