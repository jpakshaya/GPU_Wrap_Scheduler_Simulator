import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('viz_data.csv')
plt.figure(figsize=(10, 4))
plt.scatter(df['Cycle'], df['WarpID'], c=df['WarpID'], cmap='viridis', s=100)
plt.title('GPU Warp Scheduling Trace')
plt.xlabel('Clock Cycle')
plt.ylabel('Warp ID')
plt.grid(True, linestyle='--', alpha=0.6)
plt.show()