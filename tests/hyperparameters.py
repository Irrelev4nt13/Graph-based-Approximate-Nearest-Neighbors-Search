import os
import sys
import subprocess
import re
import pandas as pd
import matplotlib.pyplot as plt
from tabulate import tabulate


def execute_make(command):
    # Save the current working directory
    current_directory = os.getcwd()

    # Change to the directory where your Makefile is located
    os.chdir("..")

    # Run the 'make tests' command
    result = subprocess.run(f"make -s {command}", shell=True)

    # Restore the original working directory
    os.chdir(current_directory)

def get_test_name():
    # Check if there are at least two command-line arguments (including the script name)
    if len(sys.argv) != 2 or sys.argv[1] not in ['lsh', 'cube', 'graph_gnns', 'graph_mrng']:
        print("Usage: python3 hyperparameters.py name_of_the_test_you_want [lsh, cube, graph_gnns, graph_mrng]")
        exit() 
    
    # return command-line argument
    return sys.argv[1]

def execute_test(test_name, args):
    # Get the current working directory
    current_directory = os.getcwd()

    # Construct the absolute path to the "bin" directory and change to the "bin" directory
    os.chdir(os.path.join(current_directory, os.pardir, "bin"))

    # Specify the command to run, including the C program and any command-line arguments
    command = f"./{test_name}_test {args}"

    # Use subprocess to run the command and capture the output
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

    # Change back to the original directory
    os.chdir(current_directory)

    return result.stdout.decode('utf-8')

def save_table(transposed_data, results, test_name, graph_type):
    # Create the table
    table = tabulate(transposed_data, headers=results.keys(), tablefmt="fancy_grid", showindex=False, numalign="right", stralign="right")

    # Center the title
    if graph_type is None:
        title = f"Results {test_name.upper()}"
    else:
        title = f"Results {graph_type.upper()}"
    padding = (len(table.split('\n', 1)[0]) - len(title)) // 2
    centered_title = " " * padding + title

    if graph_type is None:
        with open(f"{test_name.upper()}/table.md", "a") as file:
            # Add a newline before appending the new table if the file is not empty
            if file.tell() > 0:
                file.write("\n\n")
            
            # Write the new table
            file.write(title)
            file.write("\n")
            file.write(table)
    else:
        with open(f"{graph_type.upper()}/table.md", "a") as file:
            # Add a newline before appending the new table if the file is not empty
            if file.tell() > 0:
                file.write("\n\n")
            
            # Write the new table
            file.write(centered_title)
            file.write("\n")
            file.write(table)

    print("Tables were saved")

def save_plots(results, test_name, graph_type):
    # Extract names
    dict_keys_list = list(results.keys())
    zero = dict_keys_list[0]
    first = dict_keys_list[1]
    second = dict_keys_list[2]
    third = dict_keys_list[3]
    fourth = dict_keys_list[4]

    plt.figure()
    plt.plot(results[zero], results[first], marker='o', color='blue')
    plt.xlabel(f"{zero} values")
    plt.ylabel(f"{first} values")
    plt.title(f"f({zero}) = {first}")
    if graph_type is None:
        plt.savefig(f'{test_name.upper()}/{first}.png')
    else:
        plt.savefig(f'{graph_type.upper()}/{first}.png')

    plt.figure()
    plt.plot(results[zero], results[second], marker='o', color='green')
    plt.xlabel(f"{zero} values")
    plt.ylabel(f"{second} values")
    plt.title(f"f({zero}) = {second}")
    if graph_type is None:
        plt.savefig(f'{test_name.upper()}/{second}.png')
    else:
        plt.savefig(f'{graph_type.upper()}/{second}.png')
    
    plt.figure()
    plt.plot(results[zero], results[third], marker='o', color='orange')
    plt.xlabel(f"{zero} values")
    plt.ylabel(f"{third} values")
    plt.title(f"f({zero}) = {third}")
    if graph_type is None:
        plt.savefig(f'{test_name.upper()}/{third}.png')
    else:
        plt.savefig(f'{graph_type.upper()}/{third}.png')

    plt.figure()
    plt.plot(results[zero], results[fourth], marker='o', color='red')
    plt.xlabel(f"{zero} values")
    plt.ylabel(f"{fourth} values")
    plt.title(f"f({zero}) = {fourth}")
    if graph_type is None:
        plt.savefig(f'{test_name.upper()}/{fourth}.png')
    else:
        plt.savefig(f'{graph_type.upper()}/{fourth}.png')

    print("Line charts were saved")

