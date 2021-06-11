#include <execution> // std::execution::par
#include <filesystem>  // filesystem::path
#include <fstream>  // ofstream
#include <iostream> // std::cout, std::cerr

#include "bg_types.hpp"
namespace bg = boost::geometry;
#include <boost/geometry/srs/epsg.hpp>
#include <boost/geometry/srs/projection.hpp>
namespace srs = bg::srs;
#include <boost/geometry/index/rtree.hpp>
namespace bgi = boost::geometry::index;
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
namespace ba = boost::adaptors;
#include <boost/program_options.hpp>
namespace bpo = boost::program_options;

#include "region.hpp"
#include "io/parse_geojson.hpp"

#include "chrono.hpp"

struct RegionInfos {
    double qualityCoef;
    double probConnectionPerMeter;
    int overrideLevel;

    public:
        RegionInfos() = default;
        RegionInfos(double qualityCoef, double probConnectionPerMeter, int overrideLevel)
            : qualityCoef(qualityCoef)
            , probConnectionPerMeter(probConnectionPerMeter)
            , overrideLevel(overrideLevel) {}
};

static bool process_command_line(int argc, char* argv[], 
    std::filesystem::path & buildings_file, std::filesystem::path & population_file, std::filesystem::path & output_file, std::filesystem::path & area_file,
    bool & generate_svg, bool & area_provided) {
    try {
        bpo::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("buildings,b", bpo::value<std::filesystem::path>(&buildings_file)->required(), "geojson feature collection file describing buildings")
            ("population,p", bpo::value<std::filesystem::path>(&population_file)->required(), "geojson feature collection file describing population areas")
            ("output,o", bpo::value<std::filesystem::path>(&output_file), "set the output geojson file path")
            ("area,a", bpo::value<std::filesystem::path>(&area_file), "geojson multipolygon file describing the area of interest")
            ("svg", "generate the svg file of the result")
        ;
        bpo::positional_options_description p;
        p.add("buildings", 1).add("population", 1).add("output", 1);
        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        if(vm.count("help")) {
            std::cout << desc << "\n";
            return false;
        }
        bpo::notify(vm); 
        generate_svg = (vm.count("svg") > 0);
        area_provided = (vm.count("area") > 0);
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::filesystem::path buildings_file;
    std::filesystem::path population_file;
    std::filesystem::path output_file;
    std::filesystem::path area_file;
    bool generate_svg;
    bool area_provided;

    bool valid_command = process_command_line(argc, argv, buildings_file, population_file, output_file, area_file, generate_svg, area_provided);
    if(!valid_command)
        return EXIT_FAILURE;

    Chrono chrono;

    std::vector<Region> buildings = IO::parse_geojson(buildings_file);
    std::vector<Region> population_areas = IO::parse_geojson(population_file);

    if(area_provided) {
        MultipolygonGeo search_area = IO::parse_geojson_multipolygon(area_file);
        buildings.erase(
            std::remove_if(buildings.begin(), buildings.end(), [&search_area](auto & r) { return !bg::intersects(search_area, r.multipolygon); }),
            buildings.end());
        population_areas.erase(
            std::remove_if(population_areas.begin(), population_areas.end(), [&search_area](auto & r) { return !bg::intersects(search_area, r.multipolygon); }),
            population_areas.end());
    }

    std::cout << "parse regions in " << chrono.lapTimeMs() << " ms" << std::endl;
    
    std::vector<double> weighted_buildings(buildings.size(), 0.0);
    
    std::cout << "raw_regions to regions in " << chrono.lapTimeMs() << " ms" << std::endl;

    using RTree = bgi::rtree<std::pair<BoxGeo,size_t>, bgi::rstar<16,4>>;
    RTree rtree(regions | ba::indexed(0) | ba::transformed([](const auto& e) {
        return std::make_pair(bg::return_envelope<BoxGeo>(e.value().first), e.index());
    }));

    std::cout << "constructed R-tree in " << chrono.lapTimeMs() << " ms" << std::endl;

    
    
    return EXIT_SUCCESS;
}