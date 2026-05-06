import requests
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib

matplotlib.use('TkAgg')

URL = "http://10.0.0.239/data"

data_history = {
    "time": [], "aqi": [], "tvoc": [],
    "eco2": [], "temp": [], "hum": []
}
counter = 0

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))


def update(frame):
    global counter
    try:
        res = requests.get(URL, timeout=1.5)
        if res.status_code == 200:
            data = res.json()

            counter += 1
            data_history["time"].append(counter)
            data_history["aqi"].append(data.get("aqi", 0))
            data_history["tvoc"].append(data.get("tvoc", 0))
            data_history["eco2"].append(data.get("eco2", 0))
            data_history["temp"].append(data.get("temp", 0))
            data_history["hum"].append(data.get("hum", 0))

            for key in data_history:
                if len(data_history[key]) > 50:
                    data_history[key].pop(0)

            ax1.clear()
            ax1.plot(data_history["time"], data_history["aqi"], label="AQI", color="red")
            ax1.plot(data_history["time"], data_history["tvoc"], label="TVOC", color="green")
            ax1.set_title("VOC")
            ax1.legend(loc="upper left")
            ax1.grid(True)

            ax2.clear()
            ax2.plot(data_history["time"], data_history["temp"], label="Temp (°C)", color="orange")
            ax2.plot(data_history["time"], data_history["hum"], label="Hum (%)", color="blue")
            ax2.set_title("Humidity and Temperature")
            ax2.legend(loc="upper left")
            ax2.grid(True)

            print(f"Updated #{counter}: {data}")

    except Exception as e:
        print(f"Error: {e}")

ani = FuncAnimation(fig, update, interval=1000, cache_frame_data=False)

plt.tight_layout()
plt.show()