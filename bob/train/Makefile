ID=1# database ID
LIMIT_SNPS=9# number of tag SNPs used to find the target SNP
LIMIT_INDIVIDUALS=10# number of individuals used to find the target SNP
RATIO=100,0# ratio of individuals used as reference or test during training

# COMPILER
CC=g++
FLAGS=-O3 -std=c++17
INCS=-I../../lib/common
LIBS=-static-libgcc -static-libstdc++

# Programs
PYTHON=python3

# FILES
EXT=.exe
DATAEXT=.data
TMPEXT=.tmp
PYEXT=.py

# original tag and target SNP databases
ifeq (${ID}, 1)
ORIG_TAG=sorted_tag_SNPs_1k_genotypes${DATAEXT}
else
ORIG_TAG=sorted_tag_SNPs_10k_genotypes${DATAEXT}
endif
ORIG_TARGET=sorted_target_SNP_genotypes${DATAEXT}
ORIG=${ORIG_TAG},${ORIG_TARGET}

# temporary files
QUERY_TAG=query_tag_snps${TMPEXT}
QUERY_TARGET=query_target_snps${TMPEXT}
RANDOM_TAG=random_tag_snps${TMPEXT}
RANDOM_TARGET=random_target_snps${TMPEXT}
REF_TAG=ref_tag_snps${TMPEXT}
REF_TARGET=ref_target_snps${TMPEXT}
REORD=${RANDOM_TAG},${RANDOM_TARGET}
SPLIT_TAG_TO=${REF_TAG},${QUERY_TAG}
SPLIT_TARGET_TO=${REF_TARGET},${QUERY_TARGET}

# C++ programs
FINDER=closestTagSNPsFinder
TESTER=tester
TRAINER=trainer

# Python scripts
REORDER=reorder${PYEXT}
SPLITTER=splitter${PYEXT}

# Data files
CLOSEST_SNPS_DATA_PREAMBLE=closestTagSNPs
CLOSEST_SNPS_DATA=${CLOSEST_SNPS_DATA_PREAMBLE}${ID}_${LIMIT_SNPS}${DATAEXT}
TRAIN_DATA_PREAMBLE=train
TRAIN_DATA=${TRAIN_DATA_PREAMBLE}${ID}_${LIMIT_SNPS}${DATAEXT}
TAG_IDS_PREAMBLE=tagIDs
TAG_IDS=${TAG_IDS_PREAMBLE}${ID}${DATAEXT}
TARGET_IDS_PREAMBLE=targetIDs
TARGET_IDS=${TARGET_IDS_PREAMBLE}${ID}${DATAEXT}

all: split compile find-closest-tagSNPs train test

run: split find-closest-tagSNPs train test

%: %.cpp
	${CC} ${FLAGS} ${INCS} $< -o $@${EXT} ${LIBS}

clean:
	rm -f *${EXT} *${TMPEXT}

clean-all:
	rm -f ${CLOSEST_SNPS_DATA_PREAMBLE}*${DATAEXT}
	rm -f ${TRAIN_DATA_PREAMBLE}*${DATAEXT}
	rm -f ${TAG_IDS_PREAMBLE}*${DATAEXT}
	rm -f ${TARGET_IDS_PREAMBLE}*${DATAEXT}

compile:
	make ${FINDER}
	make ${TESTER}
	make ${TRAINER}

deploy:
	cp ${CLOSEST_SNPS_DATA} ..
	cp ${TRAIN_DATA} ..
	cp ${TAG_IDS} ..
	cp ${TARGET_IDS} ..

find-closest-tagSNPs:
	./${FINDER}${EXT} ${REF_TAG} ${REF_TARGET} ${CLOSEST_SNPS_DATA} ${TAG_IDS} ${TARGET_IDS}

split:
	${PYTHON} ${REORDER} ${ORIG} ${REORD}
	${PYTHON} ${SPLITTER} ${RANDOM_TAG} ${SPLIT_TAG_TO} ${RATIO}
	${PYTHON} ${SPLITTER} ${RANDOM_TARGET} ${SPLIT_TARGET_TO} ${RATIO}

test:
	./${TESTER}${EXT} ${TRAIN_DATA} ${CLOSEST_SNPS_DATA} ${QUERY_TAG} ${QUERY_TARGET} ${LIMIT_SNPS}

train:
	./${TRAINER}${EXT} ${REF_TAG} ${REF_TARGET} ${CLOSEST_SNPS_DATA} ${TRAIN_DATA} ${LIMIT_SNPS} ${LIMIT_INDIVIDUALS}
