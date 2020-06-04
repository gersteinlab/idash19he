import random
import sys

offset = 4

def main():
    if len(sys.argv) != 3:
        print('Usage: python3 reorderdb.py FIN FOUT')
        print('FIN: input filenames (e.g. tag,target) ')
        print('FOUT: output filenames (e.g. rtag,rtarget)')
        exit(1)
    tagfile,targetfile = sys.argv[1].split(',')
    tagfileout,targetfileout = sys.argv[2].split(',')
    nIndividuals = countIndividuals(tagfile)
    positions = createRandomOrder(nIndividuals)
    reorderFile(tagfile, tagfileout, positions)
    reorderFile(targetfile, targetfileout, positions)

def createRandomOrder( n ):
    v = []
    while len(v) < n:
        r = random.randrange(0, n)
        if r not in v:
            v.append(r)
    return v

def countIndividuals(filename):
    with open(filename) as f:
        for line in f:
            return len( line.split('\t') ) - offset

def reorder(vin, positions):
    vout = []
    for pos in positions:
        vout.append(vin[pos])
    return vout

def reorderFile(filenamein, filenameout, positions):
    with open(filenameout, 'w') as fout:
        with open(filenamein, 'r') as fin:
            for line in fin:
                elements = line.strip().split('\t')
                head = elements[:offset]
                individuals = reorder( elements[offset:], positions )
                strout = ''
                for item in head+individuals:
                    strout += '{}\t'.format(item)
                strout = strout[:-1] + '\n'
                fout.write( strout )

if __name__ == '__main__':
    main()
