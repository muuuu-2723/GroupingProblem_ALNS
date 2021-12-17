import matplotlib.pyplot as plt
import numpy as np
import os
import sys
import glob

def exist_file(file_name):
    if not os.path.exists(file_name):
        print(file_name + " not exist", file=sys.stderr)
        sys.exit(1)

args = sys.argv
if len(args) == 2:
    data_path = args[1]
else :
    print("error")
    exit(1)

os.chdir(os.path.dirname(os.path.abspath(__file__)))
os.chdir('..\\..\\bin')
os.chdir(os.path.basename(data_path))

exist_file("eval.dat")
x, now_eval, best_eval = np.loadtxt("eval.dat", unpack=True)

plt.rcParams["font.size"] = 20

fig1 = plt.figure(figsize=(16, 12))
ax1 = fig1.add_subplot(111)
ax1.plot(x, now_eval, color="lightcyan", label="now_eval")
ax1.plot(x, best_eval, color="red", label="best_eval")
if best_eval[1] < best_eval[-1]:
    ax1.set_ylim(best_eval[-1] - np.abs(best_eval[-1]) * 0.1, best_eval[-1] * 1.05)
else:
    ax1.set_ylim(best_eval[-1] - np.abs(best_eval[-1]) * 0.5, best_eval[1] + 50)

ax1.set_xlim(0, x[-1])
ax1.set_xlabel("探索回数", fontname="MS Gothic")
ax1.set_ylabel("評価値", fontname="MS Gothic")

exist_file("search.dat")
search = np.loadtxt("search.dat")
search_labels = ["GroupPenalty", "ItemPenalty", "WeightPenalty", "Relation", "ValueAve", "ValueSum", "Decrease", "Shift", "Swap", "Graph", "ValueDiv"]
cmap = plt.get_cmap("tab20")
#ax2 = fig1.add_subplot(122)
ax2 = ax1.twinx()
ax2.set_ylabel("選択確率", fontname="MS Gothic")

for i in range(1, search.shape[1]):
    ax2.plot(search[:,0], search[:,i], color=cmap(i - 1), label=search_labels[i - 1])
ax2.set_ylim(0, 100)

h1, l1 = ax1.get_legend_handles_labels()
h2, l2 = ax2.get_legend_handles_labels()
ax1.legend(h1 + h2, l1 + l2, bbox_to_anchor=(1.1, 1), loc='upper left', borderaxespad=0)
ax2.grid(True)
fig1.savefig("search_fig.png", bbox_inches='tight', pad_inches=0.1)

destroy_files = glob.glob("*_destroy.dat")

for file in destroy_files:
    fig2 = plt.figure(figsize=(16, 12))
    ax3 = fig2.add_subplot(111)
    ax3.plot(x, now_eval, color="lightcyan", label="now_eval")
    ax3.plot(x, best_eval, color="red", label="best_eval")
    if best_eval[1] < best_eval[-1]:
        ax3.set_ylim(best_eval[-1] - np.abs(best_eval[-1]) * 0.1, best_eval[-1] * 1.05)
    else:
        ax3.set_ylim(best_eval[-1] - np.abs(best_eval[-1]) * 0.5, best_eval[1] + 50)
    ax3.set_xlim(0, x[-1])
    ax3.set_xlabel("探索回数", fontname="MS Gothic")
    ax3.set_ylabel("評価値", fontname="MS Gothic")

    exist_file(file)
    destroy = np.loadtxt(file)
    if destroy.shape[1] == 6:
        destroy_labels = ["Random", "RandomGroup", "Minimum", "MinimumGroup", "UpperWeight"]
    else:
        destroy_labels = ["RandomGroup", "MinimumGroup", "UpperWeight"]
    ax4 = ax3.twinx()
    ax4.set_ylabel("選択確率", fontname="MS Gothic")

    for i in range(1, destroy.shape[1]):
        ax4.plot(destroy[:, 0], destroy[:, i], color=cmap(i - 1), label=destroy_labels[i - 1])
    ax4.set_ylim(0, 100)

    h3, l3 = ax3.get_legend_handles_labels()
    h4, l4 = ax4.get_legend_handles_labels()
    ax3.legend(h3 + h4, l3 + l4, bbox_to_anchor=(1.1, 1), loc='upper left', borderaxespad=0)
    ax4.grid(True)
    fig2.savefig(os.path.splitext(os.path.basename(file))[0] + "_fig.png", bbox_inches='tight', pad_inches=0.1)