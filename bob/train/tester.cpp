#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "genome.hpp"

using namespace genome;
using namespace std;

const bool debug = false;

int main(int argc, char * argv[])
{
    if (argc < 6)
    {
        cout << "test reference closestSNPs queryTagSNPs queryTargetSNPs limitSNPs\n";
        return 1;
    }

    bool testingMode = argc > 5;
    // load reference tag and target SNPs
    auto refSNPs = readTraining( argv[1], nTypes );
    auto nTargetSNPs = refSNPs.first.size();
    auto closestTagSNPs = readPositions( argv[2] );
    // load query tag SNPs
    auto querySNPs = readSNPs( argv[3] );
    auto queryTargetSNPs = testingMode ? readSNPs( argv[4] ) : vector<SNP>(); // for testing accuracy
    auto nQueryIndividuals = querySNPs[0].data.size(); // # of query individuals
    auto limitSNPs = atoi( argv[5] );

    cout << refSNPs.first.size() << " x " << refSNPs.first[0].size() << " x " << refSNPs.first[0][0].size() << "\n";

    double TP=0;
    size_t i=0;
    for ( size_t i=0; i<refSNPs.first.size(); i++ )
    {
        auto refSNP = refSNPs.first[i];
        auto answers = refSNPs.second[i];
        auto qtargetSNP = testingMode ? queryTargetSNPs[i] : SNP();
        if (debug) cout << "snp processing: " << i << endl;
    	vector<vector<int>> neig, qneig;
        for ( size_t s=0; s < limitSNPs; s++)
        {
            qneig.push_back( querySNPs[ closestTagSNPs[i][s] ].data );
        }

        // for a given individual find the distance between individuals
        for ( size_t k=0; k<nQueryIndividuals; k++ )
        {
            if (debug) cout<<"individual processing: "<<k<<endl;

            int index = 0;
            for ( size_t s=0; s<limitSNPs; s++ )
                index = index*nTypes + qneig[s][k];

            if ( debug )
                for ( size_t idx=0; idx < nTypes; idx++ ) cout << "chance of " << idx << " is " << refSNP[index][idx] << endl;
            if ( testingMode && answers[index] == qtargetSNP.data[k] ) TP++; // if the highest probability matches the target SNP, increment accuracy counter
        }
    }
    if ( testingMode )
    {
        cout<<"TP is "<<TP<<endl;
        cout<<"accuracy is "<<TP/(nQueryIndividuals*double(nTargetSNPs))<<endl; // normalized accuracy
    }
}
