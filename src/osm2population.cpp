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
    std::filesystem::path & input_file, std::filesystem::path & output_file, std::filesystem::path & area_file,
    int & hexagons_resolution, bool & generate_svg, bool & area_provided, bool & graphviz) {
    try {
        bpo::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input,i", bpo::value<std::filesystem::path>(&input_file)->required(), "set input geojson file")
            ("output,o", bpo::value<std::filesystem::path>(&output_file)->required(), "set output geojson file")
            ("area,a", bpo::value<std::filesystem::path>(&area_file), "set area to process geojson geometry file, if not precised the whole box area is processed")
            ("hexagons-level,l", bpo::value<int>(&hexagons_resolution)->required(), "set h3 hexagons resolution")
            ("svg", "generate the svg file of the result regions")
            ("graphviz", "generate a vizualization of the graph produced")
        ;
        bpo::positional_options_description p;
        p.add("input", 1).add("output", 1).add("hexagons-level", 1);
        bpo::variables_map vm;
        bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        if(vm.count("help")) {
            std::cout << desc << "\n";
            return false;
        }
        bpo::notify(vm); 
        generate_svg = (vm.count("svg") > 0);
        area_provided = (vm.count("area") > 0);
        graphviz = (vm.count("graphviz") > 0);
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::filesystem::path input_file;
    std::filesystem::path output_file;
    std::filesystem::path area_file;
    int hex_resolution;
    bool generate_svg;
    bool area_provided;
    bool graphviz;

    bool valid_command = process_command_line(argc, argv, input_file, output_file, area_file, hex_resolution, generate_svg, area_provided, graphviz);
    if(!valid_command)
        return EXIT_FAILURE;

    Chrono chrono;

    std::vector<Region> raw_regions = IO::parse_geojson(input_file);
    
    std::cout << "parse regions in " << chrono.lapTimeMs() << " ms" << std::endl;
    
    std::vector<std::pair<MultipolygonGeo,RegionInfos>> regions(raw_regions.size());
    try {
        std::transform(raw_regions.cbegin(), raw_regions.cend(), regions.begin(), [](const Region & r) {
            if(! (r.hasProperty("qualityCoef") && r.hasProperty("probConnectionPerMeter")))
                throw std::runtime_error("require \"qualityCoef\" and \"probConnectionPerMeter\" properties for every regions");
            return std::make_pair(r.multipolygon, RegionInfos(
                std::atof(r.getProperty("qualityCoef").c_str()), 
                std::atof(r.getProperty("probConnectionPerMeter").c_str()), 
                r.hasProperty("overrideLevel") ? std::atoi(r.getProperty("overrideLevel").c_str()) : 0));
        });
    } catch(std::invalid_argument & e) {
        throw std::runtime_error("require \"qualityCoef\" and \"probConnectionPerMeter\" properties to be number strings");
    }
    
    std::cout << "raw_regions to regions in " << chrono.lapTimeMs() << " ms" << std::endl;

    using RTree = bgi::rtree<std::pair<BoxGeo,size_t>, bgi::rstar<16,4>>;
    RTree rtree(regions | ba::indexed(0) | ba::transformed([](const auto& e) {
        return std::make_pair(bg::return_envelope<BoxGeo>(e.value().first), e.index());
    }));

    std::cout << "constructed R-tree in " << chrono.lapTimeMs() << " ms" << std::endl;

    
    
    return EXIT_SUCCESS;
}