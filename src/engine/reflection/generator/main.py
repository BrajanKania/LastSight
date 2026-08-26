import argparse
from pathlib import Path

from macro_data import HeaderData
from parser import process_header, resolve_include_path

from generator import generate_code


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dirs", nargs="+", required=True)
    parser.add_argument("--output-file", required=True)

    args = parser.parse_args()
    headers_data: list[HeaderData] = []

    for dir_path_str in args.dirs:
        dir_path = Path(dir_path_str)

        if not dir_path.exists():
            continue

        for header_path in dir_path.rglob("*.hpp"):
            code = header_path.read_text(encoding="utf-8")
            components, enums = process_header(code)

            if components or enums:
                include_path = resolve_include_path(header_path.resolve(), dir_path)
                headers_data.append(
                    HeaderData(path=include_path, components=components, enums=enums)
                )

    generated_code = generate_code(headers_data)

    output_path = Path(args.output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(generated_code, encoding="utf-8")


if __name__ == "__main__":
    main()
