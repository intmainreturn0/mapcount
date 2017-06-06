#ifndef MAPCOUNT_MC_UTILS_HPP
#define MAPCOUNT_MC_UTILS_HPP


int strlen( const char *s ) {
    int len = -1;
    while( s[++len] );
    return len;
}

bool endsWith( const char *s, const char *ending ) {
    for( int e_len = strlen( ending ), s_len = strlen( s ); e_len >= 0 && s_len >= 0; --s_len, --e_len )
        if( s[s_len] != ending[e_len] )
            return false;
    return true;
}


#endif //MAPCOUNT_MC_UTILS_HPP
