# read in the experiments.yml with python standard yml
import subprocess

import yaml
import pathlib
import argparse
import os


def create_parse():
    parser = argparse.ArgumentParser()
    parser.add_argument("binary", type=pathlib.Path, help="Path to the binary")
    parser.add_argument("--instset", type=str, nargs="+", help="The instance sets to use", default="all")
    parser.add_argument("--num-processes", type=int, default=2)

    return parser


def get_instances(inst_sets):
    with open("experiments.yml", "r") as f:
        data = yaml.load(f, Loader=yaml.FullLoader)
    print(data)

    inst_dir = pathlib.Path(data["instdir"])
    instances = []

    for s in data["instances"]:
        if any(el in s["set"] for el in inst_sets):
            instances += s["items"]
    return inst_dir, instances


def get_command(bin_path, inst_path):
    instance_str_path = str(inst_path.name)
    return [str(bin_path), str(inst_path), '--paramfile=../params/default.set',
            f'--solution={instance_str_path}.sol.json',
            f'--statsfile={instance_str_path}.stats.json']


if __name__ == "__main__":
    # create argparse to parse one optional argument named instset
    parser = create_parse()
    args = parser.parse_args()
    inst_sets = args.instset

    inst_dir, instances = get_instances(inst_sets)

    print(inst_dir)
    print(instances)

    processes = set()

    num_processes = args.num_processes
    print(f"Using {num_processes} processes.")
    for f in instances:
        pf = pathlib.Path(inst_dir).joinpath(f"{f}.mc")
        processes.add(subprocess.Popen(get_command(args.binary, pf), stdout=subprocess.DEVNULL))
        if len(processes) >= num_processes:
            # os.wait blocks the main thread until at least one child process is finished
            os.wait()
            finished_processes = [p for p in processes if p.poll() is not None]
            processes.difference_update(finished_processes)
        # Check if all the child processes were closed
    for p in processes:
        if p.poll() is None:
            p.wait()
