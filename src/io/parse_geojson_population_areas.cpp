#include "io/parse_geojson_population_areas.hpp"

namespace IO {
    std::vector<PopulationArea> parse_geojson_population_areas(const std::filesystem::path & json_file) {
        std::vector<PopulationArea> population_areas;
        simdjson::ondemand::object obj = detail::open_geojson(json_file);

        if(obj.find_field("type") != "FeatureCollection")
            throw std::runtime_error(json_file.filename().string() + " is not of type FeatureCollection");

        for(auto region : obj.find_field("features").get_array()) {
            auto geometry = region.find_field("geometry");
            if(geometry.find_field("type") != "MultiPolygon")
                throw std::runtime_error("region geometry with type != MultiPolygon");
            population_areas.emplace_back(
                detail::parse_geojson_multipolygon<MultipolygonGeo>(geometry["coordinates"].get_array()),
                geometry["properties"].find_field("population").get_double());
        }
        return population_areas;
    }
} // namespace IO
