# basic blocks
import briltxt
import json
import sys

import pprint

def pg(bbl):
    for bb in bbl:
        l = bb[0]
        if 'label' in l:
            print('{}: '.format(l['label']))
            bb = bb[1:]
        else:
            print('anonymous block:')
        for i in bb:
            print(briltxt.instr_to_string(i))

def bbs(fs):
    bbl = []
    jdests = set()
    jbb = {}
    terms = ['ret', 'jmp', 'br']
    bb = []
    for i in fs['instrs']:
        if 'op' in i:
            bb.append(i)
            if i['op'] in terms:
                if i['op'] == 'jmp' or i['op'] == 'br':
                    jdests.add(i['args'][0])
                elif i['op'] == 'br':
                    jdests.add(i['args'][1])
                bbl.append(bb)
                bb = []
        else:
            if i['label'] in jdests:
                jdests.remove(i['label'])
                bbl.append(bb)
                bb = [i]
            else:
                bb.append(i)
                jbb[i['label']] = bb

    if len(bb) > 0:
        bbl.append(bb)

    # split up bbs if there are jumped to labels unaccounted for
    for lbl in jdests:
        if lbl in jbb:
            bb = jbb[lbl]
            bbl.remove(jbb[lbl])
            ind = 0
            for i in bb:
                if 'label' in i and i['label'] == lbl:
                    ind = bb.index(i)
                    break
            bbl.append([bb[i] for i in range(0, ind)])
            bbl.append([bb[i] for i in range(ind, len(bb))])

    return bbl



if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("need an input json file")
        sys.exit(-1)
    try:
        with open(sys.argv[1], 'r') as ifi:
            s = json.loads(ifi.read())
            pg(bbs(s['functions'][0]))
    except Exception as e:
        print(e)
        sys.exit(-1)
