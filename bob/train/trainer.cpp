#include <algorithm>
#include <fstream>
#include <iostream>
#include "genome.hpp"
#include "math.hpp"

using namespace genome;
using namespace std;

// const size_t nTypes = 3;
// const size_t nProbs = nTypes * nTypes;

int main(int argc, char * argv[])
{
    if (argc < 7)
    {
        cout << "train referenceTagSNPs referenceTargetSNPs closestSNPs output limitSNPs limitIndividuals\n";
        return 1;
    }

    auto tagSNPs = readSNPs( argv[1] );
    auto targetSNPs = readSNPs( argv[2] );
    auto closestTagSNPs = readPositions( argv[3] );
    auto fileout = string( argv[4] );
    auto limitSNPs = stoul( argv[5] );
    auto limitIndividuals = stoul( argv[6] );

    auto querySNPs = allCombinations( nTypes, limitSNPs );

    auto nIndividuals = tagSNPs[0].data.size();
    auto nQueryIndividuals = querySNPs.size();
    auto nTagSNPs = tagSNPs.size();
    auto nTargetSNPs = targetSNPs.size();

    ofstream fout( fileout );
    for ( size_t t=0; t<nTargetSNPs; t++ )
    {
        auto targetSNP = targetSNPs[t];

        vector<vector<int>> selectedSNPs;
        for ( size_t s=0; s<limitSNPs; s++ )
            selectedSNPs.push_back( tagSNPs[ closestTagSNPs[t][s] ].data );
        selectedSNPs.push_back( targetSNP.data );

        for ( size_t i=0; i<nQueryIndividuals; i++ )
        {
            auto queryIndividual = querySNPs[i];
            vector<pair<int,int>> dist;
            for ( size_t j=0, d=0; j<nIndividuals; j++, d=0 )
            {
                for ( size_t s=0; s<limitSNPs; s++ ) d += abs( selectedSNPs[s][j]-queryIndividual[s] );
                dist.push_back( make_pair(d,j) );
            }
            sort( dist.begin(), dist.end() );

            vector<vector<int>> newmat( selectedSNPs.size() );
            for ( size_t s=0; s<selectedSNPs.size(); s++ )
                for ( size_t i=0; i<limitIndividuals; i++ )
                    newmat[s].push_back( selectedSNPs[s][ dist[i].second ] );
            auto LD=getPairwiseCorr(newmat, nTypes, nProbs);

            vector<double> probs(nProbs, 0);
            for ( size_t s=0; s<newmat.size(); s++ ) // for all selected SNPs
            {
                for ( size_t tt=0; tt<newmat[s].size(); tt++ ) // for all selected individuals
                {
                    auto idx = nTypes * newmat[s][tt]; // 1st index of the pair of genotypes ( 'idx+op' gives the complete index to the pair )
                    for ( size_t op=0; op<nTypes; op++ ) probs[idx+op] += LD[idx+op][s]; // sum the probabilities of the pair genotypes for the pair of SNPs {current selected tag SNP, target SNP}
                }
            }

            double sumProbs = 0;
            for ( auto prob : probs ) sumProbs += prob;
            vector<double> ts(nTypes, 0);
            fout << ", ";
            for ( size_t idx=0; idx<ts.size(); idx++ )
            {
                for ( size_t jdx=idx; jdx<probs.size(); jdx+=nTypes ) ts[idx] += probs[jdx]; // concatenate pairs of genotypes that result in the same target genotype
                ts[idx] /= sumProbs; // normalize probabilities
                fout << ts[idx] << " ";
            }
            fout << distance( ts.begin(), max_element( ts.begin(), ts.end() ) ) << " ";
        }
        fout << "\n";
        cout << "." << flush;
    }
    cout << "\n";
}
