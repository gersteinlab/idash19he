#pragma once

#include <string>

using std::string;

namespace constants
{
    const uint64_t defaultPolyModulusDegree = 1<<15;
    const uint64_t defaultPlaintextModulus = 65537; // 114689;
    const uint64_t defaultPlaintextModulusBitsize = 17;
    const uint64_t maxPlaintextModulusBitsize = 1<<7;
    const uint64_t shifter = 7;
    const uint64_t multiplier = (1<<shifter)-1;
    const string publicKeyFile = "public.key";
    const string secretKeyFile = "secret.key";
    const string relinKeysFile = "relin.key";
    const string configFile = "config.tmp";
}
