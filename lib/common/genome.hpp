#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include "math.hpp"

using namespace std;

namespace genome
{

size_t nTypes = 3;
size_t nProbs = nTypes * nTypes;

struct SNP
{
    int idx;
    string id;
    vector<int> data;
};

int calcIndex( const vector<vector<int>> & selectedSNPs, size_t i )
{
    int index=0;
    for ( size_t s=0; s<selectedSNPs.size(); s++ )
        index = index*nTypes + selectedSNPs[s][i];
    return index;
}

vector<vector<double>> getPairwiseCorr(vector<vector<int>> data, size_t nTypes, size_t size)
{
	size_t rows=data.size(); // # of selected SNPs
	size_t cols=data[0].size(); // # of selected individuals (i.e. 'limit')
    vector<vector<double>> probs(size);
    for ( size_t i=0; i<rows; i++ )
    {
        vector<unsigned> sum(size, 0);
        for ( size_t k=0; k<cols; k++ ) sum[ nTypes * data[i][k] + data[rows-1][k] ]++;
        for ( size_t j=0; j<probs.size(); j++ ) probs[j].push_back( double(sum[j]) / cols );
    }
    return probs; // return the propability of size P x R
}

vector<vector<vector<int>>> readMatrix3D( string filename )
{
    vector<vector<vector<int>>> m3;
    ifstream fin ( filename );
    string line;
    while ( getline(fin, line) )
    {
        vector<vector<int>> m2;
        stringstream ss(line);
        string buf;
        while ( ss >> buf )
        {
            if ( buf == "," ) m2.push_back( vector<int>() );
            else m2.back().push_back( stoul(buf) );
        }
        m3.push_back( m2 );
    }
    return m3;
}

pair<vector<vector<vector<double>>>,vector<vector<int>>> readTraining( string filename, size_t nTypes )
{
    vector<vector<vector<double>>> probs; // compressed groups
    vector<vector<int>> ans;
    ifstream fin ( filename );
    string line;
    while ( getline(fin, line) )
    {
        vector<vector<double>> target;
        vector<int> genotypes;
        stringstream ss(line);
        string buf;
        size_t count = 0;
        while ( ss >> buf )
        {
            if ( buf == "," )
            {
                target.push_back( vector<double>() );
                count = 0;
            }
            else
            {
                if ( count++ < nTypes ) target.back().push_back( stod(buf) );
                else genotypes.push_back( stoi(buf) );
            }
        }
        probs.push_back( target );
        ans.push_back( genotypes );
    }
    return make_pair(probs,ans);
}

vector<vector<int>> readPositions( string filename )
{
    vector<vector<int>> positions;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) )
    {
        vector<int> selectedSNPs;
        stringstream ss(line);
        string buf;
        while ( ss >> buf ) selectedSNPs.push_back( stoul(buf) );
        positions.push_back( selectedSNPs );
    }
    return positions;
}

vector<SNP> readSNPs( string filename, size_t offset=4 )
{
    vector<SNP> snps;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) )
    {
        SNP snp;
        // vector<string> tokens;
        stringstream ss(line);
        string buf;
        for ( size_t i=0; ss >> buf; i++ )
        {
            if ( i>=offset )
            {
                auto n = isNumber(buf) ? stoul(buf) : 0;
                n = n<nTypes ? n : 0;
                // n = n==1 ? 0 : ( n==0 ? 1 : n ); // swap 0 and 1 representations
                snp.data.push_back( n );
            }
            else if ( i==1 ) snp.idx = stoul(buf);
            else if ( i==3 ) snp.id = string(buf);
        }
        snps.push_back(snp);
    }
    return snps;
}

vector<string> readVectorString( string filename )
{
    vector<string> v;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) ) v.push_back(line);
    return v;
}

vector<vector<string>> readMatrixString( string filename )
{
    vector<vector<string>> m;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) )
    {
        vector<string> v;
        stringstream ss(line);
        string buff;
        while ( ss >> buff ) v.push_back(buff);
        m.push_back(v);
    }
    return m;
}

vector<vector<int>> selectSNPs( const vector<SNP> & tagSNPs, const vector<int> & indexes, size_t nSNPs )
{
    vector<vector<int>> selectedSNPs;
    for ( size_t s=0; s<nSNPs; s++ ) selectedSNPs.push_back( tagSNPs[ indexes[s] ].data );
    return selectedSNPs;
}

}
