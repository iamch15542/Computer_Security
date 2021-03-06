# -*- coding: utf-8 -*-

import argparse

from score import main as m


def get_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('inputs', type=str, nargs='+')
    parser.add_argument('--answer', type=str, nargs='+', required=True)

    return parser


def main():
    args = get_parser().parse_args()

    for fi in args.inputs:
        try:
            m(fi, args.answer)
        except Exception as e:
            print(f'bang: {fi}', e)


if __name__ == '__main__':
    main()
