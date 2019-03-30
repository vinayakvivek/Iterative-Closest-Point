import sys
import os


if __name__ == '__main__':

    file_name = sys.argv[1]
    out_name = sys.argv[2]

    points = None
    with open(file_name, 'r') as f:
        points = f.readlines();

    points = [" ".join(x.strip().split(',')) + "\n" for x in points]

    with open(out_name, 'w') as f:
        f.writelines(points)