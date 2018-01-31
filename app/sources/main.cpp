#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <cxxopts.hpp>

#include "Config.hpp"
#include "MockGenerator.hpp"
#include "ConsoleColorizer.hpp"

const std::set<std::string> cppExtensions = { "hpp", "hxx", "hh" };

static void PrintError( const char *msg )
{
    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
    std::cerr << "ERROR: ";
    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
    std::cerr << msg;
}

int main( int argc, char* argv[] )
{
    int returnCode = 0;

    cxxopts::Options options("CppUMockGen", "Mock generator for CppUTest");

    options.add_options()
        ( "i,input", "Input file", cxxopts::value<std::string>(), "<input>" )
        ( "o,output", "Output file", cxxopts::value<std::string>(), "<output>" )
        ( "x,cpp", "Force interpretation of the input file as C++", cxxopts::value<bool>(), "<force-cpp>" )
        ( "u,underlying-typedef", "Use underlying typedef type", cxxopts::value<bool>(), "[<underlying-typedef>]" )
        ( "I,include-path", "Include path", cxxopts::value<std::vector<std::string>>(), "<path>" )
        ( "p,param-override", "Override parameter type", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "h,help", "Print help" );

    options.positional_help( "[<input>] [<output>]" );
    options.parse_positional( std::vector<std::string> { "input", "output" } );

    try
    {
        options.parse( argc, argv );

        if( options.count("help") )
        {
            std::cout << options.help();
            exit(0);
        }

        if( !options.count( "input" ) )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::string inputFilename = options["input"].as<std::string>();

        std::ofstream outputFile;
        if( options.count( "output" ) )
        {
            std::string outputFilename = options["output"].as<std::string>();
            outputFile.open( outputFilename );
            if( !outputFile.is_open() )
            {
                std::string errorMsg = "Output file '" + outputFilename + "' could not be opened.";
                throw std::runtime_error( errorMsg );
            }
        }

        bool interpretAsCpp = false;
        if( options["cpp"].as<bool>() )
        {
            interpretAsCpp = true;
        }
        else
        {
            size_t extensionIndex = inputFilename.find_last_of(".");
            if( extensionIndex != std::string::npos )
            {
                std::string fileExtension = inputFilename.substr( extensionIndex + 1 );
                interpretAsCpp = ( cppExtensions.count( fileExtension ) > 0 );
            }
        }

        Config config( options["underlying-typedef"].as<bool>(),
                       options["param-override"].as<std::vector<std::string>>() );

        std::ostringstream output;

        if( GenerateMock( inputFilename, config, interpretAsCpp, options["include-path"].as<std::vector<std::string>>(), output, std::cerr ) )
        {
            if( options.count( "output" ) )
            {
                outputFile << output.str();
            }
            else
            {
                std::cout << output.str();
            }
        }
        else
        {
            returnCode = 2;
            throw std::runtime_error( "Mock could not be generated due to errors parsing the input file." );
        }
    }
    catch(std::exception &e)
    {
        PrintError( e.what() );
        if( !returnCode )
        {
            returnCode = 1;
        }
    }

    return returnCode;
}
