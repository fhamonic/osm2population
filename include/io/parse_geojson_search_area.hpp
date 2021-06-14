#ifndef PARSE_GEOJSON_SEARCH_AREA_HPP
#define PARSE_GEOJSON_SEARCH_AREA_HPP 

#include "io/parse_geojson.hpp"
#include "building.hpp"

namespace IO {
    template <typename Multipolygon>
    Multipolygon parse_geojson_search_area(const std::filesystem::path & json_file) {
        simdjson::ondemand::parser parser;
        auto json = simdjson::padded_string::load(json_file);
        simdjson::ondemand::document doc = parser.iterate(json);
        simdjson::ondemand::object obj = doc.get_object();
        return detail::parse_geojson_multipolygon<Multipolygon>(obj.find_field("coordinates"));
    }
} // namespace IO

#endif //PARSE_GEOJSON_SEARCH_AREA_HPP