""" Utility functions for stuff necessary for the network.
"""

import numpy as np
import os
import re
import math

def create_index_array_from_step_sizes(step_y, end_y, max_y, step_x, end_x, max_x):

    steps_y = np.arange(step_y / 2, end_y, step_y)
    steps_x = np.arange(step_x / 2, end_x, step_x)

    steps_y = steps_y.astype(np.int32)
    steps_x = steps_x.astype(np.int32)

    # Numpy sometimes INCLUDES the limit in np.arange for floating point values, i.e.
    # we have to check whether we exceed the image's limits
    steps_y_last = len(steps_y) - 1
    steps_y[steps_y_last] = min(steps_y[steps_y_last], max_y)
    steps_x_last = len(steps_x) - 1
    steps_x[steps_x_last] = min(steps_x[steps_x_last], max_x)

    return steps_y, steps_x


def shrink_image_with_step_size(image, target_shape):
    """ With this function an image can be shrinked to a certain target size
    with out interpolation. Instead the pixels are extracted at regular intervals
    determined by the ratio of source and target shape.
    """
    step_y = image.shape[0] / float(target_shape[0])
    step_x = image.shape[1] / float(target_shape[1])

    steps_y, steps_x = create_index_array_from_step_sizes(step_y, 
                                                          image.shape[0],
                                                          image.shape[0] - 1,
                                                          step_x, 
                                                          image.shape[1],
                                                          image.shape[1] - 1)

    num_steps_y = len(steps_y)

    # Repeat items to be able to pair every entry in steps_y with every entry in steps_x
    steps_y = np.repeat(steps_y, len(steps_x))
    steps_x = np.tile(steps_x, num_steps_y)

    shrinked_image = np.zeros([target_shape[0], target_shape[1], 3], dtype=image.dtype)
    shrinked_image = image[steps_y, steps_x]
    shrinked_image = shrinked_image.reshape(target_shape)

    return shrinked_image

def pair_object_coords_with_index(image, original_im_size, step_y, step_x):
    """ This function pairs the object coords in a detection image with their
    actual index in the original image. The image shrinks during inference
    thus the index of the final detection does not correspond with the index
    in the source image. 
    """
    steps_y, steps_x = create_index_array_from_step_sizes(step_y, 
                                                          original_im_size[0],
                                                          original_im_size[0] - 1,
                                                          step_x,  
                                                          original_im_size[1],
                                                          original_im_size[1] - 1)

    object_points = []
    image_points = []

    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            obj_coord = image[i][j]
            if np.any(obj_coord != 0):
                # If all coords are 0, then we are outside of the segmentation mask
                object_points.append(obj_coord)
                image_points.append([steps_x[j], steps_y[i]])

    object_points = np.array(object_points).astype(np.float32)
    image_points = np.array(image_points).astype(np.float32)
    return image_points, object_points

def crop_image_on_segmentation_color(image, segmentation_mask, color, return_frame=False):
    """ This function returns the rectangle that results when cropping the mask
    """
    indices = np.where(segmentation_mask == color)
    y_indices = indices[0]
    if y_indices.shape[0] == 0:
        return np.array([])
    y_start = np.min(y_indices)
    y_end = np.max(y_indices)
    x_indices = indices[1]
    if x_indices.shape[0] == 0:
        return np.array([])
    x_start = np.min(x_indices)
    x_end = np.max(x_indices)
    cropped_image = image[y_start : y_end + 1, x_start : x_end + 1]
    if return_frame:
        return cropped_image, (y_start, x_start, y_end + 1, x_end + 1)
    else:
        return cropped_image

def get_files_at_path_of_extensions(path, extensions):
    import os
    return [fn for fn in os.listdir(path) if any(fn.endswith(ext) for ext in extensions)]

def sort_list_by_num_in_string_entries(list_of_strings):
    list_of_strings.sort()

def is_number(n, _type):
    try:
        _type(n)
    except ValueError:
        return False
    return True

def convert_printed_to_numpy_array(printed_array, _type):
    temp = printed_array.split("[")
    # Flatten array
    temp = [s.split("]") for s in temp]
    temp = [s for x in temp for s in x]
    temp = [s.split(" ") for s in temp]
    temp = [s for x in temp for s in x]
    temp = [_type(n) for n in temp if is_number(n, _type)]
    return np.array(temp)