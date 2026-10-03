import argparse
import json

from verify_capture import annotation_transition_check as check_session

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dir', required=True)
    args = parser.parse_args()
    errors, detail = check_session(args.dir)
    print(json.dumps(dict(errors=errors, **detail), indent=2))
    return 1 if errors else 0


if __name__ == '__main__':
    raise SystemExit(main())
