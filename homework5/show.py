import os
from PIL import Image, ImageFile
import matplotlib.pyplot as plt

if __name__ == "__main__":
    ImageFile.LOAD_TRUNCATED_IMAGES = True
    if not os.path.exists("results"):
        print("there is no results to show\n")
        exit(1)
    origin = Image.open(os.path.join("origin.bmp"))
    origin_grey = Image.open(os.path.join("results", "origin_grey.bmp"))
    
    mean_fliter = Image.open(os.path.join("results", "mean_fliter.bmp"))
    mean_fliter_grey = Image.open(os.path.join("results", "mean_fliter_grey.bmp"))
    laplacian_fliter = Image.open(os.path.join("results", "laplacian_fliter.bmp"))
    laplacian_fliter_grey = Image.open(os.path.join("results", "laplacian_fliter_grey.bmp"))

    # plt.ioff()
    plt.figure("Image show", figsize=(16, 8))
    plt.suptitle("all Images")
    plt.subplot(3, 2, 1), plt.title("origin"), plt.axis("off"), plt.imshow(origin)
    plt.subplot(3, 2, 2), plt.title("origin_grey"), plt.axis("off"), plt.imshow(origin_grey)
    plt.subplot(3, 2, 3), plt.title("mean_fliter"), plt.axis("off"), plt.imshow(mean_fliter)
    plt.subplot(3, 2, 4), plt.title("mean_fliter_grey"), plt.axis("off"), plt.imshow(mean_fliter_grey)
    plt.subplot(3, 2, 5), plt.title("laplacian_fliter"), plt.axis("off"), plt.imshow(laplacian_fliter)
    plt.subplot(3, 2, 6), plt.title("laplacian_fliter_grey"), plt.axis("off"), plt.imshow(laplacian_fliter_grey)
    plt.show()
    