import os

from collections import defaultdict
from sys import argv

run_test_suites = argv[1].lower() == "true"
keep_output_files = argv[2].lower() == "true"

# Switch to top level directory
main_path = os.getcwd()
os.chdir(os.path.dirname(__file__) + "/Team05/Tests05")
f = open("results.txt", "w")

dic = defaultdict(list)

def getFailedTC(text, dir_path, source, queries):
    text = text.splitlines()
    count = 0
    failed_tcs = []
    for i, x in enumerate(text):
        if x == "<failed>":
            y = text[i-4]
            pos = y.find("</id>") - 1
            res = ""
            while y[pos].isdigit():
                res += y[pos]
                pos -= 1

            res = res[::-1]
            failed_tcs.append(res)
            count += 1

    res = "All test cases passed" if len(failed_tcs) == 0 else ("Failed TCs: " + ",".join(failed_tcs))

    failed_count = "Number of Failed TC: " + str(count)
    f.write("Directory path: " + dir_path + "\n")
    f.write("Source file: " + source + "\n")
    f.write("Queries file: " + queries + "\n")
    f.write(res + "\n")
    f.write(failed_count + "\n\n")

# Compile all source and matching query files
for dirpath, dirnames, filenames in os.walk(os.getcwd()):
    filenames.sort()
    for file in filenames:
        if file.startswith("iter") and file.endswith("_source.txt"):
            print(file)
            prefix = file[:-len("_source.txt")]
            queries_match_file = prefix + "_queries.txt"
            if queries_match_file not in filenames:
                print("Unable to find matching queries for:", file)
                continue

            shortened_path = dirpath
            if shortened_path.startswith(main_path):
                shortened_path = shortened_path[len(main_path):]

            dic[shortened_path].append((file, queries_match_file, prefix))

os.chdir(os.path.dirname(__file__))

output_files = []

if run_test_suites:
    for k in dic:
        for source, queries, prefix in dic[k]:
            dirpath = k + "/"
            source_input, queries_input, output_input = dirpath + source, dirpath + queries, dirpath + prefix + "_out.txt"
            source_flag, queries_flag, output_flag = "source=" + source_input, "query=" + queries_input, "out=" + output_input
            print(source, queries, prefix)
            os.system(" ".join(["make", "autotester", source_flag, queries_flag, output_flag]))
            f2 = open(output_input[1:], "r")
            text = f2.read()
            f2.close()
            getFailedTC(text, k, source_input, queries_input)
            output_files.append(output_input)

if not keep_output_files:
    for file_input in output_files:
        if os.path.exists("." + file_input):
            os.remove("." + file_input)

f.close()

