#!/usr/bin/env python

"""
for image in inputs:
    place image at the center of set sized rect, ie 120x120
"""

import glob
import pathlib
from PIL import Image


OUTPUT_SIZE = (132, 132)
WHITE = (255, 255, 255, 0)

for img_path in glob.glob("spritesheet_in/*png"):
    img_path = pathlib.Path(img_path)
    img = Image.open(str(img_path), "r")
    img_w, img_h = img.size

    background = Image.new('RGBA', OUTPUT_SIZE, WHITE)
    bg_w, bg_h = background.size

    offset = ((bg_w - img_w) / 2, bg_w-img_h)
    background.paste(img, offset)

    out_path = "spritesheet_out\\"+img_path.name
    background.save(out_path)

print "Done"
