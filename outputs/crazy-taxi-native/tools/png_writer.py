"""Small standard-library PNG writer for decoded ROM pixels and render readbacks."""
import binascii
import struct
import zlib
from pathlib import Path


def write_png(path, width, height, rgba):
    if len(rgba) != width*height*4:
        raise ValueError('Pixel buffer size mismatch')
    def chunk(kind, data):
        return struct.pack('>I',len(data))+kind+data+struct.pack('>I',binascii.crc32(kind+data)&0xffffffff)
    rows=b''.join(b'\0'+rgba[y*width*4:(y+1)*width*4] for y in range(height))
    Path(path).write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',width,height,8,6,0,0,0))
                          +chunk(b'IDAT',zlib.compress(rows))+chunk(b'IEND',b''))


def bmp_to_png(source, destination):
    data=Path(source).read_bytes()
    if data[:2] != b'BM':
        raise ValueError('Not a BMP')
    offset=struct.unpack_from('<I',data,10)[0]
    width,height=struct.unpack_from('<ii',data,18)
    bits=struct.unpack_from('<H',data,28)[0]
    compression=struct.unpack_from('<I',data,30)[0]
    if width<=0 or height<=0 or bits!=32 or compression!=0:
        raise ValueError('Expected bottom-up uncompressed 32-bit BMP')
    pixels=bytearray(data[offset:offset+width*height*4])
    pixels[0::4],pixels[2::4]=pixels[2::4],pixels[0::4]
    rows=b''.join(pixels[y*width*4:(y+1)*width*4] for y in reversed(range(height)))
    write_png(destination,width,height,rows)


if __name__ == '__main__':
    import sys
    bmp_to_png(sys.argv[1],sys.argv[2])
