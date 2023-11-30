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

def save_table(results):
    # Extract unique training sizes
    training_sizes = sorted(set(results['Training Size']))

    # Iterate over unique training sizes and print tables
    for training_size in training_sizes:
        subset_data = [(program, key1, key2, key3, key4) for program, ts, key1, key2, key3, key4 in zip(results['Program'], results['Training Size'], results['tAverageApproximate'], results["tAverageTrue"], results["AAF"], results["MAF"]) if ts == training_size]

        # Convert data to tabulated format
        table = tabulate(subset_data, headers=[f'Training Size = {training_size}', 'tAverageApproximate', 'tAverageTrue', 'AAF', 'MAF'], tablefmt='fancy_grid', showindex=False, numalign="right", stralign="right")

        # Save table
        with open("EVALUATION/table.md", "a") as file:
            # Add a newline before appending the new table if the file is not empty
            if file.tell() > 0:
                file.write("\n\n")
            
            # Write the new table
            file.write(table)

    print("Tables were saved")

def save_plots(results):
    # Extract unique keys
    keys = set(results.keys()) - {'Training Size', 'Program'}

    # Iterate over keys and create line charts
    for key in keys:
        plt.figure(figsize=(7.5, 4.8))  # Create a new figure for each key
        plt.title(f'{key}')
        
        for program in set(results['Program']):
            subset_data = [(ts, value) for ts, prog, value in zip(results['Training Size'], results['Program'], results[key]) if prog == program]
            subset_data.sort(key=lambda x: x[0])  # Sort by Training Size
            
            training_sizes, values = zip(*subset_data)
            plt.plot(training_sizes, values, label=f'{program}', marker='o')

        plt.xlabel('Training Size')
        plt.ylabel(f"{key} values")
        plt.legend()
        plt.savefig(f'EVALUATION/{key}.png')

    print("Line charts were saved")

def save_csv(results):
    # Create a DataFrame from the dictionary
    df = pd.DataFrame(results)

    # Group the DataFrame by 'Training Size'
    grouped = df.groupby('Training Size')

    # Save each group to a single CSV file
    with open('EVALUATION/eval.csv', 'w', newline='') as csvfile:
        for size, group in grouped:
            # Write section header
            csvfile.write(f'Training Size={size},tAverageApproximate,tAverageTrue,AAF,MAF\n')

            # Write data for the section
            for index, row in group.iterrows():
                csvfile.write(f'{row["Program"]},{row["tAverageApproximate"]},{row["tAverageTrue"]},{row["AAF"]},{row["MAF"]}\n')

            # Add an empty line after each 'Training Size'
            csvfile.write('\n')

    print("CSV file was saved")

if __name__ == "__main__":
    
    execute_make(f"tests")
    
    results = {
        "Training Size": [],
        "Program": [],
        "tAverageApproximate": [],
        "tAverageTrue": [],
        "AAF": [],
        "MAF": []
    }

    training_sizes = [100, 500, 1000, 2000, 5000, 10000, 20000, 30000, 40000, 50000, 60000]
    for size in training_sizes:
        for name, command in zip(["lsh", "cube", "graph_gnns", "graph_mrng"], 
                                    [f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 4 -L 5 -N 3 -w 2240 -f {size}",
                                    f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 14 -M 6000 -probes 15 -N 3 -w 2240 -f {size}",
                                    f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 40 -E 30 -R 10 -N 3 -l 2000 -m 1 -f {size}",
                                    f"-d ../datasets/train-images.idx3-ubyte -q ../datasets/t10k-images.idx3-ubyte -k 40 -E 30 -R 10 -N 3 -l 2000 -m 2 -f {size}"]):
            if name != "lsh" and name != "cube":
                output = execute_test(name.split("_")[0], f"{command}")
            else:
                output = execute_test(name, f"{command}")
            output = output.split("\n")
            for curr_output in output:
                key, value = curr_output.split(":")
                results[key].append(float(value))
            results["Training Size"].append(size)
            if name != "lsh" and name != "cube":
                graph = name.split("_")[1]
                results["Program"].append(f"{graph.upper()}")
            else:
                results["Program"].append(f"{name.upper()}")

    save_table(results)

    save_plots(results)

    save_csv(results)

    execute_make("clean")