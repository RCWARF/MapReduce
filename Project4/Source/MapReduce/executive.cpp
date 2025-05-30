#include <string>
#include <iostream>
#include <string>
#include <boost\log\core.hpp>
#include <boost\log\expressions.hpp>
#include <boost\filesystem.hpp>
#include <boost\log\trivial.hpp>
#include <boost\program_options.hpp>

#include "executive.h"
#include "workflow.h"


/*	Constructor takes input directory, dir for intermediate files, output directory,
	path to Map DLL and path to Reduce DLL, and number of mappers and reducers as args
*/
Executive::Executive(std::string input_dir_arg, 
	std::string inter_dir_arg, 
	std::string output_dir_arg, 
	int num_mappers,
	int num_reducers,
	int controller_port,
	std::vector<std::string> stub_endpoints)
{
	workflow_ = new Workflow(input_dir_arg, 
		inter_dir_arg, 
		output_dir_arg, 
		num_mappers,
		num_reducers,
		controller_port,
		stub_endpoints);
}

// Destructor
Executive::~Executive()
{
	delete workflow_;
}

// Calls on workflow object to run its map/sort/reduce application
void Executive::startWorkflow()
{
	workflow_->runMapReduceWf();
}

// Set the boost logging threshold to >= info
void init()
{
    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= boost::log::trivial::info
    );
}

int main(int argc, char* argv[])
{
    init();

    std::string input_dir;
    std::string inter_dir;
    std::string output_dir;
    //std::string map_dll;
    //std::string reduce_dll;
    int map_count;
    int reduce_count;
    int controller_port;
    std::vector<std::string> stub_endpoints;

    // Build argument parser
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Show options")
        ("version", "Verison number")
        ("input", boost::program_options::value<std::string>(&input_dir), "Input directory path. Required.")
        ("temp", boost::program_options::value<std::string>(&inter_dir), "Intermediate directory path. Required.")
        ("output", boost::program_options::value<std::string>(&output_dir), "Output directory path. Required.")
        ("stubs", boost::program_options::value<std::vector<std::string>>(&stub_endpoints)->multitoken(), "Endpoint(s) for map/reduce stub(s). Required.")
        ("port", boost::program_options::value<int>(&controller_port)->default_value(8080), "Port for workflow/controller process. Optional. Defaults to 8080.")
        ("mappers", boost::program_options::value<int>(&map_count)->default_value(2), "Number of map threads. Optional. Defaults to 2.")
        ("reducers", boost::program_options::value<int>(&reduce_count)->default_value(3), "Number of reduce threads. Optional. Defaults to 3.")
        ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).style(
        boost::program_options::command_line_style::unix_style ^ boost::program_options::command_line_style::allow_short
    ).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }


    if (vm.count("version")) {
        std::cout << 3 << "\n";
        return 1;
    }

    // Required arguments
    if (!vm.count("input")) {
        BOOST_LOG_TRIVIAL(fatal) << "Missing input directory. An input directory must be provided.";
        return 1;
    }

    if (!vm.count("temp")) {
        BOOST_LOG_TRIVIAL(fatal) << "Missing temporary file directory. An temporary directory must be provided.";
        return 1;
    }

    if (!vm.count("output")) {
        BOOST_LOG_TRIVIAL(fatal) << "Missing output directory. An output directory must be provided.";
        return 1;
    }

    if (!vm.count("stubs")) {
        BOOST_LOG_TRIVIAL(fatal) << "Missing endpoint(s) for map/reduce stub(s). Endpoint(s) must be provided";
        return 1;
    }



    // Instantiate an executor
    Executive* executive = new Executive(input_dir,
        inter_dir,
        output_dir,
        map_count,
        reduce_count,
        controller_port,
        stub_endpoints);

    // Use the executor to run the workflow
    executive->startWorkflow();

    delete executive;
}