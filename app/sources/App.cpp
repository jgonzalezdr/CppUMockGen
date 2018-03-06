#include "App.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <cxxopts.hpp>

#include "Parser.hpp"
#include "Config.hpp"
#include "ConsoleColorizer.hpp"
#include "FileHelper.hpp"

const std::set<std::string> cppExtensions = { "hpp", "hxx", "hh" };

App::App( std::ostream &cout, std::ostream &cerr )
: m_cout(cout), m_cerr(cerr)
{
}

void App::PrintError( const char *msg )
{
    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_RED );
    m_cerr << "ERROR: ";
    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
    m_cerr << msg << std::endl;
}

std::string QuotifyOption( const std::string &option )
{
    if( option.find(' ') != std::string::npos )
    {
        return "\"" + option + "\"";
    }
    else
    {
        return option;
    }
}

std::string GetGenerationOptions( cxxopts::Options &options )
{
    std::string ret;

    if( options["cpp"].as<bool>() )
    {
        ret += "-x ";
    }

    if( options["underlying-typedef"].as<bool>() )
    {
        ret += "-u ";
    }

    for( auto paramOverride : options["param-override"].as<std::vector<std::string>>() )
    {
        ret += "-p " + QuotifyOption( paramOverride ) + " ";
    }

    for( auto typeOverride : options["type-override"].as<std::vector<std::string>>() )
    {
        ret += "-t " + QuotifyOption( typeOverride ) + " ";
    }

    return ret;
}

int App::Execute( int argc, const char* argv[] )
{
    int returnCode = 0;

    cxxopts::Options options("CppUMockGen", "Mock generator for CppUTest");

    options.add_options()
        ( "i,input", "Input file", cxxopts::value<std::string>(), "<input>" )
        ( "m,mock-output", "Mock output path", cxxopts::value<std::string>()->implicit_value(""), "<mock-output>" )
        ( "e,expect-output", "Expectation output path", cxxopts::value<std::string>()->implicit_value(""), "<expect-output>" )
        ( "x,cpp", "Force interpretation of the input file as C++", cxxopts::value<bool>(), "<force-cpp>" )
        ( "u,underlying-typedef", "Use underlying typedef type", cxxopts::value<bool>(), "[<underlying-typedef>]" )
        ( "I,include-path", "Include path", cxxopts::value<std::vector<std::string>>(), "<path>" )
        ( "p,param-override", "Override parameter type", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "t,type-override", "Override generic type", cxxopts::value<std::vector<std::string>>(), "<expr>" )
        ( "h,help", "Print help" );

    options.positional_help( "<input>" );
    options.parse_positional( std::vector<std::string> { "input" } );

    try
    {
        options.parse( argc, const_cast<char**&>(argv) );

        if( options.count("help") )
        {
            m_cerr << options.help();
            return 0;
        }

        if( !options.count( "input" ) )
        {
            throw std::runtime_error( "No input file specified." );
        }

        std::string inputFilename = options["input"].as<std::string>();

        if( ( options.count( "mock-output" ) + options.count( "expect-output" ) ) == 0 )
        {
            throw std::runtime_error( "At least the mock generation option (-m) or the expectation generation option (-e) must be specified." );
        }

        std::string mockOutputFilepath;
        std::ofstream mockOutputFile;
        if( options.count( "mock-output" ) )
        {
            mockOutputFilepath = options["mock-output"].as<std::string>();
            if( mockOutputFilepath != "@" )
            {
                if( mockOutputFilepath.empty() || IsDirPath(mockOutputFilepath) )
                {
                    mockOutputFilepath += RemoveFilenameExtension( GetFilenameFromPath(inputFilename) ) + "_mock.cpp";
                }
                mockOutputFile.open( mockOutputFilepath );
                if( !mockOutputFile.is_open() )
                {
                    std::string errorMsg = "Mock output file '" + mockOutputFilepath + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
            }
        }

        std::string expectHeaderOutputFilepath;
        std::string expectImplOutputFilepath;
        std::ofstream expectHeaderOutputFile;
        std::ofstream expectImplOutputFile;
        if( options.count( "expect-output" ) )
        {
            expectHeaderOutputFilepath = options["expect-output"].as<std::string>();
            if( expectHeaderOutputFilepath != "@" )
            {
                if( expectHeaderOutputFilepath.empty() || IsDirPath(expectHeaderOutputFilepath) )
                {
                    std::string baseFilename = RemoveFilenameExtension( GetFilenameFromPath(inputFilename) );
                    expectImplOutputFilepath = expectHeaderOutputFilepath + baseFilename + "_expect.cpp";
                    expectHeaderOutputFilepath += baseFilename + "_expect.hpp";
                }
                else
                {
                    std::string baseFilename = RemoveFilenameExtension( expectHeaderOutputFilepath );
                    expectImplOutputFilepath = baseFilename + ".cpp";
                    expectHeaderOutputFilepath = baseFilename + ".hpp";
                }

                expectHeaderOutputFile.open( expectHeaderOutputFilepath );
                if( !expectHeaderOutputFile.is_open() )
                {
                    std::string errorMsg = "Expectation header output file '" + expectHeaderOutputFilepath + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }

                expectImplOutputFile.open( expectImplOutputFilepath );
                if( !expectImplOutputFile.is_open() )
                {
                    std::string errorMsg = "Expectation implementation output file '" + expectImplOutputFilepath + "' could not be opened.";
                    throw std::runtime_error( errorMsg );
                }
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
                       options["param-override"].as<std::vector<std::string>>(),
                       options["type-override"].as<std::vector<std::string>>() );

        std::string genOpts = GetGenerationOptions( options );

        Parser parser;

        if( parser.Parse( inputFilename, config, interpretAsCpp, options["include-path"].as<std::vector<std::string>>(), m_cerr ) )
        {
            if( !mockOutputFilepath.empty() )
            {
                std::ostringstream output;
                parser.GenerateMock( genOpts, output );

                if( mockOutputFile.is_open() )
                {
                    mockOutputFile << output.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Mock generated into '" << mockOutputFilepath << "'" << std::endl;
                }
                else
                {
                    m_cout << output.str();
                }
            }

            if( !expectHeaderOutputFilepath.empty() )
            {
                std::ostringstream headerOutput;
                std::ostringstream implOutput;
                parser.GenerateExpectationHeader( genOpts, headerOutput );
                parser.GenerateExpectationImpl( genOpts, expectHeaderOutputFilepath, implOutput );

                if( expectHeaderOutputFile.is_open() )
                {
                    expectHeaderOutputFile << headerOutput.str();
                    expectImplOutputFile << implOutput.str();

                    cerrColorizer.SetColor( ConsoleColorizer::Color::LIGHT_GREEN );
                    m_cerr << "SUCCESS: ";
                    cerrColorizer.SetColor( ConsoleColorizer::Color::RESET );
                    m_cerr << "Expectations generated into '" << expectHeaderOutputFilepath << "' and '" << expectImplOutputFilepath << "'" << std::endl;
                }
                else
                {
                    m_cout << headerOutput.str();
                    m_cout << implOutput.str();
                }
            }
        }
        else
        {
            returnCode = 2;
            std::string errorMsg = "Output could not be generated due to errors parsing the input file '" + inputFilename + "'.";
            throw std::runtime_error( errorMsg );
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

