import argparse
import os
from pathlib import Path
import sys


def main():
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('--job-config', type=Path, required=True)
    args, remaining = parser.parse_known_args()
    os.environ['EUROPA_JOB_CONFIG'] = str(args.job_config.absolute())
    from job_scope import load
    load()
    import write_ledger
    sys.argv = [sys.argv[0], *remaining]
    write_ledger.main()


if __name__ == '__main__':
    main()
