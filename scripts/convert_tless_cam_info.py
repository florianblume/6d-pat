def convert_tless_cam_info(cam_info_path, images_path, image_extension, output_path):
    import os
    import tless_inout as inout
    import util
    import json

    assert os.path.exists(cam_info_path), "Cam info file does not exist."
    assert os.path.exists(images_path), "Images path does not exist."
    assert image_extension in ['png', 'jpg', 'jpeg', 'tiff'], "Unkown image file format."


    with open(output_path, "w") as json_cam_info:

        yaml_info = inout.load_info(cam_info_path)
        converted = {}

        image_filenames = util.get_files_at_path_of_extensions(images_path, [image_extension])
        util.sort_list_by_num_in_string_entries(image_filenames)
        index = 0
        for filename in image_filenames:
            info = yaml_info[index]
            converted_single = {'K' : info['cam_K'].flatten().tolist(), 
                                'mode' : info['mode'], 
                                'elev' : info['elev']}
            if 'cam_R_w2c' in info:
                # In this case we also have the rotation of the camera, which is provided for test images
                converted_single['R'] = info['cam_R_w2c'].flatten().tolist()
                converted_single['t'] = info['cam_t_w2c'].flatten().tolist()
            converted[filename] = converted_single
            index += 1

        json.dump(converted, json_cam_info)

if __name__ == '__main__':
    import argparse

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='This script converts camera infos of the T-Less datasets \
        to the one used by flower power neural network.')
    parser.add_argument("--cam_info_path",
                        required=True,
                        help="The path to the camera info.")
    parser.add_argument("--images_path",
                        required=True,
                        help="The path to the images.")
    parser.add_argument("--image_extension",
                        required=True,
                        help="The extension of the images.")
    parser.add_argument("--output_path",
                        required=True,
                        help="The path where to output the result to.")

    args = parser.parse_args()
    convert_tless_cam_info(args.cam_info_path, args.images_path, args.image_extension, args.output_path)