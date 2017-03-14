# font generator

from PIL import Image, ImageChops
import sys

def extract_glyph(im, idx):
    rect = im.crop((idx * 6, 0, (idx + 1)* 6, 8))
    glyph = Image.new('1', (8, 8), 1)
    glyph.paste(rect, (2, 0))
    return glyph

def export_glyph(im):
    bytes_ = im.tobytes('raw', '1;I')
    return ', '.join(['0x%02X' % b for b in bytes_])

def main():
    infile, outfile = sys.argv[1:]

    im = Image.open(infile)

    result = 'static const u8 lcd_font[256 * 8] = {'
    glyphs = [export_glyph(extract_glyph(im, i)) for i in range(256)]
    result += ', '.join(glyphs)
    result += '};'

    f = open(outfile, 'w')
    f.write(result)

main()

