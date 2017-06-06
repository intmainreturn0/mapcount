#ifndef MAPCOUNT_MC_DEBUG_HPP
#define MAPCOUNT_MC_DEBUG_HPP

void debugLine( const Item &mc ) {
    printf( "%d - %d\n", mc.id, mc.sum );
}

void debugLine( const Item2 &mc ) {
    printf( "%d - %d\t(%d,%d)\n", mc.id, mc.sum, mc.count1, mc.sum - mc.count1 );
}

void debugLine( const Item3 &mc ) {
    printf( "%d - %d\t(%d,%d,%d)\n", mc.id, mc.sum, mc.count1, mc.count2, mc.sum - mc.count1 - mc.count2 );
}

void debugLine( const Item4 &mc ) {
    printf( "%d - %d\t(%d,%d,%d,%d)\n", mc.id, mc.sum, mc.count1, mc.count2, mc.count3, mc.sum - mc.count1 - mc.count2 - mc.count3 );
}

#ifdef NO_DEBUG_INFO

template<class T>
void debugPrint( T *mc, int n, int cut = 5 ) {}

#else

template<class T>
void debugPrint( T *mc, int n, int cut = 5 ) {
    printf( "\nMapCount (%d):\n", n );
    if( n < cut * 2 ) {
        for( int i = 0; i < n; ++i )
            debugLine( mc[i] );
    }
    else {
        for( int i = 0; i < cut; ++i )
            debugLine( mc[i] );
        if( n > cut * 2 )
            printf( "...\n" );
        for( int i = n - cut; i < n; ++i )
            debugLine( mc[i] );
    }
}

#endif

#endif //MAPCOUNT_MC_DEBUG_HPP
