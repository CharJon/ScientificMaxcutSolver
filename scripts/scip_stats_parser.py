import sys
import json
from pathlib import Path


def to_blocks(all_lines):
    blocks = []
    num_lines = len(all_lines)

    cur_start = 0
    for i in range(1, num_lines):
        if all_lines[i][0] != " ":
            blocks.append((cur_start, i))
            cur_start = i
    blocks.append((cur_start, num_lines))
    return blocks


def to_columns(header_line: str):
    pos = header_line.find(":")
    header = header_line[:pos].strip()

    columns = []
    indices = []
    start = pos + 1
    while header_line[start] == " ":
        start += 1

    cur_start = pos + 1
    i = start
    while i < len(header_line):
        if header_line[i] == " ":
            columns.append(header_line[cur_start:i].strip())
            indices.append((cur_start, i))
            cur_start = i
            while header_line[i] == " ":
                i += 1
        i += 1
    columns.append(header_line[cur_start:].strip())
    indices.append((cur_start, len(header_line)))

    return header, columns, indices


def parse_table_row(line, columns, indices):
    name = line.split(":")[0].strip()
    inner_dict = dict()
    for cur_column, cur_index in zip(columns, indices):
        start, stop = cur_index
        inner_dict[cur_column] = line[start:stop].strip()
    last_stop = indices[-1][1]
    if len(line) > last_stop:
        inner_dict["extra"] = line[last_stop:].strip()
    return {name: inner_dict}


def parse_single_info(line):
    line_data = line.split(":")
    key = line_data[0].strip()
    value = ":".join(line_data[1:]).strip()
    return {key: value}


def parse_multi_info(lines):
    line_data = lines[0].split(":")
    outer_key = line_data[0].strip()

    inner_dict = dict()
    for i in range(1, len(lines)):
        inner_dict.update(parse_single_info(lines[i]))
    return {outer_key: inner_dict}


def parse_table(lines):
    assert len(lines) > 1
    header_line = lines[0]

    extra = None
    if header_line.find(":") >= header_line.find("("):
        header, columns, indices = to_columns(header_line)
    else:
        # TODO: Maybe clean up
        first_colon = header_line.find(":")
        first_bracket_after_colon = header_line[first_colon:].find("(")
        first_bracket_after_colon += first_colon
        header, columns, indices = to_columns(header_line[0:first_bracket_after_colon].rstrip())
        extra = header_line[first_bracket_after_colon:].strip()

    inner_dict = dict()
    for i in range(1, len(lines)):
        inner_dict.update(parse_table_row(lines[i], columns, indices))

    if not extra:
        return {header: inner_dict}
    else:
        inner_dict["extra"] = extra
        return {header: inner_dict}


def parse_block(lines):
    if len(lines) == 1:
        return parse_single_info(lines[0])
    elif len(lines) == 0:
        assert False
    else:
        split_first = lines[0].split(":")
        if split_first[1].strip() == "":
            return parse_multi_info(lines)
        elif split_first[0].startswith("Total"):
            # Maybe handle this special case?
            return parse_multi_info(lines)
        else:
            return parse_table(lines)


def rename_lpiters(lines):
    for l in range(len(lines)):
        pos = lines[l].find(":")
        lines[l] = lines[l][:pos] + lines[l][pos:].replace(" LP Iters", "  LpIters")

    return lines


def test():
    # Hardcode testcase here
    # special cases like -, "", etc.
    data = {}

    with open("../test/data/scip_statistics.txt", "r") as f:
        all_lines = f.readlines()
        all_lines = rename_lpiters(all_lines)
        blocks = to_blocks(all_lines)
        for b in blocks:
            current_block = all_lines[b[0]:b[1]]
            data.update(parse_block(current_block))

    assert data["Total Time"]["solving"] == "347.11"
    assert data["Conflict Analysis"]["extra"] == "(pool size: [--,--])"
    assert data["Estimations"]["wbe"]["smooth"] == "-"
    assert data["LP"]["lex dual LP"]["Time-0-It"] == ""


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("No arguments given, doing test run")
        test()
        exit(0)

    log_file = Path(sys.argv[1])
    output_file = Path(sys.argv[2])

    data = {}

    with open(log_file, "r") as f:
        all_lines = f.readlines()
        all_lines = rename_lpiters(all_lines)
        blocks = to_blocks(all_lines)
        for b in blocks:
            current_block = all_lines[b[0]:b[1]]
            data.update(parse_block(current_block))

    with open(output_file, "w") as f:
        json.dump(data, f, indent=4)
