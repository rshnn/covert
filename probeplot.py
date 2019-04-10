from matplotlib import pyplot as plt
import seaborn as sns

with open('probes.txt') as f:
    array = []
    for val in f.read().split(): 
        if int(val) < 400: 
            array.append(int(val))


plt.figure(figsize=(15, 8))

ax = sns.distplot(array)
ax.set_title("Histogram of probe timings (n= {})".format(len(array)))
ax.set_xlabel("CYCLES")
plt.savefig('probe_histogram.png')
