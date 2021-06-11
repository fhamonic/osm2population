
#include "io/parse_geojson.hpp"

namespace bg = boost::geometry;
namespace ba = boost::adaptors;

namespace IO {
    MultipolygonGeo parse_geojson_multipolygon(const std::filesystem::path & json_file) {
        std::vector<Region> regions;
        simdjson::ondemand::parser parser;
        auto json = simdjson::padded_string::load(json_file);
        simdjson::ondemand::document doc = parser.iterate(json);
        simdjson::ondemand::object obj = doc.get_object();

        if(obj.find_field("type") != "MultiPolygon")
            throw std::runtime_error("region geometry with type != MultiPolygon");

        return detail::parse_geojson_multipolygon<MultipolygonGeo>(obj["coordinates"].get_array());
    }

    std::vector<Region> parse_geojson(const std::filesystem::path & json_file) {
        std::vector<Region> regions;
        simdjson::ondemand::parser parser;
        auto json = simdjson::padded_string::load(json_file);
        simdjson::ondemand::document doc = parser.iterate(json);
        simdjson::ondemand::object obj = doc.get_object();

        if(obj.find_field("type") != "FeatureCollection")
            throw std::runtime_error(json_file.filename().string() + " is not of type FeatureCollection");

        for(auto region : obj.find_field("features").get_array()) {
            auto geometry = region.find_field("geometry");

            if(geometry.find_field("type") != "MultiPolygon")
                throw std::runtime_error("region geometry with type != MultiPolygon");

            regions.emplace_back(detail::parse_geojson_multipolygon<MultipolygonGeo>(geometry["coordinates"].get_array()), detail::parse_geojson_properties(region["properties"].get_object()));
        }
        return regions;
    }
} // namespace IO
