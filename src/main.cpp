/*
 * main.cpp - program's main line.
 *
 * This file is part of WordIndex.
 *
 * Copyright (C) 2007-2022, Martin J. Moene.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mngdriver; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * compile: cl -std:c++20 -EHsc -Fewordindex.exe main.cpp
 * compile: g++ -std=c++20 -o wordindex.exe main.cpp
 */

/*
 *   keywords: 33
 *      words: 1
 * references: 2
 *
 *      hello: 100% (2)  5 33
 */

/*
 * handle options:
 * ideas:
 * - case sensitivity +case=(preserve,ignore) [preserve]
 * - number of lines per page +pagelength=n [1]
 * - tokens to accept
 * - keyword tokens (stop-words) +stopwords=filename [none]
 *
 * report:
 * - report-summary: keywords, words, references
 * - report-xxx:
 * - number of distinct keywords (stopwords): +report=count_keywords
 * - number of distinct words: +report=count_words
 * - number of word occurrences: +report=count_keywords
 * - percentage of word occurrences
 * - lines with word occurrences: +report=
 * - shorthand: +report=count_words,...
 */

#include <iostream>
#include <ostream>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#ifndef  prg_VERSION
# define prg_VERSION "1.0.0"
#endif

namespace prg {

using text  = std::string;
using texts = std::vector<text>;
using maybe_text = std::optional<text>;

struct options
{
    bool help       = false;
    bool verbose    = false;
    bool version    = false;
    bool author     = false;
    bool frequency  = false;
    bool ignorecase = false;
    bool lowercase  = false;
    bool reverse    = false;
    bool summary    = false;
    text input;
    text output;
    text keywords;
};

// ToDo: std::filesystem?

auto filename( text path )
{
    return path;
}

text program_name = "[program-name]";

auto process_options( texts in, options & opt ) -> maybe_text
{
    if ( opt.ignorecase )
        return {"option --ignorecase is not yet supported"};

    if ( in.size() > 1 && !opt.output.empty() )
        return {"only specify option '--output=filename' with a single input file"};

    // at default, apply ???:

    return std::nullopt;
}

// allow for option formats '-h', '--help', '+help':

auto to_option( text arg )
{
    return arg.substr(0,2) == "--"
        ? maybe_text( arg.substr(2) )
        : ( arg[0] == '-' || arg[0] == '+' )
            ? maybe_text( arg.substr(1) )
            : std::nullopt;
} 

auto to_value( text arg )
{
    return maybe_text( arg );
}

auto split_option( text arg ) -> std::tuple<maybe_text, maybe_text>
{
    auto pos = arg.rfind( '=' );

    return pos == text::npos
                ? std::make_tuple( to_option(arg), std::nullopt )
                : std::make_tuple( to_option(arg.substr( 0, pos )), to_value(arg.substr( pos + 1 )) );
}

auto split_arguments( texts args ) -> std::tuple<options, texts>
{
    options option; texts in;

    bool in_options = true;

    for ( auto & arg : args )
    {
        if ( in_options )
        {
            maybe_text opt, val;
            std::tie( opt, val ) = split_option( arg );

            if      ( !opt                                ) { in_options        = false;           }
            else if ( *opt == ""                          ) { in_options        = false; continue; }
            else if ( *opt == "h" || "help"       == *opt ) { option.help       =  true; continue; }
            else if ( *opt == "v" || "verbose"    == *opt ) { option.verbose    =  true; continue; }
            else if (                "version"    == *opt ) { option.version    =  true; continue; }
            else if ( *opt == "a" || "author"     == *opt ) { option.author     =  true; continue; }
            else if ( *opt == "f" || "frequency"  == *opt ) { option.frequency  =  true; continue; }
            else if ( *opt == "g" || "ignorecase" == *opt ) { option.ignorecase =  true; continue; }
            else if ( *opt == "l" || "lowercase"  == *opt ) { option.lowercase  =  true; continue; }
            else if ( *opt == "r" || "reverse"    == *opt ) { option.reverse    =  true; continue; }
            else if ( *opt == "s" || "summary"    == *opt ) { option.summary    =  true; continue; }
            else if ( *opt == "i" || "input"      == *opt ) { option.input      =  *val; continue; }
            else if ( *opt == "o" || "output"     == *opt ) { option.output     =  *val; continue; }
            else if ( *opt == "k" || "keywords"   == *opt ) { option.keywords   =  *val; continue; }
            else throw std::runtime_error( "unrecognised option '" + text(arg) + "'" );
        }
        in.push_back( arg );
    }

    if ( auto result = process_options( in, option ) )
    {
        throw std::logic_error("Error: " + *result );
    }

    return std::make_tuple( option, in );
}

int usage( std::ostream & os )
{
   os <<
      "Usage: " << program_name << " [option...] [file...]\n"
      "\n"
      "  -h, --help          display this help and exit\n"
      "  -a, --author        report author's name and e-mail [no]\n"
      "      --version       report program and compiler versions [no]\n"
      "  -v, --verbose       report on processing steps [none]\n"
      "\n"
      "  -f, --frequency     also report word frequency as d.dd% (n) [no]\n"
      "  -g, --ignorecase    handle upper and lowercase as being equivalent [not implemented][no]\n"
      "  -l, --lowercase     transform words to lowercase [no]\n"
      "  -r, --reverse       only collect keyword occurrences, see --keywords [no]\n"
      "  -s, --summary       also report number of (key)words and references [no]\n"
      "\n"
      "  -i, --input=file    read filenames from given file [standard input or given filenames]\n"
      "  -o, --output=file   write output to given file [standard output]\n"
      "  -k, --keywords=file read keywords to skip (stopwords) from given file [none]\n"
      "\n"
      "Long options also may start with a plus, like: +help.\n"
      "\n" <<
      filename( program_name )  << " creates an alphabetically sorted index of words present in the\n"
      "input files and it reports the lines where those words occur.\n"
      "Words that are marked as keywords are excluded (see option --keywords).\n"
      "Use option --reverse to only show the occurrences of keywords.\n"
      "\n"
      "Words can be read from standard input, or from files specified on the command\n"
      "line and from files that are specified in another file (see option --input).\n"
      "\n"
      "A file that specifies input filenames may look as follows:\n"
      "   # comment that extends to the end of the line ( ; also starts comment line)\n"
      "   file1.txt file2.txt\n"
      "   file3.txt\n"
      "\n"
      "Example:\n"
      "   echo hello world | " << filename( program_name ) << " --summary --frequency\n"
      "       keywords  0\n"
      "          words  2\n"
      "     references  2\n"
      "\n"
      "          hello  50% (1)  1\n"
      "          world  50% (1)  1\n"
      "\n"
      "Example:\n"
      "   " << filename( program_name )  << " --lowercase file.txt | sort -n -k2 -r \n"
      "This creates a list of lowercase words, sorted on frequency of occurrence.\n"
      "\n"
//      << copyright
      ;

   return EXIT_SUCCESS;
}

int author( std::ostream & os )
{
    os << "\nAuthor: martin.moene at gmail.com";
    return EXIT_SUCCESS;
}

int nofile( std::ostream & os )
{
    os << "\nUsage: " << program_name << " [option...] [file...]\n";
    return EXIT_SUCCESS;
}

text compiler()
{
    std::ostringstream os;
#if   defined (__clang__ )
    os << "clang " << __clang_version__;
#elif defined (__GNUC__  )
    os << "gcc " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__;
#elif defined ( _MSC_VER )
    os << "MSVC " << (_MSC_VER / 100 - 5 - (_MSC_VER < 1900)) << " (" << _MSC_VER << ")";
#else
    os << "[compiler]";
#endif
    return os.str();
}

int version( std::ostream & os )
{
    os << program_name << " v" << prg_VERSION << "\n"
       << "Compiled with " << compiler()  << " on " << __DATE__ << " at " << __TIME__ << ".\n"
       << "For more information, see https://github.com/martinmoene/wordindex-ranged.\n";
   return EXIT_SUCCESS;
}

int apply( text in, options opt, std::ostream & os )
{
    os << "apply with in:'" << in << "' (options)\n"; 

    // ToDo: program functionality goes here:
    // return app.Apply( in, opt, os );

    return 0;
}

text operation_text( options opt )
{
    if      ( opt.lowercase  ) return "lowercase";
    else if ( opt.reverse    ) return "reverse";
    else if ( opt.frequency  ) return "frequency";
    else if ( opt.ignorecase ) return "ignorecase";
    else                       return "unrecognized";
}

// text make_output_name( text path, options opt )
// {
//     return !opt.output.empty()
//         ? opt.output
//         : path + "-" + operation_text( opt ) + ".txt";
// }

int apply( texts in, options opt, std::ostream & os )
{
    auto count = 0;

    for ( auto & input : in )
    {
        // opt.output = make_output_name( input, opt );

        // log(level) << ...
        os << input << " => " << opt.output << ":\n";

        count += apply( input, opt, os );
    }

    return count != in.size();
}

int run( texts arguments, std::ostream & os )
{
    try
    {
        options option; texts in;
        std::tie( option, in ) = split_arguments( arguments );

        if ( option.help    ) { return usage  ( os ); }
        if ( option.author  ) { return author ( os ); }
        if ( option.version ) { return version( os ); }
		if ( in.size() < 1  ) { return nofile ( os ); }

        return apply( in, option, os );
    }
    catch ( std::exception const & e )
    {
        os << program_name << ": " << e.what() << " (try option --help).\n";
        return EXIT_FAILURE;
    }
}

texts to_texts( std::span<char const * const> args )
{
    return texts( args.begin(), args.end() );
}

int main( std::span<char const * const> args, std::ostream & os )
{
    program_name = args[0];

    return run( to_texts( args.subspan(1) ), os  );
}

} // namespace prg

int main(int argc, char * argv[])
{
    return prg::main( { argv, argv + argc }, std::cout );
}
