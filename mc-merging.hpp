/*
	Терминология:
	* гистограмма (histogram, hist) - набор (id,sum) или много колонок (id,count...,sum) ОТСОРТИРОВАННЫЙ ПО ID
		из-за подобной сортировки несколько гистограмм легко сливать между собой
	* MapCount (map-count, mc) – та же структура в памяти, но ОТСОРТИРОВАННАЯ ПО SUM
		обычно это тот результат, что и нужен – он (или первые M элементов) идут в выходной файл (json/tsv)
*/

#ifndef MAPCOUNT_MC_MERGING_HPP
#define MAPCOUNT_MC_MERGING_HPP


unsigned int calcUniqueIDsOf( const Item *hist1, unsigned int n1, const Item *hist2, unsigned int n2 ) {
    Timer t3( "calc unique in 2 hists" );
    const Item *end1 = hist1 + n1, *end2 = hist2 + n2;
    unsigned int uniq = 0;

    while( hist1 < end1 && hist2 < end2 ) {
        ++uniq;
        if( hist1->id == hist2->id )
            ++hist1, ++hist2;
        else if( hist1->id < hist2->id )
            ++hist1;
        else
            ++hist2;
    }

    uniq += end1 - hist1;
    uniq += end2 - hist2;
    t3.stop();
    return uniq;
}

unsigned int calcUniqueIDsOf( const Item *hist1, unsigned int n1, const Item *hist2, unsigned int n2, const Item *hist3, unsigned int n3 ) {
    Timer t3( "calc unique in 3 hists" );
    unsigned int uniq = 0, id;

    while( 1 ) {
        id = MAX_ID;
        if( hist1->id < id )
            id = hist1->id;
        if( hist2->id < id )
            id = hist2->id;
        if( hist3->id < id )
            id = hist3->id;

        if( id == MAX_ID )
            break;
        ++uniq;

        if( id == hist1->id )
            ++hist1;
        if( id == hist2->id )
            ++hist2;
        if( id == hist3->id )
            ++hist3;
    }

    t3.stop();
    return uniq;
}

unsigned int calcUniqueIDsOf( const Item *hist1, unsigned int n1, const Item *hist2, unsigned int n2, const Item *hist3, unsigned int n3,
                              const Item *hist4, unsigned int n4 ) {
    Timer t3( "calc unique in 4 hists" );
    unsigned int uniq = 0, id;

    while( 1 ) {
        id = MAX_ID;
        if( hist1->id < id )
            id = hist1->id;
        if( hist2->id < id )
            id = hist2->id;
        if( hist3->id < id )
            id = hist3->id;
        if( hist4->id < id )
            id = hist4->id;

        if( id == MAX_ID )
            break;
        ++uniq;

        if( id == hist1->id )
            ++hist1;
        if( id == hist2->id )
            ++hist2;
        if( id == hist3->id )
            ++hist3;
        if( id == hist4->id )
            ++hist4;
    }

    t3.stop();
    return uniq;
}

void mergeHistograms( Item *out, Item *hist1, int n1, Item *hist2, int n2 ) {
    Timer t5( "Merge 2 hists in 1-col" );
    Item *end1 = hist1 + n1, *end2 = hist2 + n2;

    while( hist1 < end1 && hist2 < end2 ) {
        if( hist1->id == hist2->id ) {
            out->id = hist1->id;
            out->sum = hist1->sum + hist2->sum;
            ++out, ++hist1, ++hist2;
        }
        else if( hist1->id < hist2->id ) {
            out->id = hist1->id;
            out->sum = hist1->sum;
            ++out, ++hist1;
        }
        else {
            out->id = hist2->id;
            out->sum = hist2->sum;
            ++out, ++hist2;
        }
    }

    while( hist1 < end1 ) {
        out->id = hist1->id;
        out->sum = hist1->sum;
        ++out, ++hist1;
    }
    while( hist2 < end2 ) {
        out->id = hist2->id;
        out->sum = hist2->sum;
        ++out, ++hist2;
    }

    t5.stop();
}

void mergeHistograms( Item2 *out, Item *hist1, unsigned int n1, Item *hist2, unsigned int n2 ) {
    Timer t5( "Merge 2 hists in 2-col" );
    Item *end1 = hist1 + n1, *end2 = hist2 + n2;

    while( hist1 < end1 && hist2 < end2 ) {
        if( hist1->id == hist2->id ) {
            out->id = hist1->id;
            out->count1 = hist1->sum;
            out->sum = hist1->sum + hist2->sum;
            ++out, ++hist1, ++hist2;
        }
        else if( hist1->id < hist2->id ) {
            out->id = hist1->id;
            out->count1 = hist1->sum;
            out->sum = hist1->sum;
            ++out, ++hist1;
        }
        else {
            out->id = hist2->id;
            out->count1 = 0;
            out->sum = hist2->sum;
            ++out, ++hist2;
        }
    }

    while( hist1 < end1 ) {
        out->id = hist1->id;
        out->count1 = hist1->sum;
        out->sum = hist1->sum;
        ++out, ++hist1;
    }
    while( hist2 < end2 ) {
        out->id = hist2->id;
        out->count1 = 0;
        out->sum = hist2->sum;
        ++out, ++hist2;
    }

    t5.stop();
}

void mergeHistograms( Item3 *out, unsigned int n, Item *hist1, Item *hist2, Item *hist3 ) {
    Timer t5( "Merge 3 hists in 3-col" );
    const Item3 *end = out + n;
    unsigned int id;

    while( out < end ) {
        id = MAX_ID;
        if( hist1->id < id )
            id = hist1->id;
        if( hist2->id < id )
            id = hist2->id;
        if( hist3->id < id )
            id = hist3->id;
        out->id = id;
        out->count1 = out->count2 = out->sum = 0;

        if( hist1->id == id ) {
            out->count1 = hist1->sum;
            out->sum += hist1->sum;
            ++hist1;
        }
        if( hist2->id == id ) {
            out->count2 = hist2->sum;
            out->sum += hist2->sum;
            ++hist2;
        }
        if( hist3->id == id ) {
            out->sum += hist3->sum;
            ++hist3;
        }

        ++out;
    }

    t5.stop();
}

void mergeHistograms( Item4 *out, unsigned int n, Item *hist1, Item *hist2, Item *hist3, Item *hist4 ) {
    Timer t5( "Merge 4 hists in 4-col" );
    const Item4 *end = out + n;
    unsigned int id;

    while( out < end ) {
        id = MAX_ID;
        if( hist1->id < id )
            id = hist1->id;
        if( hist2->id < id )
            id = hist2->id;
        if( hist3->id < id )
            id = hist3->id;
        if( hist4->id < id )
            id = hist4->id;
        out->id = id;
        out->count1 = out->count2 = out->count3 = out->sum = 0;

        if( hist1->id == id ) {
            out->count1 = hist1->sum;
            out->sum += hist1->sum;
            ++hist1;
        }
        if( hist2->id == id ) {
            out->count2 = hist2->sum;
            out->sum += hist2->sum;
            ++hist2;
        }
        if( hist3->id == id ) {
            out->count3 = hist3->sum;
            out->sum += hist3->sum;
            ++hist3;
        }
        if( hist4->id == id ) {
            out->sum += hist4->sum;
            ++hist4;
        }

        ++out;
    }

    t5.stop();
}


#endif //MAPCOUNT_MC_MERGING_HPP
