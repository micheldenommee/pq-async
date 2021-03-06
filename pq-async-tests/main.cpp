/*
MIT License

Copyright (c) 2011-2019 Michel Dénommée

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//#include "args.h"

#include <gmock/gmock.h>
#include "pq_async_test_env.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int pq_async_log_level;
std::string pq_async_connection_string;
int pq_async_max_pool_size;

int main(int argc, char** argv)
{
    po::options_description desc("pq-async_tests");
    
	std::string verbosity_values;
    desc.add_options()
        ("help,h", "produce help message")
        ("db",
            po::value<std::string>()->default_value(
                "dbname=libpq_async_tests"
            ),
            "Tests database_t connection string"
        )
        ("pool-size",
            po::value<int>()->default_value(5),
            "Max pool size"
        )
        ("gtest_list_tests", "List avaiable tests")
        ("verbosity,v",
            po::value(&verbosity_values)->implicit_value(""),
            "defined verbosity level"
        )
        ;
    
    po::variables_map vm;
    po::store(
        po::command_line_parser(argc, argv)
        .options(desc)
        .allow_unregistered()
        .run(),
        vm
    );
    
    po::notify(vm);
    
    if (vm.count("help")) {
        std::cout << "Usage: pq-async_tests [options]\n"
            << desc << std::endl;
        return 0;
    }
    
    if(vm.count("db"))
        pq_async_connection_string = vm["db"].as<std::string>();
    
    if(vm.count("pool-size"))
        pq_async_max_pool_size = vm["pool-size"].as<int>();
    
    if(vm.count("verbosity"))
        verbosity_values += "v";
    if(
        std::any_of(
            begin(verbosity_values), end(verbosity_values), [](auto& c){
                return c != 'v';
            }
        )
    ){
        throw std::runtime_error("Invalid verbosity level");
    }
    
    pq_async_log_level = verbosity_values.size() +4;
    if((md::log::log_level)pq_async_log_level > md::log::log_level::trace)
        pq_async_log_level = (int)md::log::log_level::trace;
    
    
    testing::InitGoogleMock(&argc, argv);
    testing::AddGlobalTestEnvironment(new pq_async::tests::pq_async_test_env());
    
    int r = RUN_ALL_TESTS();
        
    return r;
}