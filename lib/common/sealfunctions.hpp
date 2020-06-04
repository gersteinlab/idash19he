#pragma once

#include <cstddef>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <memory>
#include <limits>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "base64.hpp"
#include "constants.hpp"
#include "genome.hpp"
#include "math.hpp"
#include "seal/seal.h"

using namespace genome;
using namespace seal;
using namespace std;

namespace sealfunctions
{

inline Ciphertext base64ToCiphertext( shared_ptr<SEALContext> context, string s )
{
    stringstream ss ( base64::decode(s) );
    Ciphertext c;
    c.load(context, ss);
    return c;
}

inline string ciphertextToBase64( Ciphertext & c )
{
    stringstream ss;
    c.save(ss);
    auto strout = ss.str();
    return base64::encode( reinterpret_cast<const unsigned char*>(strout.c_str()), strout.size() );
}

inline vector<uint64_t> ciphertextToVectorInt( BatchEncoder & batchEncoder, Decryptor & decryptor, const Ciphertext & c )
{
    Plaintext m;
    decryptor.decrypt(c, m);
    vector<uint64_t> v;
    batchEncoder.decode(m, v);
    return v;
}

inline Ciphertext vectorIntToCiphertext( BatchEncoder & batchEncoder, Encryptor & encryptor, const vector<uint64_t> v )
{
    Plaintext m;
    batchEncoder.encode(v, m);
    Ciphertext c;
    encryptor.encrypt(m, c);
    return c;
}

bool isWithinRange( BatchEncoder & batchEncoder, Decryptor & decryptor, const Ciphertext & c, uint64_t min, uint64_t max )
{
    auto v = ciphertextToVectorInt( batchEncoder, decryptor, c );
    for ( auto e : v ) if ( e<min || e>max ) return false;
    return true;
}

bool isAllZero( BatchEncoder & batchEncoder, Decryptor & decryptor, const Ciphertext & c )
{    return isWithinRange( batchEncoder, decryptor, c, 0, 0 );  }

bool isAllType( BatchEncoder & batchEncoder, Decryptor & decryptor, const Ciphertext & c )
{    return isWithinRange( batchEncoder, decryptor, c, 0, nTypes-1 );  }

bool isAllBool( BatchEncoder & batchEncoder, Decryptor & decryptor, const Ciphertext & c )
{    return isWithinRange( batchEncoder, decryptor, c, 0, 1 );  }

Ciphertext add_vector( Evaluator & evaluator, vector<Ciphertext> & e )
{
    auto size = e.size();
    for ( auto n = 1<<sizeCeilPowerOfTwo(size); n>1; )
    {
        n >>= 1;
        for ( size_t i=n; i<size; i++ ) evaluator.add_inplace(e[i-n], e[i]);
        size = n;
    }
    return e[0];
}

inline Ciphertext gate_xnor( Evaluator & evaluator, RelinKeys & relinkeys, const Ciphertext & c1, const Ciphertext & c2, const Ciphertext & unit )
{
    Ciphertext c, tmp, r;
    evaluator.multiply(c1, c2, c);
    evaluator.relinearize_inplace(c, relinkeys);
    // evaluator.add_inplace(c, c); // 2ab
    // evaluator.add_inplace(c, unit); // 1+2ab
    evaluator.add(c1, c2, tmp); // a+b
    evaluator.sub(unit, tmp, tmp);
    evaluator.add_many( {c, tmp, c}, r);
    // evaluator.relinearize_inplace(r, relinkeys);
    // evaluator.sub_inplace(c, tmp); // 1+2ab-(a+b)
    return r;
}

Ciphertext add_if_equal( Evaluator & evaluator, RelinKeys & relinkeys, const vector<Ciphertext> & eq1, const vector<Ciphertext> & eq2, const Ciphertext & add_from, const Ciphertext & unit )
{
    auto size = eq1.size();
    vector<Ciphertext> e;
    for ( size_t i=0; i<size; i++ ) e.push_back( gate_xnor(evaluator, relinkeys, eq1[i], eq2[i], unit) );
    e.push_back(add_from);
    size++;
    for ( auto n = 1<<sizeCeilPowerOfTwo(size); n>1; )
    {
        n >>= 1;
        for ( size_t i=n; i<size; i++ )
        {
            evaluator.multiply_inplace(e[i-n], e[i]);
            evaluator.relinearize_inplace(e[i-n], relinkeys);
        }
        size = n;
    }
    return e[0];
}

void add_inplace_if_equal( Evaluator & evaluator, RelinKeys & relinkeys, const vector<Ciphertext> & eq1, const vector<Ciphertext> & eq2, const Ciphertext & add_from, Ciphertext & add_to, const Ciphertext & unit )
{
    auto size = eq1.size();
    vector<Ciphertext> e;
    for ( size_t i=0; i<size; i++ ) e.push_back( gate_xnor(evaluator, relinkeys, eq1[i], eq2[i], unit) );
    e.push_back(add_from);
    size++;
    for ( auto n = 1<<sizeCeilPowerOfTwo(size); n>1; )
    {
        n >>= 1;
        for ( size_t i=n; i<size; i++ )
        {
            evaluator.multiply_inplace(e[i-n], e[i]);
            evaluator.relinearize_inplace(e[i-n], relinkeys);
        }
        size = n;
    }
    evaluator.add_inplace(add_to, e[0]);
}

void conditional_add_inplace( Evaluator & evaluator, RelinKeys & relinkeys, const Ciphertext & cond, const Ciphertext & add_from, Ciphertext & add_to )
{
    Ciphertext value;
    evaluator.multiply(cond, add_from, value);
    evaluator.relinearize_inplace(value, relinkeys);
    evaluator.add_inplace(add_to, value);
}

Ciphertext equal( Evaluator & evaluator, RelinKeys & relinkeys, const vector<Ciphertext> & c1, const vector<Ciphertext> & c2, const Ciphertext & unit )
{
    auto size = c1.size();
    vector<Ciphertext> e;
    for ( size_t i=0; i<size; i++ ) e.push_back( gate_xnor(evaluator, relinkeys, c1[i], c2[i], unit) );
    for ( auto n = 1<<sizeCeilPowerOfTwo(size); n>1; )
    {
        n >>= 1;
        for ( size_t i=n; i<size; i++ )
        {
            evaluator.multiply_inplace(e[i-n], e[i]);
            evaluator.relinearize_inplace(e[i-n], relinkeys);
        }
        size = n;
    }
    return e[0];
}

vector<vector<Ciphertext>> readMatrixCiphertext( shared_ptr<SEALContext> context, string filename )
{
    vector<vector<Ciphertext>> m;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) )
    {
        vector<Ciphertext> v;
        stringstream ss(line);
        string buff;
        while ( ss >> buff ) v.push_back( base64ToCiphertext(context, buff) );
        m.push_back(v);
    }
    return m;
}