def save_csv(results, test_name, graph_type):
    # Convert the dictionary to a DataFrame
    df = pd.DataFrame(results)

    # Save the DataFrame to a CSV file
    if graph_type is None:
        df.to_csv(f"{test_name.upper()}/{test_name}.csv", index=False)
    else:
        df.to_csv(f"{graph_type.upper()}/{test_name}_{graph_type}.csv", index=False)

    print("CSV file was saved")

if __name__ == "__main__":
    
    if len(get_test_name().split("_")) == 2:
        test_name, graph_type = get_test_name().split("_")      
    else:
        test_name, graph_type = get_test_name(), None

    execute_make(f"{test_name}-test")
    
    results = {}

    if test_name == "lsh":
        for w in [10, 100, 200, 300, 500, 1000, 1500, 2000, 2240, 2500, 3000]: #range(1, 21):
            output = execute_test(test_name, f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 4 -L 5 -N 3 -w {w} -s")
            output = output.split("\n")
            for curr_output in output:
                key, value = curr_output.split(":")
                if key in results:
                    if key == "w":
                        results[key].append(int(value))
                    else:
                        results[key].append(float(value))
                else:    
                    if key == "w":
                        results[key] = [int(value)]
                    else:
                        results[key] = [float(value)]
    elif test_name == "cube":
        for w in [10, 100, 200, 300, 500, 1000, 1500, 2000, 2240, 2500, 3000]: #range(1, 21):
            output = execute_test(test_name, f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 14 -M 6000 -probes 15 -N 3 -w {w} -s")
            output = output.split("\n")
            for curr_output in output:
                key, value = curr_output.split(":")
                if key in results:
                    if key == "w":
                        results[key].append(int(value))
                    else:
                        results[key].append(float(value))
                else:    
                    if key == "w":
                        results[key] = [int(value)]
                    else:
                        results[key] = [float(value)]
    elif test_name == "graph" and graph_type == "gnns":
        for r in [1, 2, 5, 10, 15, 20, 30, 40, 50, 100, 200, 500, 1000, 2000]:
            output = execute_test(test_name, f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 40 -E 30 -R {r} -N 3 -l 10 -m 1 -s")
            output = output.split("\n")
            for curr_output in output:
                key, value = curr_output.split(":")
                if key in results:
                    if key == "R":
                        results[key].append(int(value))
                    else:
                        results[key].append(float(value))
                else:    
                    if key == "R":
                        results[key] = [int(value)]
                    else:
                        results[key] = [float(value)]
    elif test_name == "graph" and graph_type == "mrng":
        for l in [20, 100, 300, 500, 600, 700, 800, 900, 1000, 2000, 2500]:
            output = execute_test(test_name, f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 40 -E 30 -R 1 -N 3 -l {l} -m 2 -s -f 20000")
            output = output.split("\n")
            for curr_output in output:
                key, value = curr_output.split(":")
                if key in results:
                    if key == "l":
                        results[key].append(int(value))
                    else:
                        results[key].append(float(value))
                else:    
                    if key == "l":
                        results[key] = [int(value)]
                    else:
                        results[key] = [float(value)]
    # Transpose the data
    transposed_data = list(map(list, zip(*results.values())))

    save_table(transposed_data, results, test_name, graph_type)

    save_plots(results, test_name, graph_type)

    save_csv(results, test_name, graph_type)

    execute_make("clean")