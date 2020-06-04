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

int main( int argc, char* argv[] )
{
    if ( argc<3 )
    {
        cout << "Usage encrypt.exe filename output [polyModulusDegree] [plaintextModulus]\n";
        cout << " - filename: a textfile to be encrypted\n";
        cout << " - output: the encrypted output\n";
        cout << " - polyModulusDegree: the exponent of a power of two that will become the polynomial modulus degree\n";
        cout << " - plaintextModulus: the plaintext modulus. If it is not great than " << constants::maxPlaintextModulusBitsize;
        cout << ", the input will be considered a bitsize and the program will try to find a plaintext modulus with that bitsize that works for batching\n";
        return 1;
    }

    auto indexes = readMatrix3D( argv[1] );
    auto fileout = string( argv[2] );
    size_t offset=2;
    uint64_t polyModulusDegree = argc>offset+1 ? 1<<stoul(argv[offset+1]) : constants::defaultPolyModulusDegree;
    auto plaintextModulus = selectPlaintextModulus( argc, argv, offset, polyModulusDegree );
    cout << indexes.size() << " x " << indexes[0].size() << " x " << indexes[0][0].size() << "\n";

    EncryptionParameters params(scheme_type::BFV);
    params.set_poly_modulus_degree(polyModulusDegree);
    params.set_coeff_modulus(CoeffModulus::BFVDefault(polyModulusDegree));
    params.set_plain_modulus(plaintextModulus);
    auto context = SEALContext::Create(params);

    PublicKey publickey;
    ifstream finPK ( constants::publicKeyFile, ios::binary );
    publickey.load(context, finPK);

    Encryptor encryptor(context, publickey);
    BatchEncoder batchEncoder(context);

    size_t size = batchEncoder.slot_count();
    auto nIndividuals = indexes.size();
    auto nTargetSNPs = indexes[0].size();
    auto nBits = indexes[0][0].size();
    size_t individualsPerCiphertext = size / nTargetSNPs;
    auto maxSize = individualsPerCiphertext * nTargetSNPs;

    {
        ofstream foutCFG( constants::configFile );
        foutCFG << nTargetSNPs << "\n" << nIndividuals << "\n";
    }

    // put int(nSlots/nTargetSNPs) individuals in each vector and encrypt
    vector<vector<Ciphertext>> ciphertexts(nBits);
    vector<vector<uint64_t>> sets(nBits, vector<uint64_t>(size, 0) );
    size_t j=0;
    for ( size_t i=0; i<nIndividuals; i++ )
    {
        for ( size_t t=0; t<nTargetSNPs; t++ )
        {
            for ( size_t b=0; b<nBits; b++ ) sets[b][j] = indexes[i][t][b];
            j++;
            if ( j >= maxSize )
            {
                j=0;
                for ( size_t b=0; b<nBits; b++ ) ciphertexts[b].push_back( vectorIntToCiphertext(batchEncoder, encryptor, sets[b]) );
                sets = vector<vector<uint64_t>>(nBits, vector<uint64_t>(size, 0) );
            }
        }
    }
    if ( j ) for ( size_t b=0; b<nBits; b++ ) ciphertexts[b].push_back( vectorIntToCiphertext(batchEncoder, encryptor, sets[b]) );

    // encrypt plaintexts and convert the ciphertexts to base64 in order to save the whole query in a single file
    auto nQueries = ciphertexts[0].size();
    ofstream fout( fileout );
    for ( size_t q=0; q<nQueries; q++ )
    {
        for ( size_t b=0; b<nBits; b++ ) fout << ciphertextToBase64( ciphertexts[b][q] ) << " ";
        fout << "\n";
    }
}
