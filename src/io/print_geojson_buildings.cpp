
#include "io/print_geojson_buildings.hpp"

namespace bg = boost::geometry;
namespace ba = boost::adaptors;

namespace IO {
    void print_geojson_buildings(const std::vector<Building> & buildings, const std::filesystem::path & json_file) {
        std::ofstream json(json_file);
        json << std::setprecision(std::numeric_limits<double>::max_digits10);

        json << "{\"type\":\"FeatureCollection\",\"features\":[";
        for(const auto & e : buildings | ba::indexed(0)) {
            json << "{\"type\":\"Feature\",\"geometry\":{\"type\":\"MultiPolygon\",\"coordinates\":" 
                << bg::dsv(e.value().multipolygon, ",", "[", "]", ",", "[", "]", ",")
                << "},\"properties\":{\"population\":\""+std::to_string(e.value().population)+"\",}}" 
                << (e.index()+1 != static_cast<std::ptrdiff_t>(buildings.size()) ? "," : "");
        }
        json << "]}";
    }
}
