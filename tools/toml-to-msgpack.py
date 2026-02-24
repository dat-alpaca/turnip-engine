import sys
import argparse
import msgpack
import tomllib


def toml_to_msgpack(filepath: str, output_filepath: str):
    with open(filepath, 'rb') as file:
        data = tomllib.load(file)

    with open(output_filepath, 'wb') as file:
        msgpack.pack(data, file, use_bin_type = True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("output")

    args = parser.parse_args()

    try:
        toml_to_msgpack(args.input, args.output)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)

if __name__ == "__main__":
    main()
