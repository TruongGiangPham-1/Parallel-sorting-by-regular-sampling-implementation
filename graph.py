import re

import numpy as np
import matplotlib.pyplot as plt

# Function to extract data from the file
def extract_data(file_path):
    data = {}
    current_core_count = None
    with open(file_path, 'r') as file:
        for line in file:
            if "CORES" in line:
                current_core_count = int(re.findall(r'\d+', line)[0])
                if (current_core_count not in data):
                    data[current_core_count] = {"Phase1": [], "Phase2": [], "Phase3": [], "Phase4": [], "Total": []}
            elif "Phase" in line:
                phase = line.split(" is ")[0].split(" ")[0]; 
                time = line.split(" is ")[1].split()[0]
                data[current_core_count][phase].append(int(time))
            elif "total time" in line:
                time = line.split(" is ")[1]
                data[current_core_count]["Total"].append(int(time))
    return data

# Function to calculate average times
def calculate_average(data):
    averages = {}
    for core_count, phases in data.items():
        averages[core_count] = {}
        for phase, times in phases.items():
            averages[core_count][phase] = sum(times) / len(times)
    return averages
def compute_speedup(data):
    baseCPU = 1
    baseTime = int(data[baseCPU]["Total"])
    speedup = {}
    for core_count, phases in data.items():
        speedup[core_count] = baseTime / int(phases["Total"]) 

    return speedup
def oneSpeedupgraph(file):
    data = extract_data(file)
    average_times = calculate_average(data)
    speedup = compute_speedup(average_times)
    keys = list(speedup.keys())
    values = list(speedup.values())

    # Plot
    plt.figure(figsize=(8, 6))
    plt.plot(keys, values, marker='o', linestyle='-')
    plt.plot(keys, keys, linestyle='--', label='y = x')
    plt.title('Graph of speedup Values')
    plt.xlabel('X (CORES)')
    plt.ylabel('Y (speedup)')
    plt.grid(True)
    plt.xticks(keys)
    plt.show()
    return


def multipleSpeedupgraph(files: list):
    data = {}
    for file in files:
        data[file] = extract_data(file)
    average_times = {}
    for file, file_data in data.items():
        average_times[file] = calculate_average(file_data)
    speedup = {}
    for file, file_data in average_times.items():
        speedup[file] = compute_speedup(file_data)


    file0 = files[0]
    plt.figure(figsize=(8, 6))
    for file in files:
        plt.plot(list(speedup[file].keys()), list(speedup[file].values()), marker='o', linestyle='-', label=file)
    plt.plot(list(speedup[file0].keys()), list(speedup[file0].keys()), linestyle='--', label='y = x')
    plt.title('Graph of speedup Values')
    plt.xlabel('X (CORES)')
    plt.ylabel('Y (speedup)')
    plt.grid(True)
    plt.xticks(list(speedup[file0].keys()))
    plt.legend()
    plt.show()

    return

def printAVGtime(average_times):
    for core_count, phases in average_times.items():
        print(f"{core_count} CORES:")
        for phase, average_time in phases.items():
            print(f"Average {phase} time: {average_time}")
        print("-------------------------")
    return

# Specify the file path
file_path = "out80M64.txt"


def main():
    #oneSpeedupgraph(file_path)
    
    files = ["out20M64.txt", "out40M64.txt", "out60M64.txt", "out80M64.txt", "out100M64.txt", "out256M64.txt"]
    multipleSpeedupgraph(files)

if __name__ == '__main__':
    main()



