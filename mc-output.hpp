/*
 * Разные форматы вывода (json, tsv), для разных типов Item.
 */

#ifndef MAPCOUNT_MC_OUTPUT_HPP
#define MAPCOUNT_MC_OUTPUT_HPP

#include <fstream>
#include "mc-utils.hpp"

template<class T>
void outputToJson_lids( std::ofstream &out, const T *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << '"' << idPrefix << mc[i].id
        << (i == n - 1 ? "\"]" : "\",");
    }
}

template<class T>
void outputToTsv_lids( std::ofstream &out, const T *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << idPrefix << mc[i].id << '\n';
    }
}

template<class T>
void outputToJson_ids( std::ofstream &out, const T *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << mc[i].id
        << (i == n - 1 ? ']' : ',');
    }
}

template<class T>
void outputToTsv_ids( std::ofstream &out, const T *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << mc[i].id << '\n';
    }
}

void outputToJson( std::ofstream &out, const Item *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << "{\"id\":\"" << idPrefix << mc[i].id
        << "\",\"count\":" << mc[i].sum
        << (i == n - 1 ? "}]\n" : "},\n");
    }
}

void outputToTsv( std::ofstream &out, const Item *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << idPrefix << mc[i].id << '\t'
        << mc[i].sum << '\n';
    }
}

void outputToJson( std::ofstream &out, const Item2 *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << "{\"id\":\"" << idPrefix << mc[i].id
        << "\",\"count1\":" << mc[i].count1
        << ",\"count2\":" << mc[i].sum - mc[i].count1
        << (i == n - 1 ? "}]\n" : "},\n");
    }
}

void outputToTsv( std::ofstream &out, const Item2 *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << idPrefix << mc[i].id << '\t'
        << mc[i].count1 << '\t'
        << mc[i].sum - mc[i].count1 << '\n';
    }
}

void outputToJson( std::ofstream &out, const Item3 *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << "{\"id\":\"" << idPrefix << mc[i].id
        << "\",\"count1\":" << mc[i].count1
        << ",\"count2\":" << mc[i].count2
        << ",\"count3\":" << mc[i].sum - mc[i].count1 - mc[i].count2
        << (i == n - 1 ? "}]\n" : "},\n");
    }
}

void outputToTsv( std::ofstream &out, const Item3 *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << idPrefix << mc[i].id << '\t'
        << mc[i].count1 << '\t'
        << mc[i].count2 << '\t'
        << mc[i].sum - mc[i].count1 - mc[i].count2 << '\n';
    }
}

void outputToJson( std::ofstream &out, const Item4 *mc, unsigned int n, char idPrefix ) {
    out << (n ? "[" : "[]");
    for( int i = 0; i < n; ++i ) {
        out << "{\"id\":\"" << idPrefix << mc[i].id
        << "\",\"count1\":" << mc[i].count1
        << ",\"count2\":" << mc[i].count2
        << ",\"count3\":" << mc[i].count3
        << ",\"count4\":" << mc[i].sum - mc[i].count1 - mc[i].count2 - mc[i].count3
        << (i == n - 1 ? "}]\n" : "},\n");
    }
}

void outputToTsv( std::ofstream &out, const Item4 *mc, unsigned int n, char idPrefix ) {
    for( int i = 0; i < n; ++i ) {
        out << idPrefix << mc[i].id << '\t'
        << mc[i].count1 << '\t'
        << mc[i].count2 << '\t'
        << mc[i].count3 << '\t'
        << mc[i].sum - mc[i].count1 - mc[i].count2 - mc[i].count3 << '\n';
    }
}


#endif //MAPCOUNT_MC_OUTPUT_HPP
