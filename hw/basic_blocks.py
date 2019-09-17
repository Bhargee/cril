#!/usr/bin/python3
import briltxt
import json
import sys
import traceback

def print_blocks(blocks):
    for block in blocks:
        leader = block[0]
        if 'label' in leader:
            print('block {}: '.format(leader['label']))
            block = block[1:]
        else:
            print('anonymous block:')

        for instr in block:
            if 'label' in instr: print(instr['label'] + ': ')
            else: print(briltxt.instr_to_string(instr))


def _tail(l):
    if len(l) is 0: return []
    else: return l[len(l)-1]


def basic_blocks(blocks):
    main = blocks['functions'][0]

    blocks, block = [], []
    terminators = ['ret', 'jmp', 'br']
    jump_dests = set()

    for instr in main['instrs']:
        if 'op' in instr:
            block.append(instr)
            if instr['op'] in terminators:
                if instr['op'] == 'jmp':
                    jump_dests.add(instr['args'][0])
                elif instr['op'] == 'br':
                    jump_dests.add(instr['args'][1])
                    jump_dests.add(instr['args'][2])
                blocks.append(block)
                block = []
        else: # label
            if len(block) > 0: blocks.append(block) # for loops
            block = [instr]

    if len(block) > 0:
        blocks.append(block)

    # merge blocks
    final_blocks = []
    for block in blocks:
        leader = block[0]
        if 'label' in leader and not leader['label'] in jump_dests:
            tail = _tail(final_blocks)
            tail += block
        else: final_blocks.append(block)

    return final_blocks



if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("need an input json file")
        sys.exit(-1)
    try:
        with open(sys.argv[1], 'r') as input_file:
            source = json.loads(input_file.read())
            print_blocks(basic_blocks(source))
    except Exception as e:
        traceback.print_exc()
        sys.exit(-1)