vector<Ciphertext> readVectorCiphertext( shared_ptr<SEALContext> context, string filename )
{
    vector<Ciphertext> v;
    ifstream infile( filename );
    string line;
    while ( getline(infile, line) ) v.push_back( base64ToCiphertext(context, line) );
    return v;
}

inline SmallModulus selectPlaintextModulus( int argc, char * argv[], size_t offset, uint64_t polyModulusDegree )
{
    SmallModulus plaintextModulus;
    if ( argc > offset+2 )
    {
        auto arg2 = stoul( argv[offset+2] );
        plaintextModulus = arg2 > constants::maxPlaintextModulusBitsize ? arg2 : PlainModulus::Batching(polyModulusDegree, arg2);
    }
    else if ( argc > offset+1 ) plaintextModulus = PlainModulus::Batching(polyModulusDegree, constants::defaultPlaintextModulusBitsize);
    else plaintextModulus = constants::defaultPlaintextModulus;
    return plaintextModulus;
}

inline void printParams( shared_ptr<SEALContext> context )
{
    if (!context)
    {
        cout << "Error: context is not set\n";
        return;
    }
    auto &contextData = *context->key_context_data();
    cout << "------------------------------------\n";
    cout << "| Encryption parameters :          |\n";
    cout << "|   scheme: BFV                    |\n";
    cout << "|   poly modulus degree: " << setfill(' ') << setw(9) << contextData.parms().poly_modulus_degree() << " |\n";
    cout << "|   coeff modulus size (bits): " << setfill(' ') << setw(3) << contextData.total_coeff_modulus_bit_count() << " |\n";
    cout << "|   plain modulus: " << setfill(' ') << setw(15) << contextData.parms().plain_modulus().value() << " |\n";
    cout << "|   batching status:      " << ( context->first_context_data()->qualifiers().using_batching ? " enabled" : "disabled" ) << " |\n";
}

}
