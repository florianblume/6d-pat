def convert_tless_gt(gt_path, images_path, image_extension, object_models_path, output_path):
    import os
    import tless_inout as inout
    import util
    import json
    from collections import OrderedDict

    assert os.path.exists(gt_path), "Ground truth file does not exist."
    assert os.path.exists(images_path), "Images path does not exist."
    assert os.path.exists(object_models_path), "Object models path does not exist."
    assert image_extension in ['png', 'jpg', 'jpeg', 'tiff'], "Unkown image file format."


    with open(output_path, "w") as json_gt:

        yml_gt = inout.load_gt(gt_path)
        converted = OrderedDict()

        image_filenames = util.get_files_at_path_of_extensions(images_path, [image_extension])
        util.sort_list_by_num_in_string_entries(image_filenames)

        object_model_filenames = util.get_files_at_path_of_extensions(object_models_path, ['ply', 'obj'])
        util.sort_list_by_num_in_string_entries(object_model_filenames)
        index = 0
        for filename in image_filenames:
            converted[filename] = []
            for gt_entry in range(len(yml_gt[index])):
                gt = yml_gt[index][gt_entry]
                obj_id = gt['obj_id']
                # IDs of object models in T-Less are 1 based
                obj_id -= 1
                obj_filename = object_model_filenames[obj_id]
                converted[filename].append({'R' : gt['cam_R_m2c'].flatten().tolist(), 't' : gt['cam_t_m2c'].flatten().tolist(), 
                                            'bb' : gt['obj_bb'], 'obj' : obj_filename})
            index += 1

        json.dump(converted, json_gt)

if __name__ == '__main__':
    import argparse

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='This script converts ground-truth poses file of the T-Less datasets \
        to the one used by flower power neural network.')
    parser.add_argument("--gt_path",
                        required=True,
                        help="The path to the ground truth file.")
    parser.add_argument("--images_path",
                        required=True,
                        help="The path to the images.")
    parser.add_argument("--image_extension",
                        required=True,
                        help="The extension of the images.")
    parser.add_argument("--object_models_path",
                        required=True,
                        help="The path to the object models.")
    parser.add_argument("--output_path",
                        required=True,
                        help="The path where to output the result to.")

    args = parser.parse_args()
    convert_tless_gt(args.gt_path, args.images_path, args.image_extension, args.object_models_path, args.output_path)