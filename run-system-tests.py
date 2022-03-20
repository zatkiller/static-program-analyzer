import os

from collections import defaultdict
from sys import argv

run_test_suites = argv[1].lower() == "true"
keep_output_files = argv[2].lower() == "true"

# Switch to top level directory
os.chdir(os.path.dirname(__file__) + "/Team05/Tests05")
f = open("results.txt", "w")
main_path = os.getcwd()

dic = defaultdict(list)

def getFailedTC(text, dir_path, source, quries):
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
        if file.startswith("source_"):
            suffix = file[len("source_"):]
            prefix = "queries_"
            queries_matching_file = prefix + suffix
            if queries_matching_file not in filenames:
                print("Unable to find matching queries")
                break

            shortened_path = dirpath
            if shortened_path.startswith(main_path):
                shortened_path = shortened_path[len(main_path):]

            dic["/Team05/Tests05" + shortened_path].append((file, queries_matching_file, suffix))

# Switch back to top level directory
os.chdir(os.path.dirname(__file__))

output_files = []

if run_test_suites:
    for k in dic:
        files = dic[k]
        for source, queries, suffix in files:
            prefix = k + "/"

            source_input, queries_input, file_input = prefix + source, prefix + queries, prefix + "out_" + suffix

            source_path, queries_path, output_path = "source=" + prefix + source, "query=" + prefix + queries, "out=" + prefix + "out_" + suffix
            os.system(" ".join(["make", "autotester", source_path, queries_path, output_path]))

            f2 = open(file_input[1:], "r")
            text = f2.read()
            f2.close()
            getFailedTC(text, k, source_input, queries_input)
            output_files.append(file_input)

if not keep_output_files:
    for file_input in output_files:
        if os.path.exists("." + file_input):
            os.remove("." + file_input)

f.close()

