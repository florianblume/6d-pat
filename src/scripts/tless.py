import os
import yaml

def load_images(path):
    cam_info_file_path = os.path.abspath(os.path.join(path, '..', 'info.yml'))
    with open(cam_info_file_path, 'r') as cam_info_file:
        cam_info = yaml.load(cam_info_file)

def load_object_models(path):
    print('load object models')
    pass

def load_poses(path):
    print('load poses')
    pass

def persist_pose(path, pose):
    print('persist pose')
    pass