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
# Specify the file path
file_path = "out64.txt"

# Extract data from the file
data = extract_data(file_path)
print(data)

# Calculate average times
average_times = calculate_average(data)
print(average_times)


speedup = compute_speedup(average_times)
print("sppedup")
print(speedup)


# Print average times
for core_count, phases in average_times.items():
    print(f"{core_count} CORES:")
    for phase, average_time in phases.items():
        print(f"Average {phase} time: {average_time}")
    print("-------------------------")

# graph it
# compute speedup

# optimal




# Extract keys and values
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




