#Name: Priyankar Das
#ID: 2026A4PS1208H
#Task 1: Sensor Data Visualization and Filtering


#Import necessary libraries
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd
import numpy as np



# --- 1. Read and Clean Data ---
"""
    Reads depth data from CSV file
    Applies a median filter to drop extreme sensor spikes,
    followed by an exponential moving average (EMA) for noise smoothing.
    
    """
def read_and_filter_data(filepath="depth_data.csv"):
    try:
        df=pd.read_csv(filepath)
        df.columns = df.columns.str.strip().str.lower()  # Remove any leading/trailing whitespace from column names

        df['depth'] = pd.to_numeric(df['depth'], errors='coerce')  #Force depth column to numbers; text like '#VALUE!' becomes NaN
        
        df['depth']=df['depth'].ffill().bfill()  #Fill any corrupted NaN rows with adjacent valid numbers

        
    except FileNotFoundError:

        #Dummy data generator for testing if csv is missing
        time_pts = np.arange(0,60,1)
        depth_pts = 50 + 10*np.sin(time_pts/5) + np.random.normal(0, 2, size=len(time_pts))
        
        depth_pts[12] = 200  #Add random erratic spikes (sensor corruption)
        depth_pts[35] = -50


        df = pd.DataFrame({'time': time_pts, 'depth': depth_pts})

    #Filter erratic values (Spike removal using Rolling Median)
    df['filtered_depth']= df['depth'].rolling(window=3, min_periods=1, center=True).median()

    #Smooth random noise using Exponential Moving Average (EMA)
    df['smoothed_depth']=df['filtered_depth'].ewm(span=3, adjust=False).mean()

    return df

df= read_and_filter_data()

# --- 2. Real-Time Plot Setup ---

fig, ax = plt.subplots(figsize=(10,5)) 
fig.patch.set_facecolor("#f4f4f9")
ax.set_facecolor('#ffffff')

raw_line, = ax.plot([], [], label='Raw Sensor Data (Erratic)', color='#ff6b6b', alpha=0.5, linestyle='--')
clean_line, = ax.plot([], [], label='Filtered Depth (Smoothed)', color='#1dd1a1', linewidth=2)

ax.set_xlim(0, max(df['time']) + 2)
ax.set_ylim(min(df['depth'])-10, max(df['depth']) + 10)
ax.set_xlabel('Time (seconds)', fontsize=11, fontweight='bold')
ax.set_ylabel('Depth (meters)', fontsize=11, fontweight='bold')
ax.set_title("Odysseus' Vessel: Real-Time Sea Floor Depth Monitoring", fontsize=13, fontweight='bold', pad=12)
ax.grid(True, linestyle=':', alpha=0.6)
ax.legend(loc='upper right')

# --- 3. Animation ---
def animate(i):
    current_time = df['time'].iloc[:i+1]
    current_raw = df['depth'].iloc[:i+1]
    current_clean = df['smoothed_depth'].iloc[:i+1]

    raw_line.set_data(current_time, current_raw)
    clean_line.set_data(current_time, current_clean)
    return raw_line, clean_line

#Simulates live data streaming by progressively slicing the dataset and updating every 1000 ms.
ani = animation.FuncAnimation(
    fig, animate, frames=len(df), interval=1000, repeat=False
)

plt.tight_layout()
plt.show()
