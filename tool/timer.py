import sys

def main():
    if len(sys.argv) != 5:
        print('Usage: python3 timer.py time_encryption time_computation time_decryption output')
        exit(1)
    fileTenc, fileTcomp, fileTdec, fileout = sys.argv[1:]

    with open(fileTenc) as fin:
        tenc = fin.read().split(' ')[2].split('e')[0].split(':')
        tenc = float(tenc[0]) * 60 + float(tenc[1])
    with open(fileTcomp) as fin:
        tcomp = fin.read().split(' ')[2].split('e')[0].split(':')
        tcomp = float(tcomp[0]) * 60 + float(tcomp[1])
    with open(fileTdec) as fin:
        tdec = fin.read().split(' ')[2].split('e')[0].split(':')
        tdec = float(tdec[0]) * 60 + float(tdec[1])
    ttotal = tenc+tcomp+tdec

    with open(fileout, 'w') as fout:
        fout.write( '{},{},{},{}'.format('%.2f' % ttotal, '%.2f' % tenc, '%.2f' % tcomp, '%.2f' % tdec) )

if __name__ == '__main__':
    main()
