from pathlib import Path
from argparse import ArgumentParser


def main():
    parser = ArgumentParser()
    parser.add_argument('-i', '--input', type=str, required=True)
    args = parser.parse_args()

    path = Path(args.input)
    exts = ['.jpg', '.jpeg']
    if path.is_dir():
        files = [p.resolve() for p in path.glob('**/*') if p.suffix in exts]
    if path.is_file() and path.suffix in exts:
        files = [path]
    for f in files:
        name = f.stem
        size = f.stat().st_size
        with open(f, 'rb') as img, open(f'{name}.c', 'w') as out:
            c_array = f'const unsigned char {name}[{size}] = {{\n'
            out.write(c_array)
            while True:
                data = img.read(12)
                if not data:
                    break
                [out.write(f'0x{d:02x}, ') for d in data]
                out.write('\n')
            out.write('};\n')


if __name__ == '__main__':
    main()