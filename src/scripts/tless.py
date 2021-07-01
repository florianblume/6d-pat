import os
import yaml
import time
import datetime

image_extensions = ['jpg', 'png', 'bmp']
object_models_extensions = ['.obj', '.ply', '.3ds', '.fbx']

def get_files_at_path_of_extensions(path, extensions):
    return [fn for fn in os.listdir(path) if any(fn.endswith(ext) for ext in extensions)]

def sort_list_by_num_in_string_entries(list_of_strings):
    list_of_strings.sort()

def load_images(images_path, segmentation_images_path):
    cam_info_file_path = os.path.abspath(os.path.join(images_path, '..', 'info.yml'))
    
    with open(cam_info_file_path, 'r') as cam_info_file:
        cam_info = yaml.safe_load(cam_info_file)

        converted = []

        image_filenames = get_files_at_path_of_extensions(images_path, image_extensions)
        sort_list_by_num_in_string_entries(image_filenames)

        if len(cam_info) != len(image_filenames):
            return 'Error: info.yml does not contain as many entries as images in the folder.'

        # If segmentation images path is not None is exists, because the user can only select it
        # via a folder selection dialog in the main program
        if segmentation_images_path is not None:
            segmentation_image_filenames = get_files_at_path_of_extensions(segmentation_images_path,
                                                                           image_extensions)
            sort_list_by_num_in_string_entries(segmentation_image_filenames)

        for index, filename in enumerate(image_filenames):
            info = cam_info[index]
            converted_single = {'img_id' : index,
                                'img_path' : filename,
                                'base_path' : images_path,
                                'K' : info['cam_K'], 
                                'mode' : info['mode'], 
                                'elev' : info['elev']}
            if segmentation_images_path is not None:
                segmentation_filename = os.path.basename(
                    segmentation_image_filenames[index])
                converted_single['segmentation_image_path'] = segmentation_filename
            if 'cam_R_w2c' in info:
                # In this case we also have the rotation of the camera, which is provided for test images
                converted_single['R'] = info['cam_R_w2c']
                converted_single['t'] = info['cam_t_w2c']
            converted.append(converted_single)
        return converted

    return 'Error: info.yml could not be read.'

def load_object_models(object_models_path):
    converted = []

    object_model_filenames = get_files_at_path_of_extensions(object_models_path, object_models_extensions)
    sort_list_by_num_in_string_entries(object_model_filenames)

    for index, filename in enumerate(object_model_filenames):
        converted_single = {'obj_id' : index,
                            'obj_model_path' : filename,
                            'base_path' : object_models_path}
        converted.append(converted_single)
    return converted

def load_poses(path):
    gt_file_path = os.path.abspath(path)

    converted = None
    needs_update = False
    
    with open(gt_file_path, 'r') as gt_file:
        gt = yaml.safe_load(gt_file)
        
        converted = []

        for img_index, entry in gt.items():
            entries_for_image = []
            for gt_entry in entry:
                converted_single = {'img_id' : img_index,
                                    # obj_id starts counting at 1
                                    'obj_id' : int(gt_entry['obj_id']) -1,
                                    'R' : gt_entry['cam_R_m2c'],
                                    't' : gt_entry['cam_t_m2c']}
                if 'pose_id' in gt_entry:
                    converted_single['pose_id'] = gt_entry['pose_id']
                else:
                    needs_update = True
                    ts = time.time()
                    timestamp = datetime.datetime.fromtimestamp(ts).strftime('%d.%m.%y_%H:%M:%S')
                    gt_entry['pose_id'] = str(img_index) + '_' + str(int(gt_entry['obj_id']) -1) + '_' + timestamp
                entries_for_image.append(converted_single)
            converted.append(entries_for_image)

    if converted is not None:
        if needs_update:
            # Try to write back the updated IDs
            with open(gt_file_path, 'w') as gt_file:
                yaml.dump(gt, gt_file)
        return converted

    return 'Error: info.yml could not be read.'

def persist_pose(path, pose_id, image_id, image_path, obj_id, obj_path, rotation, translation, remove):
    # We passed obj_id - 1 to the code -> reverse here
    obj_id = int(obj_id) + 1
    print(path, pose_id, image_id, image_path, obj_id, obj_path, rotation, translation, remove)
    return True