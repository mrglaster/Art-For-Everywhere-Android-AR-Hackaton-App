import colortrans
import numpy as np
from PIL import Image
content = np.array(Image.open("image_pseudonormal.png").convert('RGB'))
reference = np.array(Image.open("image.jpg").convert('RGB'))
result = Image.fromarray(colortrans.transfer_reinhard(content, reference))
result.save("result.png")
