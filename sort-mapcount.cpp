/*
 * Основной бинарник, который вызывается для анализа одного или нескольких -i bin-файлов:
 * сортировка по sum, слайс по нему же и запись результатов в json/tsv.
 *
 * Отдельный паттерн использования этого бинарника — последовательное слияние нескольких гистограмм в одну большую,
 * чтобы в конце отсортировать её по sum и отслайсить по n_common/sum. См. про расширение ".hist".
 */

#include <cstdio>
#include <algorithm>
#include <fstream>
#include "options.hpp"

#ifdef _OPENMP
#include <parallel/algorithm>
#define SORTER_FUNC __gnu_parallel::sort
#else
#define SORTER_FUNC std::sort
#endif

#include "mc-items.hpp"
#include "mc-merging.hpp"
#include "mc-utils.hpp"
#include "mc-output.hpp"
#include "mc-debug.hpp"


enum optionIndex {
    UNKNOWN, HELP, INFILE, OUTFILE, ID_PREFIX, OUT_LIMIT, MIN_VALUE, MAX_VALUE
};
const option::Descriptor usage[] =
        {
                {UNKNOWN,   0, "",  "",          Arg::Unknown,  "USAGE: ./map-count [options]\n\nOptions:"},
                {HELP,      0, "h", "help",      Arg::None,     "  --help  \tPrint usage and exit."},
                {INFILE,    0, "i", "input",     Arg::Required, "  --input, -i  \tInput binary file (required) or many: -i ... -i ..."},
                {OUTFILE,   0, "o", "output",    Arg::Required, "  --output, -o  \tOutput file (extension .json or .tsv)."},
                {ID_PREFIX, 0, "",  "id-prefix", Arg::Required, "  --id-prefix \tu|g|... prefix for localId."},
                {OUT_LIMIT, 0, "",  "out-limit", Arg::Required, "  --out-limit 10000 \tMax output after sorting."},
                {MIN_VALUE, 0, "",  "min-value", Arg::Required, "  --min-value 1 \tFilter count (or sum if multiple inputs)"},
                {MAX_VALUE, 0, "",  "max-value", Arg::Required, "  --max-value 9999\tFilter count (or sum if multiple inputs)"},
                {0,         0, 0,   0,           0,             0}
        };


template<class T>
void filterMapcountMinMax( const T *mc, unsigned int nMc,
                           unsigned int minValue, unsigned int maxValue, unsigned int outLimit,
                           unsigned int &start, unsigned int &count ) {
    if( minValue <= 1 && maxValue >= mc->sum ) {
        start = 0;
        count = nMc > outLimit ? outLimit : nMc;
        return;
    }
    // считаем start
    for( start = 0, count = 0; start < nMc; ++start ) {
        if( mc[start].sum <= maxValue )
            break;
    }
    // считаем count
    for( int i = start; i < nMc && count < outLimit; ++i, ++count ) {
        if( mc[i].sum < minValue )
            break;
    }
}

template<class T>
unsigned int outputToFile( const char *outFn, const T *mc, unsigned int n, char idPrefix ) {
    std::ofstream out( outFn, std::ios::out | std::ios::trunc );
    std::char_traits<char>::pos_type before = out.tellp();
    Timer t6( "printing output" );

    if( endsWith( outFn, ".lids.json" ))
        outputToJson_lids( out, mc, n, idPrefix );
    else if( endsWith( outFn, ".ids.json" ))
        outputToJson_ids( out, mc, n, idPrefix );
    else if( endsWith( outFn, ".json" ))
        outputToJson( out, mc, n, idPrefix );
    else if( endsWith( outFn, ".lids.tsv" ))
        outputToTsv_lids( out, mc, n, idPrefix );
    else
        outputToTsv( out, mc, n, idPrefix );

    t6.stop();
    return (unsigned int) (out.tellp() - before);
}


