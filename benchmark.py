import subprocess
import re
import matplotlib.pyplot as plt

# Define matrix sizes to test
sizes = [128, 256, 512, 1024]
naive_times = []
strassen_times = []

for size in sizes:
    print(f"Running benchmarks for matrix size {size}x{size}")
    # Call the executable with the size as an argument
    result = subprocess.run(["./matrix_mul", str(size)], capture_output=True, text=True)
    output = result.stdout

    # Use regex to extract the time from the output
    match_naive = re.search(r"Naive Multiplication Time: (\d+) ms", output)
    match_strassen = re.search(r"Strassen's Multiplication Time: (\d+) ms", output)
    
    if match_naive and match_strassen:
        naive_time = int(match_naive.group(1))
        strassen_time = int(match_strassen.group(1))
        naive_times.append(naive_time)
        strassen_times.append(strassen_time)
        print(f"  Naive: {naive_time} ms, Strassen: {strassen_time} ms")
    else:
        print(f"Error parsing output for matrix size {size}")

# Plot the results
