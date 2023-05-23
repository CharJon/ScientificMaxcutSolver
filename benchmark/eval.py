"""
    eval.py <output_dir> <optional: --check_file> <optional: --plots>

    Output of this script includes a CSV file of the results for all runs, a pivot table of the runs and for each run
    plots of the history.json and the scip log parsed as json file.
    If for a run the optimal values does not match the value reported in the check file, 'ERROR' is written into the CSV
    file. If a check file is given, but not all instances are found in the check file, validation is skipped for these.
"""
import pathlib
import sys
import json
import simexpal
import pandas as pd
from pathlib import Path
from os import mkdir

import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt

import argparse

from scripts import scip_stats_parser


def parse(run, output_file):
    d = json.load(output_file)
    return {
        'experiment': run.experiment.name,
        'revision': run.experiment.revision.name,
        'instance': run.instance.shortname,
        'parameter': run.experiment.variation[0].name,
        'seed': run.repetition,
        **d
    }


def parse_scip_log(file):
    data = {}
    all_lines = file.readlines()
    all_lines = scip_stats_parser.rename_lpiters(all_lines)
    blocks = scip_stats_parser.to_blocks(all_lines)
    for b in blocks:
        current_block = all_lines[b[0]:b[1]]
        data.update(scip_stats_parser.parse_block(current_block))

    return data


def history_plots(file, output_dir):
    history = json.load(file)

    # primal dual bound plot
    db = np.array(history['dualbound'][1:])
    pb = np.array(history['primalbound'][1:])
    cpu_time = np.array(history['cpu_solving_time'][1:])

    sns.lineplot(x=cpu_time, y=db, label="Dual")
    sns.lineplot(x=cpu_time, y=pb, label="Primal")

    plt.title("Primal and Dual Bound")
    plt.xlabel("Solving time")
    plt.savefig(output_dir.joinpath("primal_dual_bound.svg"))
    plt.clf()

    # number of bab nodes
    xrange = np.arange(0, len(np.array(history["bab_nodes"])))
    sns.lineplot(x=xrange, y=np.array(history["bab_nodes"]))
    plt.title("Number of BaB Nodes")
    plt.xlabel("Iterations")
    plt.ylabel("BaB Nodes")
    plt.savefig(output_dir.joinpath("bab_nodes.svg"))
    plt.clf()

    # number of LP iterations
    sns.lineplot(x=np.array(history["cpu_solving_time"]), y=np.array(history["lp_iterations"]))
    plt.xlabel("Solving time")
    plt.ylabel("LP iterations")
    plt.title("Number of LP iterations")
    plt.savefig(output_dir.joinpath("lp_iterations.svg"))
    plt.clf()

    # number of events
    sns.histplot(x=np.array(history["cpu_solving_time"]))
    plt.xlabel("Solving time")
    plt.ylabel("Event Count")
    plt.title("Number of Events")
    plt.savefig(output_dir.joinpath("event_count.svg"))
    plt.clf()


def create_parse():
    parser = argparse.ArgumentParser(description='Collect and parse result of the SIMEX benchmark-runs.',
                                     prog='Eval for SIMEX',
                                     epilog='This script collects the results of the SIMEX benchmark-runs and parses'
                                            'them into a CSV file. It also creates plots of the history.json and the '
                                            'scip log parsed as json file. If a check file is given, it will'
                                            ' cross-check the optimal values in the check file with the reported'
                                            ' values for the'
                                            ' jobs. Check file should be a CSV file with columns \'instances\' and'
                                            ' \'optimum\'. If for a run the optimal values does not match the value'
                                            ' reported in the check file, \'ERROR\' is written into the CSV file.'
                                            ' If a check file is given, but not all instances are found in the check'
                                            ' file, validation is skipped for these.')
    parser.add_argument('output_dir', type=pathlib.Path, help='Output directory for all result files')
    parser.add_argument('--check_file', type=pathlib.Path,
                        help='Check file containing optimal values for the instances')
    parser.add_argument('--plots', action='store_true', help='Create plots for each run')
    return parser


if __name__ == "__main__":
    args = create_parse().parse_args()

    output_dir = args.output_dir
    check_file = args.check_file

    if args.check_file:
        check_data = pd.read_csv(check_file, index_col="instance")
        print("Check file found. Validating results.")
    else:
        check_data = None
        print("No check file specified. Skipping validation.")

    if not output_dir.is_dir():
        mkdir(output_dir)

    cfg = simexpal.config_for_dir()
    results = []

    for successful_run in cfg.collect_successful_results():
        # get unique name for each run
        exp = successful_run.experiment.name
        rev = successful_run.experiment.revision.name
        inst = successful_run.instance.shortname
        param = successful_run.experiment.variation[0].name
        seed = successful_run.repetition

        run_id = f"{exp}_{rev}_{inst}_{param}_{seed}"

        run_output_dir = output_dir.joinpath(run_id)
        if not run_output_dir.is_dir():
            mkdir(run_output_dir)

        scip_data = None
        with open(successful_run.output_file_path(ext="scip_stats.txt")) as f:
            scip_data = parse_scip_log(f)

            # save scip log data to json
            with open(run_output_dir.joinpath("scip_log.json"), "w") as f:
                json.dump(scip_data, f, indent=4)

        with open(successful_run.output_file_path(ext="stats.json")) as f:
            stats_data = parse(successful_run, f)

            if scip_data is not None:
                stats_data["root_initial_dual_bound"] = scip_data["Root Node"]["First LP value"]
                stats_data["root_fina_dual_bound"] = scip_data["Root Node"]["Final Dual Bound"]

            if check_data is not None:
                if inst in check_data.index:
                    if stats_data["best_solution_value"] != check_data.at[inst, "optimum"]:
                        stats_data["best_solution_value"] = "ERROR"
                        print(f"WARNING: {run_id} has wrong optimum value!")
                else:
                    print(f"INFO: {inst} not found in check file, skipping validation.")

            results.append(stats_data)

        if args.plots:
            with open(successful_run.output_file_path(ext="history.json")) as f:
                history_plots(f, run_output_dir)

    if len(results) > 0:
        df = pd.DataFrame(results)
        df.to_csv(output_dir.joinpath("results.csv"), index=False)

        # create pivot table
        index = ['instance', 'seed']
        columns = ['experiment', 'revision', 'parameter']
        values = ['cpu_solving_time', 'bab_nodes']

        df_pivot = df.pivot_table(index=index, columns=columns, values=values)
        df_pivot.reset_index(inplace=True)
        df_pivot.set_index(index, inplace=True)
        df_pivot.to_csv(output_dir.joinpath("pivot.csv"))

    else:
        print("No successful runs found.", file=sys.stderr)
        exit(1)
