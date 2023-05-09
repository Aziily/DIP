import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    origin = Image.open(os.path.join("origin.bmp"))
    
    translation = Image.open(os.path.join("results", "translation.bmp"))
    rotation = Image.open(os.path.join("results", "rotation.bmp"))
    scale = Image.open(os.path.join("results", "scale.bmp"))
    shear = Image.open(os.path.join("results", "shear.bmp"))
    mirror = Image.open(os.path.join("results", "mirror.bmp"))

    # plt.ioff()
    plt.figure("Image show", figsize=(16, 8))
    plt.suptitle("all Images")
    plt.subplot(2, 3, 1), plt.title("origin"), plt.axis("off"), plt.imshow(origin)
    plt.subplot(2, 3, 2), plt.title("translation"), plt.axis("off"), plt.imshow(translation)
    plt.subplot(2, 3, 3), plt.title("rotation"), plt.axis("off"), plt.imshow(rotation)
    plt.subplot(2, 3, 4), plt.title("scale"), plt.axis("off"), plt.imshow(scale)
    plt.subplot(2, 3, 5), plt.title("shear"), plt.axis("off"), plt.imshow(shear)
    plt.subplot(2, 3, 6), plt.title("mirror"), plt.axis("off"), plt.imshow(mirror)
    plt.show()
    