int main( int argc, char **argv ) {
    argc -= (argc > 0);
    argv += (argc > 0); // argv[0] — имя программы
    option::Stats stats( usage, argc, argv );
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    option::Parser parse( usage, argc, argv, options, buffer );
    if( parse.error())
        return 1;

#ifndef NO_DEBUG_INFO
    if( options[HELP] ) {
        option::printUsage( std::fwrite, stdout, usage );
        return 0;
    }
#endif

    char idPrefix = options[ID_PREFIX] ? options[ID_PREFIX].arg[0] : 'u';
    unsigned int outLimit = 1000000;
    unsigned int minValue = 1;
    unsigned int maxValue = 1000000000;
    const char *outFn = "out.json";

    if( options[OUT_LIMIT] )
        sscanf( options[OUT_LIMIT].arg, "%u", &outLimit );
    if( options[MIN_VALUE] )
        sscanf( options[MIN_VALUE].arg, "%u", &minValue );
    if( options[MAX_VALUE] )
        sscanf( options[MAX_VALUE].arg, "%u", &maxValue );
    if( options[OUTFILE] )
        outFn = options[OUTFILE].arg;        // *.json / *.tsv
    if( maxValue == 0 )
        maxValue = 1000000000;
    if( minValue > maxValue )
        minValue ^= maxValue ^= minValue ^= maxValue;

    unsigned int start, result_amount = 0, filesize = 0;

    switch( options[INFILE].count()) {
        case 0:
            printf( "-i not passed\n" );
            return 1;

        case 1: {
            unsigned int n;
            Item *hist = makeHistogramFromFile( options[INFILE].arg, n );

            convertHistogramToMapcount( hist, n );
            debugPrint( hist, n );
            filterMapcountMinMax( hist, n, minValue, maxValue, outLimit, start, result_amount );
            filesize = outputToFile( outFn, hist + start, result_amount, idPrefix );
            break;
        }

        case 2: {
            unsigned int n1, n2;
            option::Option *opt = options[INFILE];
            Item *hist1 = makeHistogramFromFile( opt->arg, n1 );
            opt = opt->next();
            Item *hist2 = makeHistogramFromFile( opt->arg, n2 );
            opt = opt->next();

            unsigned int n = calcUniqueIDsOf( hist1, n1, hist2, n2 );
            Item2 *hist = new Item2[n];
            mergeHistograms( hist, hist1, n1, hist2, n2 );
            convertHistogramToMapcount( hist, n );
            debugPrint( hist, n );

            filterMapcountMinMax( hist, n, minValue, maxValue, outLimit, start, result_amount );
            filesize = outputToFile( outFn, hist + start, result_amount, idPrefix );
            break;
        }

        case 3: {
            unsigned int n1, n2, n3;
            option::Option *opt = options[INFILE];
            Item *hist1 = makeHistogramFromFile( opt->arg, n1 );
            opt = opt->next();
            Item *hist2 = makeHistogramFromFile( opt->arg, n2 );
            opt = opt->next();
            Item *hist3 = makeHistogramFromFile( opt->arg, n3 );
            opt = opt->next();

            unsigned int n = calcUniqueIDsOf( hist1, n1, hist2, n2, hist3, n3 );
            Item3 *hist = new Item3[n];
            mergeHistograms( hist, n, hist1, hist2, hist3 );
            convertHistogramToMapcount( hist, n );
            debugPrint( hist, n );

            filterMapcountMinMax( hist, n, minValue, maxValue, outLimit, start, result_amount );
            filesize = outputToFile( outFn, hist + start, result_amount, idPrefix );
            break;
        }

        case 4: {
            unsigned int n1, n2, n3, n4;
            option::Option *opt = options[INFILE];
            Item *hist1 = makeHistogramFromFile( opt->arg, n1 );
            opt = opt->next();
            Item *hist2 = makeHistogramFromFile( opt->arg, n2 );
            opt = opt->next();
            Item *hist3 = makeHistogramFromFile( opt->arg, n3 );
            opt = opt->next();
            Item *hist4 = makeHistogramFromFile( opt->arg, n4 );
            opt = opt->next();

            unsigned int n = calcUniqueIDsOf( hist1, n1, hist2, n2, hist3, n3, hist4, n4 );
            Item4 *hist = new Item4[n];
            mergeHistograms( hist, n, hist1, hist2, hist3, hist4 );
            convertHistogramToMapcount( hist, n );
            debugPrint( hist, n );

            filterMapcountMinMax( hist, n, minValue, maxValue, outLimit, start, result_amount );
            filesize = outputToFile( outFn, hist + start, result_amount, idPrefix );
            break;
        }

        default:
            printf( "Incorrect number of input files: %d\n", options[INFILE].count());
            return 1;
    }

    printf( "{\"length\":%d,\"filename\":\"%s\",\"filesize\":%d}\n", result_amount, outFn, filesize );

    return 0;
}