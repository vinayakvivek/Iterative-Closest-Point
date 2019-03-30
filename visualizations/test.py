from open3d import *
import copy

def draw_registration_result(source, target):
    source_temp = copy.deepcopy(source)
    target_temp = copy.deepcopy(target)
    source_temp.paint_uniform_color([1, 0.706, 0])
    target_temp.paint_uniform_color([0, 0.651, 0.929])
    draw_geometries([source_temp, target_temp])

cone = read_point_cloud("cone_original.txt", format='xyz')
cone.paint_uniform_color([0, 1, 0])

cone_transformed = read_point_cloud("cone_transformed.txt", format="xyz")
cone_transformed.paint_uniform_color([0, 0, 1])

cone_r = read_point_cloud("cone_registered.txt", format='xyz')
cone_r.paint_uniform_color([1, 0, 0])


draw_geometries([cone, cone_transformed, cone_r])