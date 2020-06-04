import os
import sys

offset = 4

def main():
    if len(sys.argv) != 4:
        print('Usage: python3 splitdb.py FIN FOUT P')
        print('FIN: input filename')
        print('FOUT: output filenames (e.g. name1,name2) # x')
        print('P: percentage of FIN that goes to its respective FOUT (e.g. 70,30)')
        print('NOTE: P is normalized, so you can either use percentages or the # of elements')
        exit(1)
    filenamein, filenameout, p = sys.argv[1:]
    filenameout = filenameout.split(',')
    p = [int(x) for x in p.split(',')]
    nIndividuals = countIndividuals(filenamein)
    ratio = nIndividuals / sum(p)
    np = [ round(ratio*x) for x in p ]
    splitFile(filenamein, filenameout, np)

def countIndividuals(filename):
    with open(filename) as f:
        for line in f:
            return len( line.split('\t') ) - offset

def splitFile(filenamein, filenameouts, nps):
    for filenameout in filenameouts:
        if os.path.isfile(filenameout):
            os.remove(filenameout)

    with open(filenamein) as fin:
        for line in fin:
            items = line[:-1].split('\t')
            header = items[:offset]
            genotypes = items[offset:]
            i=0
            idxI=0
            idxF=nps[i]
            for filenameout in filenameouts:
                with open(filenameout, 'a') as fout:
                    strout = ''
                    for item in header+genotypes[idxI:idxF]:
                        strout += '{}\t'.format(item)
                    strout = strout[:-1] + '\n'
                    fout.write(strout)
                i += 1
                idxI = idxF
                idxF += nps[i] if i < len(nps) else 0

if __name__ == '__main__':
    main()
