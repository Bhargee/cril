#!/usr/bin/python3.5
import briltxt
import json
import sys


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Need to supply an input file')
        sys.exit(-1)

    ifn = sys.argv[1]
    ext = (ifn.split('.'))[1]
    if not ext in ['json', 'bril']:
        print('File needs to end in  \'.json\' or \'.bril\'')
        sys.exit(-1)
    try:
        with open(ifn, 'r') as ifi:
            if ext == 'json': 
                briltxt.print_prog(json.loads(ifi.read()))
            elif ext == 'bril':
                print(briltxt.parse_bril(ifi.read()))
    except Exception as e:
        print(e)
        sys.exit(-1)

