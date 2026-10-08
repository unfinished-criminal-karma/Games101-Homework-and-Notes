"""PPM(P6) -> PNG 转换（只依赖 Python 标准库 zlib/struct）。
用法: python ppm2png.py <in.ppm> [out.png]
"""
import re
import struct
import sys
import zlib


def read_ppm(path):
    data = open(path, "rb").read()
    m = re.match(rb"P6\s+(?:#.*\s+)*(\d+)\s+(\d+)\s+(\d+)\s", data, re.S)
    if not m:
        raise SystemExit(f"不是 P6 PPM: {path}")
    w, h, mx = int(m.group(1)), int(m.group(2)), int(m.group(3))
    body = data[m.end():]
    if mx > 255:
        raise SystemExit(f"暂不支持 maxval>255 (maxval={mx})")
    if len(body) < w * h * 3:
        raise SystemExit(f"像素数据不足: {len(body)} < {w*h*3}")
    return w, h, body[:w * h * 3]


def write_png(path, w, h, rgb):
    raw = bytearray()
    stride = w * 3
    for y in range(h):
        raw.append(0)  # filter type 0
        raw += rgb[y * stride:(y + 1) * stride]

    def chunk(tag, payload):
        return (struct.pack(">I", len(payload)) + tag + payload
                + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))

    ihdr = struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)
    png = (b"\x89PNG\r\n\x1a\n"
           + chunk(b"IHDR", ihdr)
           + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
           + chunk(b"IEND", b""))
    with open(path, "wb") as f:
        f.write(png)


def main():
    src = sys.argv[1]
    dst = sys.argv[2] if len(sys.argv) > 2 else re.sub(r"\.ppm$", "", src, flags=re.I) + ".png"
    w, h, rgb = read_ppm(src)
    write_png(dst, w, h, rgb)
    print(f"{src} ({w}x{h}) -> {dst}")


if __name__ == "__main__":
    main()
