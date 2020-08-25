ID=10# database ID
LIMIT_SNPS=5# number of tag SNPs used to find the target SNP

# COMPILER
CC=g++
FLAGS=-O3 -std=c++17
INCS=-I../lib/common -I../lib/seal_unx/include
LIBS=-L../lib/seal_unx/lib -lseal -pthread -static-libgcc -static-libstdc++

# FILES
EXT=.exe# file extension
DATAEXT=.data
KEYEXT=.key
TMPEXT=.tmp

# C++ programs
DECRYPTOR=decryptor
ENCRYPTOR=encryptor
KEYGEN=keygen
PARSER=parser

# Data files
QUERY=query_tag_SNPs_${ID}_genotypes${DATAEXT}
QUERY_PARSED=queryTagSNPsParsed${ID}${TMPEXT}
ENCRYPTED_QUERY=encryptedQuery${ID}${TMPEXT}
CLOSEST_SNPS_DATA_PREAMBLE=closestTagSNPs
CLOSEST_SNPS_DATA=${CLOSEST_SNPS_DATA_PREAMBLE}${ID}_${LIMIT_SNPS}${DATAEXT}
TAG_IDS_PREAMBLE=tagIDs
TAG_IDS=${TAG_IDS_PREAMBLE}${ID}${DATAEXT}
TARGET_IDS_PREAMBLE=targetIDs
TARGET_IDS=${TARGET_IDS_PREAMBLE}${ID}${DATAEXT}
ENCRYPTED_TARGET=encryptedProbs${ID}${TMPEXT}
QUERY_DECRYPTED_PROBS=decryptedProbs${ID}${DATAEXT}
QUERY_DECRYPTED_TARGETS=decryptedTargets${ID}${DATAEXT}

# DIRECTORIES
BOB=../bob

# KEYS
PUBLIC_KEY=public${KEYEXT}
SECRET_KEY=secret${KEYEXT}
RELIN_KEYS=relin${KEYEXT}

all: compile run

run: request-closest-tagSNPs parse generate-keys encrypt send

%: %.cpp
	${CC} ${FLAGS} ${INCS} $< -o $@${EXT} ${LIBS}

clean:
	rm -f *${EXT} *${KEYEXT} *${TMPEXT}
	rm -f ${CLOSEST_SNPS_DATA_PREAMBLE}*${DATAEXT}
	rm -f ${TAG_IDS_PREAMBLE}*${DATAEXT}
	rm -f ${TARGET_IDS_PREAMBLE}*${DATAEXT}

compile:
	make ${KEYGEN}
	make ${ENCRYPTOR}
	make ${DECRYPTOR}
	make ${PARSER}

decrypt:
	./${DECRYPTOR}${EXT} ${ENCRYPTED_TARGET} ${TARGET_IDS} ${QUERY_DECRYPTED_PROBS} ${QUERY_DECRYPTED_TARGETS}

encrypt:
	./${ENCRYPTOR}${EXT} ${QUERY_PARSED} ${ENCRYPTED_QUERY}

generate-keys:
	./${KEYGEN}${EXT}

parse:
	./${PARSER}${EXT} ${QUERY} ${CLOSEST_SNPS_DATA} ${QUERY_PARSED} ${LIMIT_SNPS}

request-closest-tagSNPs:
	cp ${BOB}/${CLOSEST_SNPS_DATA} .
	cp ${BOB}/${TAG_IDS} .
	cp ${BOB}/${TARGET_IDS} .

send:
	cp ${ENCRYPTED_QUERY} ${PUBLIC_KEY} ${RELIN_KEYS} ${BOB}/
