ID=1# database ID
LIMIT_SNPS=9# number of tag SNPs used to find the target SNP
N_THREADS=8

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
QUERIER=querier

# Data files
ENCRYPTED_QUERY=encryptedQuery${ID}${TMPEXT}
ENCRYPTED_TARGET=encryptedProbs${ID}${TMPEXT}

CLOSEST_SNPS_DATA_PREAMBLE=closestTagSNPs
CLOSEST_SNPS_DATA=${CLOSEST_SNPS_DATA_PREAMBLE}${ID}_${LIMIT_SNPS}${DATAEXT}
TRAIN_DATA_PREAMBLE=train
TRAIN_DATA=${TRAIN_DATA_PREAMBLE}${ID}_${LIMIT_SNPS}${DATAEXT}
TAG_IDS_PREAMBLE=tagIDs
TAG_IDS=${TAG_IDS_PREAMBLE}${ID}${DATAEXT}
TARGET_IDS_PREAMBLE=targetIDs
TARGET_IDS=${TARGET_IDS_PREAMBLE}${ID}${DATAEXT}

# DIRECTORIES
ALICE=../alice

all: compile run

run: query send-result

%: %.cpp
	${CC} ${FLAGS} ${INCS} $< -o $@${EXT} ${LIBS}

clean:
	rm -f *${EXT} *${KEYEXT} *${TMPEXT}

clean-all: clean
	rm -f ${CLOSEST_SNPS_DATA_PREAMBLE}*${DATAEXT}
	rm -f ${TRAIN_DATA_PREAMBLE}*${DATAEXT}
	rm -f ${TAG_IDS_PREAMBLE}*${DATAEXT}
	rm -f ${TARGET_IDS_PREAMBLE}*${DATAEXT}

compile:
	make ${QUERIER}

query:
	./${QUERIER}${EXT} ${TRAIN_DATA} ${CLOSEST_SNPS_DATA} ${ENCRYPTED_QUERY} ${ENCRYPTED_TARGET} ${N_THREADS}

send-result:
	cp ${ENCRYPTED_TARGET} ${ALICE}
