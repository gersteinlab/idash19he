#include <fstream>
#include <sstream>
#include <string>
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

int main( int argc, char * argv[] )
{
    if ( argc<5 )
    {
        cout << "Usage decrypt encResult targetIDs outProbs outTargets [polyModulusDegree] [plaintextModulus]\n";
        return 1;
    }

    auto targetIDs = readVectorString( argv[2] );
    auto fileoutProbs = string( argv[3] );
    auto fileoutTargets = string( argv[4] );

    size_t offset=4;
    uint64_t polyModulusDegree = argc>offset+1 ? 1<<stoul(argv[offset+1]) : constants::defaultPolyModulusDegree;
    auto plaintextModulus = selectPlaintextModulus( argc, argv, offset, polyModulusDegree );

    EncryptionParameters params(scheme_type::BFV);
    params.set_poly_modulus_degree(polyModulusDegree);
    params.set_coeff_modulus(CoeffModulus::BFVDefault(polyModulusDegree));
    params.set_plain_modulus(plaintextModulus);
    auto context = SEALContext::Create(params);
    auto etargets = readVectorCiphertext( context, argv[1] );

    SecretKey secretkey;
    ifstream finSK ( constants::secretKeyFile, ios::binary );
    secretkey.load(context, finSK);

    Decryptor decryptor(context, secretkey);
    BatchEncoder batchEncoder(context);

    size_t nTargets;
    size_t nIndividuals;
    {
        ifstream fin( constants::configFile );
        string line;
        getline(fin, line);
        nTargets = stoul(line);
        getline(fin, line);
        nIndividuals = stoul(line);
    }

    auto nQueries = etargets.size();
    size_t size = batchEncoder.slot_count();
    size_t individualsPerCiphertext = size / nTargets;
    auto maxSize = individualsPerCiphertext * nTargets;

    // Decrypt
    vector<vector<uint64_t>> targets( nTargets, vector<uint64_t>(nIndividuals) );
    for ( size_t q=0; q<nQueries; q++ )
    {
        auto target = ciphertextToVectorInt(batchEncoder, decryptor, etargets[q]);
        for ( size_t i=0; i<individualsPerCiphertext; i++ )
        {
            auto ind = q*individualsPerCiphertext+i;
            if ( ind >= nIndividuals ) break;
            for ( size_t t=0; t<nTargets; t++ ) targets[t][ind] = target[i*nTargets+t];
        }
    }

    // Write
    ofstream foutp ( fileoutProbs );
    ofstream foutt ( fileoutTargets );
    foutp << "Subject ID,target SNP,0,1,2\n";
    foutt << "class\n";
    for ( size_t i=0; i<targets[0].size(); i++ )
    {
        for ( size_t t=0; t<targets.size(); t++ )
        {
            foutp << (i+1) << "," << targetIDs[t] << ",";
            double lastProb = 1;
            for ( size_t p=0; p<nTypes-1; p++ )
            {
                auto prob = targets[t][i] & constants::multiplier;
                auto dprob = double(prob) / constants::multiplier;
                foutp << fixed << setprecision(3) << dprob << ",";
                lastProb -= dprob;
                targets[t][i] >>= constants::shifter;
            }
            foutp << fixed << setprecision(3) << lastProb << "\n"; // ( i+1==nIndividuals ? "\n" : "," );
            // foutt << (i+1) << "," << targetIDs[t] << "," << targets[t][i] << "\n"; // ( i+1==nIndividuals ? "\n" : "," );
            foutt << targets[t][i] << "\n"; // ( i+1==nIndividuals ? "\n" : "," );
        }
    }
}
