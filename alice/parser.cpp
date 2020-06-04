#include <iostream>
#include <fstream>
#include <vector>
#include "genome.hpp"
#include "math.hpp"

using namespace genome;
using namespace std;

int main( int argc, char* argv[] )
{
    if ( argc < 5 )
    {
        cout << "Usage: parser queryTagSNPs closestTagSNPs output limitSNPs\n";
        return 1;
    }

    auto tagSNPs = readSNPs( argv[1] );
    auto closestTagSNPs = readPositions( argv[2] );
    auto fileout = string( argv[3] );
    auto limitSNPs = stoul( argv[4] );
    auto nIndividuals = tagSNPs[0].data.size();
    auto nTargetSNPs = closestTagSNPs.size();
    auto nCombinations = expt(nTypes, limitSNPs);
    auto nBits = sizeCeilPowerOfTwo( nCombinations-1 );

    vector<vector<int>> indexes(nIndividuals);
    for ( size_t t=0; t<nTargetSNPs; t++ )
    {
        auto selectedSNPs = selectSNPs( tagSNPs, closestTagSNPs[t], limitSNPs );
        for ( size_t i=0; i<nIndividuals; i++ )
            indexes[i].push_back( calcIndex( selectedSNPs, i ) );
    }

    ofstream fout( fileout );
    for ( size_t i=0; i<nIndividuals; i++ )
    {
        for ( size_t t=0; t<nTargetSNPs; t++ )
        {
            fout << ", ";
            for ( size_t j=0; j<nBits; j++) fout << ((indexes[i][t]>>j)&1) << " ";
        }
        fout << "\n";
    }
}
