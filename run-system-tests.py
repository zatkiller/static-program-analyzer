import os

from collections import defaultdict
from sys import argv

run_test_suites = argv[1].lower() == "true"
keep_output_files = argv[2].lower() == "true"
specified_iter = argv[3] if len(argv) > 3 else "iter"

# Switch to top level directory
main_path = os.getcwd()
os.chdir(main_path + "/Team05/Tests05")
f = open("results.txt", "w")

dic = defaultdict(list)

def getExpectedTC(text):
    text_arr = text.splitlines()
    return len(text_arr) // 5

def getFailedTC(text, dir_path, source, queries, expectedTC):
    text = text.splitlines()
    ranTC = 0
    count = 0
    failed_tcs = []
    for i, x in enumerate(text):
        if x.find("</id>") != -1:
            pos = x.find("</id>") - 1
            res = ""
            while x[pos].isdigit():
                res += x[pos]
                pos -= 1

            res = res[::-1]
            ranTC = max(ranTC, int(res))
            continue
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
    f.write("Expected TC: " + str(expectedTC) + "\n")
    f.write("Number of completed TC runs: " + str(ranTC) + "\n")
    f.write(res + "\n")
    f.write(failed_count + "\n\n")

# Compile all source and matching query files
for dirpath, dirnames, filenames in os.walk(os.getcwd()):
    filenames.sort()
    for file in filenames:
        if file.startswith(specified_iter) and file.endswith("_source.txt"):
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

os.chdir(main_path)

output_files = []

if run_test_suites:
    for k in dic:
        for source, queries, prefix in dic[k]:
            dirpath = k + "/"
            source_input, queries_input, output_input = dirpath + source, dirpath + queries, dirpath + prefix + "_out.txt"
            f1 = open(queries_input[1:], "r")
            queries_text = f1.read()
            expectedTc = getExpectedTC(queries_text)
            f1.close()
            source_flag, queries_flag, output_flag = "source=" + source_input, "query=" + queries_input, "out=" + output_input

            print(source, queries, prefix)
            os.system(" ".join(["make", "autotester", source_flag, queries_flag, output_flag]))
            f2 = open(output_input[1:], "r")
            text = f2.read()
            f2.close()
            getFailedTC(text, k, source_input, queries_input, expectedTc)
            output_files.append(output_input)

if not keep_output_files:
    for file_input in output_files:
        if os.path.exists("." + file_input):
            os.remove("." + file_input)

f.close()

