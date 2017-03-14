from PIL import Image

CL_BACK1 = (0, 180, 180)
CL_BACK0 = (0, 200, 200)


out = Image.new('RGB', (256 * 6, 8), 0)
for i in range(256):
    box = Image.new('RGB', (6, 8), CL_BACK0 if i % 2 else CL_BACK1)
    out.paste(box, (i * 6, 0))

out.save('underlay.png')

