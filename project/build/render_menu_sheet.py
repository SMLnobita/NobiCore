"""Compose the three rendered tabs into a single menu.png sheet."""
from pathlib import Path

from PIL import Image

BUILD = Path(__file__).resolve().parent
OUT = BUILD.parent / 'menu.png'
PAD = 16
GAP = 16
BACKGROUND = (13, 15, 20)

tabs = [Image.open(BUILD / f'menu-tab-{i}.png').convert('RGB') for i in range(3)]
width = PAD * 2 + sum(im.width for im in tabs) + GAP * (len(tabs) - 1)
height = PAD * 2 + max(im.height for im in tabs)
sheet = Image.new('RGB', (width, height), BACKGROUND)

x = PAD
for image in tabs:
    sheet.paste(image, (x, PAD))
    x += image.width + GAP

sheet.save(OUT)
print(OUT, sheet.size)
