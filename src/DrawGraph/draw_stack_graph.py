import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import glob
import seaborn as sns

def exist_file(file_name):
    if not os.path.exists(file_name):
        print(file_name + "not exist", file=sys.stderr)
        sys.exit(1)

args = sys.argv
if len(args) == 2:
    data_path = args[1]
else:
    print("error")
    exit(1)

os.chdir(os.path.dirname(os.path.abspath(__file__)))
os.chdir("..\\..\\bin")
os.chdir(os.path.basename(data_path))

fig1 = plt.figure(figsize=(16, 12))
ax1 = fig1.add_subplot(111)
ax1.set_ylim(0, 100)

exist_file("search.dat")
data = np.loadtxt("search.dat")
x, search = np.split(data, [1], 1)
x = np.reshape(x, x.shape[0])
sort_search = search[:,[0, 1, 4, 9, 3, 6, 7, 8, 5, 2]]
ax1.set_xlim(0, x[-1])
search_labels = ["GroupPenalty", "ItemPenalty", "WeightPenalty", "Relation", "ValueAve", "Decrease", "Shift", "Swap", "Graph", "ValueDiv"]
sort_search_labels = [search_labels[i] for i in (0, 1, 4, 9, 3, 6, 7, 8, 5, 2)]
pal = sns.hls_palette(11, s=0.5)
#ax1.stackplot(x, search.T, colors=pal, labels=search_labels)
ax1.stackplot(x, sort_search.T, labels=sort_search_labels)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0, fontsize=18)
fig1.savefig("search_fig_stack.png", bbox_inches='tight', pad_inches=0.1)