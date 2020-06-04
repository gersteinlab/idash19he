#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "base64.hpp"
#include "constants.hpp"
#include "genome.hpp"
#include "seal/seal.h"
#include "sealfunctions.hpp"

using namespace genome;
using namespace seal;
using namespace sealfunctions;
using namespace std;

size_t nThreads = 4;

size_t nIts;
size_t nQueries;
Evaluator * pevaluator;
RelinKeys * prelinkeys;
Ciphertext * pzero;
Ciphertext * pone;
vector<vector<Ciphertext>> * pindexes;
vector<Ciphertext> * petargets;
vector<vector<Ciphertext>> * pquery;
vector<Ciphertext> * ptargetSNPs;

unsigned progress=0;
inline void updateProgress()
{
    cout << "\rProgress: " << int(round(100*double(progress++)/nQueries)) << "%" << flush;
}

void oneQuery( Evaluator & evaluator, RelinKeys & relinkeys, const Ciphertext & zero, const Ciphertext & one, const vector<vector<Ciphertext>> & indexes, const vector<Ciphertext> & etargets, const vector<Ciphertext> & query, Ciphertext & targetSNP )
{
    auto nCombinations = indexes.size();
    vector<Ciphertext> partialResults;
    for ( size_t i=0; i<nCombinations; i++ ) partialResults.push_back( add_if_equal( evaluator, relinkeys, query, indexes[i], etargets[i], one ) );
    targetSNP = add_vector( evaluator, partialResults );
}

void oneQueryWrapper( size_t q )
{
    for ( size_t it=0; it<nIts && q<nQueries; it++, q+=nThreads )
    {
        oneQuery( *pevaluator, *prelinkeys, *pzero, *pone, *pindexes, *petargets, (*pquery)[q], (*ptargetSNPs)[q] );
        updateProgress();
    }
}

int main( int argc, char * argv[] )
{
    if ( argc < 6 )
    {
        cout << "Usage: querier reference closestTagSNPs query output nThreads [polyModulusDegree] [plaintextModulus]\n";
        return 1;
    }

    auto reference = readTraining( argv[1], nTypes );
    auto refProbs = reference.first;
    auto refTarget = reference.second;
    auto closestTagSNPs = readPositions( argv[2] );
    auto fileout = string( argv[4] );
    nThreads = stoul( argv[5] );

    size_t offset=6;
    uint64_t polyModulusDegree = argc>offset+1 ? 1<<stoul(argv[offset+1]) : constants::defaultPolyModulusDegree;
    auto plaintextModulus = selectPlaintextModulus( argc, argv, offset, polyModulusDegree );

    EncryptionParameters params(scheme_type::BFV);
    params.set_poly_modulus_degree(polyModulusDegree);
    params.set_coeff_modulus(CoeffModulus::BFVDefault(polyModulusDegree));
    params.set_plain_modulus(plaintextModulus);
    auto context = SEALContext::Create(params);
    auto query = readMatrixCiphertext( context, argv[3] );

    PublicKey publickey;
    ifstream finPK ( constants::publicKeyFile, ios::binary );
    publickey.load(context, finPK);

    RelinKeys relinkeys;
    ifstream finRK ( constants::relinKeysFile, ios::binary );
    relinkeys.load(context, finRK);

    Encryptor encryptor(context, publickey);
    Evaluator evaluator(context);
    BatchEncoder batchEncoder(context);

    size_t size = batchEncoder.slot_count();
    auto nTargetSNPs = refProbs.size();
    size_t individualsPerCiphertext = size / nTargetSNPs;
    auto nCombinations = refProbs[0].size();
    nQueries = query.size();
    nIts = nQueries / nThreads + (nQueries%nThreads>0);
    auto nBits = query[0].size();

    updateProgress();

    Ciphertext zero = vectorIntToCiphertext( batchEncoder, encryptor, vector<uint64_t>(size, 0) );
    Ciphertext one  = vectorIntToCiphertext( batchEncoder, encryptor, vector<uint64_t>(size, 1) );

    // preparing the training data for batching encryption
    vector<vector<uint64_t>> targets(nCombinations, vector<uint64_t>(size, 0) );
    for ( size_t t=0; t<nTargetSNPs; t++ )
    {
        for ( size_t i=0; i<nCombinations; i++ )
        {
            auto target = refTarget[t][i];
            for ( size_t p=0; p<nTypes-1; p++ ) target <<= constants::shifter;
            for ( size_t p=0; p<nTypes-1; p++ ) target |= uint64_t( constants::multiplier * refProbs[t][i][p] ) << (p * constants::shifter);
            for ( size_t j=0; j<individualsPerCiphertext; j++ ) targets[i][nTargetSNPs*j+t] = target;
        }
    }

    // encrypt training data with Alice's public key
    vector<Ciphertext> etargets;
    for ( size_t i=0; i<nCombinations; i++ ) etargets.push_back( vectorIntToCiphertext(batchEncoder, encryptor, targets[i]) );

    // create encrypted indexes that will be compared to the query
    vector<vector<Ciphertext>> indexes( nCombinations, vector<Ciphertext>(nBits) );
    for ( size_t i=0; i<nCombinations; i++ )
        for ( size_t b=0; b<nBits; b++ )
            indexes[i][b] = vectorIntToCiphertext(batchEncoder, encryptor, vector<uint64_t>(size, (i>>b)&1) );

    // get encrypted target SNPs
    vector<Ciphertext> targetSNPs(nQueries);
    pevaluator = &evaluator;
    prelinkeys = & relinkeys;
    pzero = &zero;
    pone = &one;
    pindexes = &indexes;
    petargets = &etargets;
    pquery = &query;
    ptargetSNPs = &targetSNPs;

    // query
    vector<thread> threads(nThreads);
    for ( size_t q=0; q<nThreads; q++ ) threads[q] = thread( oneQueryWrapper, q );
    for ( size_t i=0; i<nThreads; i++ ) threads[i].join();

    // write to file
    ofstream fout( fileout );
    for ( size_t q=0; q<nQueries; q++ ) fout << ciphertextToBase64( targetSNPs[q] ) << "\n";
    cout << "\n";
}
