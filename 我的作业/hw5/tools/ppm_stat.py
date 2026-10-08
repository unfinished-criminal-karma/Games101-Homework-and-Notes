"""临时工具：统计 PPM(P6) 图像的基本信息，用于核对渲染结果。
用法: python ppm_stat.py <file.ppm> [topN]
"""
import collections
import re
import sys


def main():
    path = sys.argv[1]
    topn = int(sys.argv[2]) if len(sys.argv) > 2 else 5
    data = open(path, "rb").read()
    m = re.match(rb"P6\s+(\d+)\s+(\d+)\s+(\d+)\s", data)
    if not m:
        print("不是 P6 PPM")
        return
    w, h, mx = int(m.group(1)), int(m.group(2)), int(m.group(3))
    body = data[m.end():]
    n = len(body) // 3
    counter = collections.Counter(tuple(body[i:i + 3]) for i in range(0, n * 3, 3))
    print(f"size={w}x{h} maxval={mx} pixels={n} (expect {w*h}) bytes={len(data)}")
    print(f"distinct_colors={len(counter)}")
    for color, cnt in counter.most_common(topn):
        print(f"  {color}  {cnt}  ({cnt / n * 100:.2f}%)")
    # 包围盒：与出现最多的颜色（背景）不同的像素
    bg = counter.most_common(1)[0][0]
    xs, ys, cnt = [], [], 0
    for i in range(n):
        p = tuple(body[i * 3:i * 3 + 3])
        if p != bg:
            cnt += 1
            xs.append(i % w)
            ys.append(i // w)
    if cnt:
        print(f"non-bg pixels={cnt} bbox=x[{min(xs)},{max(xs)}] y[{min(ys)},{max(ys)}]")
    else:
        print("non-bg pixels=0（整幅都是背景色）")


if __name__ == "__main__":
    main()
