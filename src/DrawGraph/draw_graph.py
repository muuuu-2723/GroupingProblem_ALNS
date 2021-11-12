import matplotlib.pyplot as plt
import numpy as np
import os
import sys

def exist_file(file_name):
    if not os.path.exists(file_name):
        print(file_name + " not exist", file=sys.stderr)
        sys.exit(1)

args = sys.argv
if len(args) == 2:
    data_file = args[1]
else :
    print("error")
    exit(1)

os.chdir(os.path.dirname(os.path.abspath(__file__)))
os.chdir('..\\..\\bin')

exist_file("eval_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat")
x, now_eval, best_eval = np.loadtxt("eval_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat", unpack=True)

eval_max = best_eval[-1]

fig1 = plt.figure(figsize=(16, 12))
ax1 = fig1.add_subplot(111)
ax1.plot(x, now_eval, color="lightcyan", label="now_eval")
ax1.plot(x, best_eval, color="red", label="best_eval")
ax1.set_ylim(eval_max - np.abs(eval_max) * 00.1, eval_max + 50)
ax1.set_xlim(0, x[-1])

exist_file("search_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat")
search = np.loadtxt("search_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat")
search_labels = ["GroupPenalty", "ItemPenalty", "WeightPenalty", "Relation", "ValueAve", "ValueSum", "Decrease", "Shift", "Swap", "Graph", "ValueDiv"]
cmap = plt.get_cmap("tab20")
#ax2 = fig1.add_subplot(122)
ax2 = ax1.twinx()

for i in range(1, search.shape[1]):
    ax2.plot(search[:,0], search[:,i], color=cmap(i - 1), label=search_labels[i - 1])
ax2.set_ylim(0, 100)

h1, l1 = ax1.get_legend_handles_labels()
h2, l2 = ax2.get_legend_handles_labels()
ax1.legend(h1 + h2, l1 + l2, bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
ax2.grid(True)
fig1.savefig("search_fig_" + os.path.splitext(os.path.basename(data_file))[0] + ".png")

fig2 = plt.figure(figsize=(16, 12))
ax3 = fig2.add_subplot(111)
ax3.plot(x, now_eval, color="lightcyan", label="now_eval")
ax3.plot(x, best_eval, color="red", label="best_eval")
ax3.set_ylim(eval_max - np.abs(eval_max) * 00.1, eval_max + 50)
ax3.set_xlim(0, x[-1])

exist_file("destroy_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat")
destroy = np.loadtxt("destroy_" + os.path.splitext(os.path.basename(data_file))[0] + ".dat")
destroy_labels = ["Random", "RandomGroup", "Minimum", "MinimumGroup", "UpperWeight"]
ax4 = ax3.twinx()

for i in range(1, destroy.shape[1]):
    ax4.plot(destroy[:, 0], destroy[:, i], color=cmap(i - 1), label=destroy_labels[i - 1])
ax4.set_ylim(0, 100)

h3, l3 = ax3.get_legend_handles_labels()
h4, l4 = ax4.get_legend_handles_labels()
ax3.legend(h3 + h4, l3 + l4, bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0)
ax4.grid(True)
fig2.savefig("destroy_fig_" + os.path.splitext(os.path.basename(data_file))[0] + ".png")