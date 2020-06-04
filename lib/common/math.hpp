#pragma once

#include <string>
#include <vector>

using namespace std;

vector<int> decimalToBase( size_t n, size_t base, size_t size )
{
    vector<int> vout(size, 0);
    for ( size_t i=size; n && i; )
    {
        vout[--i] = n % base;
        n /= base;
    }
    return vout;
}

template <typename T>
T expt(T p, unsigned q)
{
    T r(1);
    while (q)
    {
        if (q & 1)
        {
            q--;
            r *= p;
        }
        p *= p;
        q >>= 1;
    }
    return r;
}

bool isNumber( string s )
{
    for ( auto c : s ) if ( !isdigit(c) ) return false;
    return true;
}


size_t sizeCeilPowerOfTwo( uint64_t n )
{
    size_t counter=0;
    while (n)
    {
        counter++;
        n>>=1;
    }
    return counter;
}

vector<vector<int>> allCombinations( size_t base, size_t size )
{
    vector<vector<int>> vout;
    auto nCombinations = expt( base, size );
    for ( size_t i=0; i<nCombinations; i++ )
        vout.push_back( decimalToBase( i, base, size ) );
    return vout;
}
