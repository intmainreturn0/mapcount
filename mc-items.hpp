#ifndef MAPCOUNT_MC_ITEMS_HPP
#define MAPCOUNT_MC_ITEMS_HPP

#include "timer.h"

const unsigned int MAX_ID = 4200000000u;

struct Item {
    unsigned int id;
    unsigned int sum;        // count = sum (для общности многоколоночности называем поле sum, а не count)
};

struct Item2 {
    unsigned int id;
    unsigned int count1;
    unsigned int sum;        // count2 = sum - count1
};

struct Item3 {
    unsigned int id;
    unsigned int count1;
    unsigned int count2;
    unsigned int sum;        // count3 = sum - count1 - count2
};

struct Item4 {
    unsigned int id;
    unsigned int count1;
    unsigned int count2;
    unsigned int count3;
    unsigned int sum;        // count4 = sum - count1 - count2 - count3
};


template<class T>
bool cmp_Item( const T &a, const T &b ) {
    return a.sum > b.sum;
}


void fillHistogram( const unsigned int *sortedIDs, unsigned int N, Item *hist, unsigned int nMc ) {
    unsigned int count_of_cur = 1, last = *sortedIDs;
    for( const unsigned int *cur = sortedIDs + 1, *end = sortedIDs + N; cur < end; ++cur ) {
        if( *cur != last ) {
            hist->id = last;
            hist->sum = count_of_cur;
            hist++;
            count_of_cur = 1;
            last = *cur;
        }
        else {
            count_of_cur++;
        }
    }
    hist->id = last;
    hist->sum = count_of_cur;
}

unsigned int calcUniqueIDs( const unsigned int *sortedIDs, unsigned int N ) {
    Timer t3( "calc number of unique (nMc)" );

    unsigned int uniq = 0;
#pragma omp parallel for reduction(+:uniq)
    for( int i = 1; i < N; ++i ) {
        if( sortedIDs[i] != sortedIDs[i - 1] )
            ++uniq;
    }

    t3.stop();
    return uniq + (N > 0 ? 1 : 0);
}

Item *makeHistogramFromFile( const char *filename, unsigned int &nMc ) {
    Timer t1( "read array of numbers" );
    std::FILE *f = fopen( filename, "r" );
    fseek( f, 0, SEEK_END );
    unsigned int N = (unsigned int) (ftell( f )) / 4;

#ifndef NO_DEBUG_INFO
    printf( "file size = %d (%d numbers)\n", N * 4, N );
#endif

    unsigned int *arr = new unsigned int[N];        // N=0 нормально отрабатывает
    fseek( f, 0, SEEK_SET );
    fread( arr, 4, N, f );
    fclose( f );
    t1.stop();

    Timer t2( "sort initial array of numbers" );
    SORTER_FUNC( arr, arr + N );
    t2.stop();

    nMc = calcUniqueIDs( arr, N );
#ifndef NO_DEBUG_INFO
    printf( "Unique count = %d\n", nMc );
#endif

    Timer t4( "fill histogram" );
    Item *hist = new Item[nMc + 1];
    fillHistogram( arr, N, hist, nMc );
    hist[nMc].id = MAX_ID;    // stop-символ для merge-алгоритмов
    t4.stop();

    delete[] arr;    // если несколько колонок, эта же память может выделиться под чтение следующих
    return hist;
}

template<class T>
void convertHistogramToMapcount( T *hist, unsigned int nElems ) {
    Timer t7( "sorting histogram by count" );
    SORTER_FUNC( hist, hist + nElems, cmp_Item<T> );
    t7.stop();
}


#endif //MAPCOUNT_MC_ITEMS_HPP
