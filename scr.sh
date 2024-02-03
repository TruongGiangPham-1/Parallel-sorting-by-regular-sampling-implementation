#!/bin/bash

# Function to run './a.out' with specified parameters
run_program() {
    ./a.out "$1" "$2"
}

# Number of repetitions for each value of P
repetitions=5

# Values of P
processors=(1 2 4 8 16)

# Number to use for parameter N
n_value=100000000

# Loop through each value of P
for p in "${processors[@]}"; do
    # Run the program 'repetitions' times with current value of P
    for ((i = 1; i <= repetitions; i++)); do
        echo "Running './a.out' with N=$n_value and P=$p (Iteration $i)"
        run_program "$n_value" "$p" > out2.txt
    done
done

