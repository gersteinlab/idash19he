#include <fstream>
#include <iostream>
#include "constants.hpp"
#include "seal/seal.h"
#include "sealfunctions.hpp"

using namespace seal;
using namespace sealfunctions;
using namespace std;

int main( int argc, char* argv[] )
{
    size_t offset=0;
    uint64_t polyModulusDegree = argc>offset+1 ? 1<<stoul(argv[offset+1]) : constants::defaultPolyModulusDegree;
    auto plaintextModulus = selectPlaintextModulus( argc, argv, offset, polyModulusDegree );

    EncryptionParameters params(scheme_type::BFV);
    params.set_poly_modulus_degree(polyModulusDegree);
    params.set_coeff_modulus(CoeffModulus::BFVDefault(polyModulusDegree));
    params.set_plain_modulus(plaintextModulus);
    auto context = SEALContext::Create(params);
    auto qualifiers = context->first_context_data()->qualifiers();
    KeyGenerator keygen(context);
    PublicKey publickey = keygen.public_key();
    SecretKey secretkey = keygen.secret_key();
    RelinKeys relinkeys = keygen.relin_keys();
    ofstream foutPK ( constants::publicKeyFile, ios::binary );
    publickey.save(foutPK);
    ofstream foutSK ( constants::secretKeyFile, ios::binary );
    secretkey.save(foutSK);
    ofstream foutRK ( constants::relinKeysFile, ios::binary );
    relinkeys.save(foutRK);
    printParams(context);
    if ( qualifiers.using_batching )
    {
        BatchEncoder batchEncoder(context);
        size_t size = batchEncoder.slot_count();
        cout << "|   batching slots: " << setfill(' ') << setw(14) << size <<  " |\n";
    }
    cout << "------------------------------------\n";
    cout << "|     Keys have been generated     |\n";
    cout << "------------------------------------\n";
}
