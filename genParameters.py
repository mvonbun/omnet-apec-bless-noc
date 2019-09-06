#!/usr/bin/env python


def is_rate_range(x):
    return (len(x) == 1) and (re.match("[0-9]+.[0-9]*:[0-9]+.[0-9]*:[0-9]+.[0-9]*", x[0]) != None)


def is_int_range(x):
    return (len(x) == 1) and (re.match("^[0-9]+:[0-9]+:?[0-9]*$", x[0]) != None)



# script usage
if __name__ == "__main__":
    # execute only if run as a script

    # command line interface
    # https://docs.python.org/3/library/argparse.html
    import argparse
    import sys
    import re
    import numpy
    import os

    parser = argparse.ArgumentParser(
        description='Return Omnet++ Paramter Set.')

    # positional arguments: output file
    parser.add_argument('outfile',
                        nargs='?', type=argparse.FileType('w'),
                        default=sys.stdout,
                        help='optional output file')

    # options with arguments
    parser.add_argument('-g', '--generationRate',
                        nargs='+',
                        default=[0.1],
                        help='List (0.1 0.2 0.3) or range (0.1:0.1:0.3) of generation rates.')
    parser.add_argument('-l', '--numFlitMin',
                        nargs='+',
                        default=[1],
                        help='List (1 2 3) or range (1:2:5) of number of minimum flits per packet.')
    parser.add_argument('-u', '--numFlitMax',
                        nargs='+',
                        default=[8],
                        help='List (1 2 3) or range (1:2:5) of number of maximum flits per packet.')
    parser.add_argument('-e', '--extended',
                        help='Add extended output. Use to pipe output to end of an ini file.',
                        action='store_true', default=False)
    parser.add_argument('-i', '--iterateJoint',
                        help='Iterate through min and max flits jointly.',
                        action='store_true', default=False)

    args = parser.parse_args()

    gen_rate = args.generationRate
    numflit_min = args.numFlitMin
    numflit_max = args.numFlitMax

    if is_rate_range(gen_rate):
        tmp = gen_rate[0].split(":")
        gen_rate = []
        start = float(tmp[0])
        step = float(tmp[1])
        stop = float(tmp[2])
        gen_rate = numpy.arange(start, stop, step)

    if is_int_range(numflit_min):
        tmp = numflit_min[0].split(":")
        numflit_min = []
        start = int(tmp[0])
        if len(tmp) == 3:
            step = int(tmp[1])
        else:
            step = 1
        stop = int(tmp[-1])
        numflit_min = numpy.arange(start, stop, step)
    else:
        numflit_min = [int(x) for x in numflit_min]
        # print(numflit_min)

    if is_int_range(numflit_max):
        tmp = numflit_max[0].split(":")
        numflit_max = []
        start = int(tmp[0])
        if len(tmp) == 3:
            step = int(tmp[1])
        else:
            step = 1
        stop = int(tmp[-1])
        numflit_max = numpy.arange(start, stop, step)
    else:
        numflit_max = [int(x) for x in numflit_max]
        # print(numflit_max)

    send_interval = []
    genRate_out = []
    flit_min_out = []
    flit_max_out = []
    if args.iterateJoint:
        for rate in gen_rate:
            for flit_min, flit_max in zip(numflit_min, numflit_max):
                if flit_max >= flit_min:
                    send_interval.append(
                        (flit_min + flit_max) / 2.0 / float(rate))
                    genRate_out.append(rate)
                    flit_min_out.append(flit_min)
                    flit_max_out.append(flit_max)
    else:
        for rate in gen_rate:
            for flit_min in numflit_min:
                for flit_max in numflit_max:
                    if flit_max >= flit_min:
                        send_interval.append(
                            (flit_min + flit_max) / 2.0 / float(rate))
                        genRate_out.append(rate)
                        flit_min_out.append(flit_min)
                        flit_max_out.append(flit_max)

    s_send_interval = "**.source.send_interval = exponential(${sendInterval=" + ", ".join(
        map(str, send_interval)) + "}s)"
    s_generation_rate = "Mesh.generation_rate = ${genRate=" + \
        ", ".join(map(str, genRate_out)) + " ! sendInterval}"
    s_flit_min = "Mesh.num_flit_min = ${numflitMin=" + \
        ", ".join(map(str, flit_min_out)) + " ! sendInterval}"
    s_flit_max = "Mesh.num_flit_max = ${numflitMax=" + \
        ", ".join(map(str, flit_max_out)) + " ! sendInterval}"

    if args.extended:
        print('# --- generation rate')
        if args.iterateJoint:
            num_combinations = len(gen_rate) * len(numflit_min)
        else:
            num_combinations = len(gen_rate) * \
                len(numflit_min) * len(numflit_max)

        print('#     with {} combinations: <{}> rates, <{}> numflit_min, <{}> numflit_max'.format(
            num_combinations,
            len(gen_rate), len(numflit_min), len(numflit_max))
        )
        print('#     generated using:')
        print('#     {} -e -g {} -l {} -u {}'.format(
            os.path.basename(__file__),
            args.generationRate, args.numFlitMin, args.numFlitMax).replace('[', '').replace(']', '').replace(',', '').replace('\'', ''))

    print(s_send_interval)
    print(s_generation_rate)
    print(s_flit_min)
    print(s_flit_max)

    if args.extended:
        print('# --- uniform packet generation')
        print('**.source.num_flit = intuniform(${numflitMin}, ${numflitMax})')
