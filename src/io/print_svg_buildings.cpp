
#include "io/print_svg_buildings.hpp"

namespace bg = boost::geometry;
namespace ba = boost::adaptors;

namespace IO {
    void print_svg_buildings(const std::vector<Building> & buildings, const std::filesystem::path & svg_file) {
        std::ofstream svg(svg_file);
        bg::svg_mapper<PointGeo> mapper(svg, 1500, 1500);

        double max_population = *boost::max_element(buildings | ba::transformed([](auto & b){ return b.population; }));

        for(auto & b : buildings)
            mapper.add(b.multipolygon);
        for(auto & b : buildings) {
            int red =  b.population / max_population * 255;
            mapper.map(b.multipolygon, "fill-opacity:0.5;fill:rgb(" + std::to_string(red) + ",0,0);stroke:rgb(0,0,0);stroke-width:0");
        }
    }
}