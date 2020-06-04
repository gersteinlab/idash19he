#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include "genome.hpp"

using namespace genome;
using namespace std;

int main(int argc, char * argv[])
{
    if (argc < 6)
    {
        cout << "Usage: closestTagSNPsFinder tagSNPs targetSNPs indexes tagIDs targetIDs\n";
        return 1;
    }

    auto tagSNPs = readSNPs( argv[1] );
    auto targetSNPs = readSNPs( argv[2] );
    auto fileout = string( argv[3] );
    auto fileTagIDs = string( argv[4] );
    auto fileTargetIDs = string( argv[5] );

    ofstream foutTag( fileTagIDs );
    for ( auto tagSNP : tagSNPs ) foutTag << tagSNP.id << "\n";

    ofstream foutTarget( fileTargetIDs );
    vector<vector<int>> positions;
    for ( auto targetSNP : targetSNPs )
    {
        foutTarget << targetSNP.id << "\n";
        vector<pair<int,int>> distances(tagSNPs.size());
        for ( int i=0; i<tagSNPs.size(); i++ )
        {
            distances[i].first = abs(tagSNPs[i].idx - targetSNP.idx);
            distances[i].second = i;
        }
        sort(distances.begin(), distances.end());
        vector<int> aux;
        for ( auto distance : distances ) aux.push_back( distance.second );
        positions.push_back( aux );
    }

    {
        ofstream fout( fileout );
        for ( auto pos : positions )
        {
            for ( auto idx : pos ) fout << idx << " ";
            fout << "\n";
        }
    }
}
