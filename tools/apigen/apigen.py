# quick and dirty pub function extractor
# expects strict layout of module.h
# not robust under any conditions

import re
import os
import sys
import argparse

START = re.compile('[#]define \s+ [\w_]+_H_', re.VERBOSE)
END0 = re.compile('[#]ifdef \s+ [\w_]+_C_', re.VERBOSE)
END1 = re.compile('[#]endif', re.VERBOSE)


def extract_pub_section(fname):
    f = open(fname)
    data = f.read()
    f.close()

    start = START.search(data)

    if start:
        data = data[start.end():]

        end = END0.search(data) or END1.search(data)
        if end:
            return data[:end.start()]
    return None

def gen_module_entry(filename):
    out = '//--- %s' % os.path.basename(filename)
    out += extract_pub_section(filename)
    return out


def main():
    parser = argparse.ArgumentParser(description='export public function from modules headers')
    parser.add_argument('headers', help='list of input headers', nargs='+', type=str)
    parser.add_argument('--guard', help='name for output header guard', type=str, default=None)
    parser.add_argument('-o', '--output', help='resulting output header', type=str)
    args = parser.parse_args()

    text = '\n'.join([gen_module_entry(name) for name in args.headers])

    if args.guard:
        text = '#ifndef %s\n#define %s\n\n%s\n#endif' % (args.guard, args.guard, text)

    if args.output:
        open(args.output, 'w').write(text)
    else:
        print(text)

main()
