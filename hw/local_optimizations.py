#!/usr/bin/python3

from basic_blocks import basic_blocks
import briltxt
from collections import defaultdict
import json
import sys
import traceback

COMMUTATIVE_OPS = ['add', 'mul', 'eq', 'and', 'or']

def lvn(block, cse=False): # operates on one basic block
    val_ind = 1
    var2val= {}
    val2var = defaultdict(lambda: [])
    exp2var = {}

    ret_block = []

    for instr in block:
        # basic blocks can contain un-jumped-to labels since I merge blocks
        if 'label' in instr or instr['op'] == 'nop': continue
        # TODO this has to be brought into it
        if instr['op'] == 'print': 
            ret_block.append(instr)
            continue

        op = instr['op']
        dest = instr['dest']
        if op == 'const':
            var2val[dest] = val_ind
            val2var[val_ind].append(dest)
            val_ind += 1
            ret_block.append(instr)
            continue

        args = instr['args']

        if op in COMMUTATIVE_OPS: args = sorted(args)

        arg_vals = list(map(lambda a: var2val[a], args))
        key = tuple([op] + arg_vals)
        # I'm rewriting wrong!
        if key in exp2var:
            var2val[dest] = var2val[exp2var[key]]
            val2var[var2val[dest]].append(dest)
            if cse:
                continue
        else:
            exp2var[key] = dest
            var2val[dest] = val_ind
            val2var[val_ind].append(dest)
            val_ind += 1
            if cse:
                instr['args'] = [val2var[val][0] for val in arg_vals]
        ret_block.append(instr)
    print(val2var, var2val)
    return ret_block

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Need to provide input")
        sys.exit(-1)
    try:
        with open(sys.argv[1], 'r') as input_file:
            source = json.loads(input_file.read())
            blocks = basic_blocks(source)
            for block in blocks:
                for instr in lvn(block, True):
                    briltxt.print_instr(instr)
    except:
        traceback.print_exc()
        sys.exit(-1)
