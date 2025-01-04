import tkinter as tk
from tkinter import messagebox
import random
from tkinter import ttk

def generate_address_sequence():
    sequence = []
    m = 160  # 初始化 m
    for i in range(80):
        j = i * 4
        # 生成四个地址
        sequence.append(m)
        sequence.append(m + 1)
        # 生成一个随机地址在 [0, m) 范围内
        sequence.append(int(m * random.random()))
        # 生成地址的后继
        sequence.append(sequence[-1] + 1)
        # 更新 m，根据公式计算新的 m
        m = sequence[-1] + (319 - sequence[-1]) * random.random()
    return sequence
def convert_to_page_addresses(sequence):
    return [addr // 10 for addr in sequence]

# 页面置换算法模块
def fifo(page_stream, capacity):
    memory = []
    page_faults = 0

    for page in page_stream:
        if page not in memory:
            page_faults += 1
            if len(memory) >= capacity:
                memory.pop(0)
            memory.append(page)
    return page_faults

def lru(page_stream, capacity):
    memory = []
    page_faults = 0

    for page in page_stream:
        if page not in memory:
            page_faults += 1
            if len(memory) >= capacity:
                memory.pop(0)
        else:
            memory.remove(page)
        memory.append(page)
    return page_faults

def opt(page_stream, capacity):
    memory = []
    page_faults = 0

    for i, page in enumerate(page_stream):
        if page not in memory:
            page_faults += 1
            if len(memory) >= capacity:
                future_uses = [page_stream[i + 1:].index(p) if p in page_stream[i + 1:] else float('inf') for p in memory]
                memory.pop(future_uses.index(max(future_uses)))
            memory.append(page)
    return page_faults

def lfu(page_stream, capacity):
    memory = []
    frequency = {}
    page_faults = 0

    for page in page_stream:
        if page not in memory:
            page_faults += 1
            if len(memory) >= capacity:
                lfu_page = min(memory, key=lambda p: frequency[p])
                memory.remove(lfu_page)
                del frequency[lfu_page]
            memory.append(page)
        frequency[page] = frequency.get(page, 0) + 1
    return page_faults

# 绘图模块
def draw_chart(canvas, capacities, results, algorithms):
    canvas.delete("all")

    # 画布尺寸
    width = 500  # 调整宽度以适应表格
    height = 400  # 保持合适的高度

    # 坐标系边距
    margin = 50

    # 绘制坐标轴
    canvas.create_line(margin, height - margin, width - margin, height - margin, arrow=tk.LAST)  # X轴
    canvas.create_line(margin, height - margin, margin, margin, arrow=tk.LAST)  # Y轴

    # 坐标轴标记
    for i in range(0, 11):
        y = height - margin - i * (height - 2 * margin) / 10
        canvas.create_line(margin - 5, y, margin + 5, y)
        canvas.create_text(margin - 20, y, text=f"{i / 10:.1f}")

    for i, capacity in enumerate(capacities):
        x = margin + i * (width - 2 * margin) / (len(capacities) - 1)
        canvas.create_line(x, height - margin - 5, x, height - margin + 5)
        canvas.create_text(x, height - margin + 20, text=str(capacity))

    # 绘制曲线
    colors = ["red", "blue", "green", "purple"]
    line_positions = []  # 用于存储每条曲线最后一个点的位置，确保图例顺序不重叠
    for algo_idx, result in enumerate(results):
        points = []
        for i, value in enumerate(result):
            x = margin + i * (width - 2 * margin) / (len(capacities) - 1)
            y = height - margin - value * (height - 2 * margin)
            points.append((x, y))

        # 绘制曲线
        for j in range(len(points) - 1):
            canvas.create_line(points[j][0], points[j][1], points[j + 1][0], points[j + 1][1], fill=colors[algo_idx], width=2)

        # 记录每条曲线最后一个点的位置，用于图例的顺序
        line_positions.append((points[-1][0], points[-1][1]))

    # 排序图例，按照Y坐标从上到下显示
    sorted_algorithms = [x for _, x in sorted(zip(line_positions, algorithms), key=lambda x: x[1])]

    # 固定位置显示图例
    for idx, algo in enumerate(sorted_algorithms):
        y_offset = height - margin - 20 * (idx + 1)  # 设置图例从上到下依次排列
        x_offset = width - margin + 20  # 控制图例横向位置
        canvas.create_text(x_offset, y_offset, text=algo, fill=colors[algorithms.index(algo)])

# 表格输出模块
def update_table(table, capacities, fifo_results, lru_results, opt_results, lfu_results):
    for i in table.get_children():
        table.delete(i)

    for i, capacity in enumerate(capacities):
        table.insert("", "end", values=(capacity, f"{fifo_results[i]:.2f}", f"{lru_results[i]:.2f}", f"{opt_results[i]:.2f}", f"{lfu_results[i]:.2f}"))

# 图形化界面模块
def run_simulation():
    try:
        min_capacity = int(entry_min_capacity.get())
        max_capacity = int(entry_max_capacity.get())

        if min_capacity < 4 or max_capacity > 32 or min_capacity > max_capacity:
            raise ValueError("容量范围应在4到32之间，且最小值不大于最大值。")

        sequence = generate_address_sequence()
        page_stream = convert_to_page_addresses(sequence)

        fifo_results = []
        lru_results = []
        opt_results = []
        lfu_results = []
        capacities = list(range(min_capacity, max_capacity + 1))

        for capacity in capacities:
            fifo_faults = fifo(page_stream, capacity)
            lru_faults = lru(page_stream, capacity)
            opt_faults = opt(page_stream, capacity)
            lfu_faults = lfu(page_stream, capacity)

            fifo_results.append(1 - 1.0 * fifo_faults / 320)
            lru_results.append(1 - 1.0 * lru_faults / 320)
            opt_results.append(1 - 1.0 * opt_faults / 320)
            lfu_results.append(1 - 1.0 * lfu_faults / 320)

        draw_chart(canvas, capacities, [fifo_results, lru_results, opt_results, lfu_results], ["FIFO", "LRU", "OPT", "LFU"])
        update_table(table, capacities, fifo_results, lru_results, opt_results, lfu_results)

    except ValueError as e:
        messagebox.showerror("输入错误", str(e))

# 主窗口设计
root = tk.Tk()
root.title("存储管理设计实验")

# 使用框架来组织表格和图形
frame = tk.Frame(root)
frame.pack(padx=10, pady=10)

# 设置表格的左侧
table_frame = tk.Frame(frame)
table_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

columns = ("容量", "FIFO命中率", "LRU命中率", "OPT命中率", "LFU命中率")
table = ttk.Treeview(table_frame, columns=columns, show="headings", height=10)  # 控制显示的行数
table.heading("容量", text="容量")
table.heading("FIFO命中率", text="FIFO命中率")
table.heading("LRU命中率", text="LRU命中率")
table.heading("OPT命中率", text="OPT命中率")
table.heading("LFU命中率", text="LFU命中率")

# 设置列宽
table.column("容量", width=50)
table.column("FIFO命中率", width=100)
table.column("LRU命中率", width=100)
table.column("OPT命中率", width=100)
table.column("LFU命中率", width=100)

table.pack(fill=tk.BOTH, expand=True)  # 使表格自适应父容器大小

# 设置画布的右侧
canvas_frame = tk.Frame(frame)
canvas_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

canvas = tk.Canvas(canvas_frame, width=600, height=400, bg="white")
canvas.pack()

# 控制输入框
input_frame = tk.Frame(root)
input_frame.pack(padx=10, pady=10)

label_min_capacity = tk.Label(input_frame, text="最小实存容量:")
label_min_capacity.grid(row=0, column=0, padx=5, pady=5)
entry_min_capacity = tk.Entry(input_frame)
entry_min_capacity.grid(row=0, column=1, padx=5, pady=5)

label_max_capacity = tk.Label(input_frame, text="最大实存容量:")
label_max_capacity.grid(row=1, column=0, padx=5, pady=5)
entry_max_capacity = tk.Entry(input_frame)
entry_max_capacity.grid(row=1, column=1, padx=5, pady=5)

button_run = tk.Button(input_frame, text="运行模拟", command=run_simulation)
button_run.grid(row=2, column=0, columnspan=2, pady=10)

# 调整表格和画布的大小
frame.grid_rowconfigure(0, weight=1)
frame.grid_columnconfigure(0, weight=1)
frame.grid_columnconfigure(1, weight=1)

root.mainloop()