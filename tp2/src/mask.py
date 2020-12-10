import numpy as np

# https://medium.com/spinor/a-straightforward-introduction-to-image-blurring-smoothing-using-python-f8870cf1096


def gaussian_kernel(size, sigma=1):
    """
    Sigma values: For simplicity, you can set the 2 sigma values to be the same. 
    If they are small (< 10), the filter will not have much effect, 
    whereas if they are large (> 150), they will have a very strong effect, 
    making the image look “cartoonish”.
    """
    size = int(size) // 2
    x, y = np.mgrid[-size:size+1, -size:size+1]
    normal = 1 / (2.0 * np.pi * sigma**2)
    g = np.exp(-((x**2 + y**2) / (2.0*sigma**2))) * normal
    return g


mask = gaussian_kernel(3, 1)

print(mask)