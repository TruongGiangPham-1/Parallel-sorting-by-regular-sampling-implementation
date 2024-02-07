import re

import numpy as np
import matplotlib.pyplot as plt


# Function to extract data from the file
# WRITTEN BY GPT
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
    """
    returns 
    {core_count: {phase1: [time1, time2, time3, ...], phase2: [time1, time2, time3, ...], phase3: [time1, time2, time3, ...], phase4: [time1, time2, time3, ...], total: [time1, time2, time3, ...]}


    """
    return data

# Function to calculate average times
     #THIS FUNCTION WRITTEN IN BY GPT
def calculate_average(data):
    averages = {}
    for core_count, phases in data.items():
        averages[core_count] = {}
        for phase, times in phases.items():
            averages[core_count][phase] = sum(times) / len(times)
    """
    returns 
    {core_count: {phase1: average_time, phase2: average_time, phase3: average_time, phase4: average_time, total: average_time}}}
    """
    return averages
# WRITTEN BY GPT
def compute_speedup(data):
    baseCPU = 1
    baseTime = int(data[baseCPU]["Total"])
    speedup = {}
    for core_count, phases in data.items():
        speedup[core_count] = baseTime / int(phases["Total"]) 

    """
    returns 
    {core_count: speedup}
    """
    return speedup
# ADAPTED FROM THE  compute_speedup FUNCTION
def oneSpeedupgraph(file):
    data = extract_data(file)
    average_times = calculate_average(data)
    speedup = compute_speedup(average_times)
    keys = list(speedup.keys())
    values = list(speedup.values())
    print("speedup" + " ",  speedup)

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

# ADAPTED FROM THE  compute_speedup FUNCTION
def speedupEfficiencyGraph(files: list):
    data = {}
    for file in files:
        data[file] = extract_data(file)
    average_times = {}
    for file, file_data in data.items():
        average_times[file] = calculate_average(file_data)
    speedup = {}
    efficiency = {}
    for file, file_data in average_times.items():
        speedup[file] = compute_speedup(file_data)
    


    for file, file_data in speedup.items():
        efficiency[file] = {}
        for core_count, speedup in file_data.items():
            efficiency[file][core_count] = speedup / core_count


    file0 = files[0]
    plt.figure(figsize=(8, 6))
    for file in files:
        plt.plot(list(efficiency[file].keys()), list(efficiency[file].values()), marker='o', linestyle='-', label=file)
    #plt.plot(list(efficiency[file0].keys()), list(efficiency[file0].keys()), linestyle='--', label='y = x')
    plt.title('speedup efficiency')
    plt.xlabel('X (CORES)')
    plt.ylabel('Y (efficiency)')
    plt.grid(True)
    plt.xticks(list(efficiency[file0].keys()))
    plt.legend()
    plt.show()

    return

def multipleSpeedupgraph(files: list):
    data = {}
    for file in files:
        data[file] = extract_data(file)
    average_times = {}
    for file, file_data in data.items():
        average_times[file] = calculate_average(file_data)
        print("TIME FOR ", file)
        printAVGtime(average_times[file])
    speedup = {}
    for file, file_data in average_times.items():
        speedup[file] = compute_speedup(file_data)


    
    file0 = files[0]
    plt.figure(figsize=(8, 6))
    for file in files:
        plt.plot(list(speedup[file].keys()), list(speedup[file].values()), marker='o', linestyle='-', label=file)
    plt.plot(list(speedup[file0].keys()), list(speedup[file0].keys()), linestyle='--', label='y = x')
    plt.title('speedup')
    plt.xlabel('X (CORES)')
    plt.ylabel('Y (speedup)')
    plt.grid(True)
    plt.xticks(list(speedup[file0].keys()))
    plt.legend()
    plt.show()

    return




# WRITTEN BY GPT
def printAVGtime(average_times):
    for core_count, phases in average_times.items():
        print(f"{core_count} CORES:")
        for phase, average_time in phases.items():
            print(f"Average {phase} time: {average_time}")
        print("-------------------------")
    return

def plotPhases(file):
    data = extract_data(file)
    average_times = calculate_average(data)
    cores = list(average_times.keys())

    # compute sum of each phase for all cores
    phase1times = []
    phase2times = []
    phase3times = []
    phase4times = []
    # looks okay tho if u grad student
    for core in cores:
        phase1times.append(average_times[core]["Phase1"])
        phase2times.append(average_times[core]["Phase2"])
        phase3times.append(average_times[core]["Phase3"])
        phase4times.append(average_times[core]["Phase4"])

    # Plot CORES vs PHASES
    plt.figure(figsize=(8, 6))
    #plt.plot(cores, phase1times, marker='o', linestyle='-', label='Phase1')
    #plt.plot(cores, phase2times, marker='o', linestyle='-', label='Phase2')
    #plt.plot(cores, phase3times, marker='o', linestyle='-', label='Phase3')
    plt.plot(cores, phase4times, marker='o', linestyle='-', label='Phase4')
    plt.title('CORES vs PHASES' + " " + fileTitleDict[file])
    plt.xlabel('X (CORES)')
    plt.ylabel('Y (TIME Microseconds)')
    plt.grid(True)
    plt.xticks(cores)
    plt.legend()
    plt.show()

    return
# Specify the file path
file_path = "out80M64.txt"
fileTitleDict = {
    "out20M64.txt": "20M",
    "out40M64.txt": "40M",
    "out60M64.txt": "60M",
    "out80M64.txt": "80M",
    "out100M64.txt": "100M",
    "out256M64.txt": "256M",
    "256.txt": "256M"
        }


def main():
    oneSpeedupgraph(file_path)
    
    files = ["out20M64.txt", "out40M64.txt", "out60M64.txt", "out80M64.txt", "out100M64.txt", "out256M64.txt"]
    #multipleSpeedupgraph(files)
    #multipleSpeedupgraph(files)

    #plotPhases("out256M64.txt")
    #plotPhases("out20M64.txt")

    #plotPhases("256.txt")

    #multipleSpeedupgraph(files)
    speedupEfficiencyGraph(files)
if __name__ == '__main__':
    main()



