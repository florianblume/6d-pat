import os
import convert_tless_cam_info
import convert_tless_gt

def process_subdir(subdir):
    image_folders = [os.path.join(subdir, image_folder) for image_folder in os.listdir(subdir)]
    for image_folder in image_folders:
        files = os.listdir(image_folder)
        assert 'gt.yml' in files, f'Ground truth file gt.yml not found in {image_folder}'
        gt_index = files.index('gt.yml')
        assert 'info.yml' in files, f'Cam info file info.yml not found in {image_folder}'
        cam_index = files.index('info.yml')
        assert 'rgb' in files, f'Image folder \'rgb\' not found in {image_folder}'
        rgb_index = files.index('rgb')
        files = [os.path.join(image_folder, _file) for _file in files]
        convert_tless_cam_info.convert_tless_cam_info(files[cam_index],
                                                      files[rgb_index],
                                                      'jpg',
                                                      os.path.join(image_folder, 'rgb', 'info.json'))
        convert_tless_gt.convert_tless_gt(files[gt_index],
                                          files[rgb_index],
                                          'jpg',
                                          os.path.abspath(os.path.join(subdir, '..', 'models_cad')),
                                          os.path.join(image_folder, 'gt.json'))

def convert_tless(path):
    subdirs = [subdir for subdir in os.listdir(path)]
    assert 'models_cad' in subdirs, 'Model files need to be present (expected to be in \'models_cad\' folder).'
    if 'train' in subdirs:
        process_subdir(os.path.join(path, 'train'))
    if 'test' in subdirs:
        process_subdir(os.path.join(path, 'test'))

if __name__ == '__main__':
    import argparse

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='This script converts cam info and gt files of T-Less in a folder.')
    parser.add_argument("directory", help="The path to the folder where (parts of) T-Less is stored.")

    args = parser.parse_args()
    convert_tless(args.directory)