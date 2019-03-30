import sys
import os
from os.path import join
from open3d import *
import copy


def draw(original, transformed, final):
    original_pcd = read_point_cloud(original, format='xyz')
    original_pcd.paint_uniform_color([0, 1, 0])

    transformed_pcd = read_point_cloud(transformed, format="xyz")
    transformed_pcd.paint_uniform_color([0, 0, 1])

    final_pcd = read_point_cloud(final, format='xyz')
    final_pcd.paint_uniform_color([1, 0, 0])

    draw_geometries([original_pcd, transformed_pcd, final_pcd])


if __name__ == '__main__':

    if len(sys.argv) < 2:
        print("provide data dir as argument.")
        sys.exit(-1)

    data_dir = sys.argv[1]

    original = join(data_dir, "original.txt")
    transformed = join(data_dir, "transformed.txt")
    final = join(data_dir, "final.txt")
    draw(original, transformed, final)


