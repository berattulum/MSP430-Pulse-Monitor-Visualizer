import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import time

# --- Seri Port Ayarları ---
PORT = 'COM8'
BAUDRATE = 9600
TIMEOUT = 1

# --- Seri Portu Aç ---
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    print(f"{PORT} portu açıldı.")
except Exception as e:
    print(f"{PORT} açılamadı:", e)
    exit()

# --- Veri Listesi ---
adc_values = []
bpm = 0
peak_times = []
threshold = 2150  # Tepe algılama için eşik

# --- Grafik Hazırlığı ---
fig, ax = plt.subplots(figsize=(12, 4))
fig.patch.set_facecolor('black')
ax.set_facecolor('black')

line, = ax.plot([], [], color='red', linewidth=2)

ax.set_xlim(0, 100)
ax.set_ylim(500, 4000)
ax.set_xticks(range(0, 101, 2))
ax.set_yticks(range(500, 4001, 50))
ax.grid(True, color='gold', linestyle='-', linewidth=0.3)
ax.set_title("Gerçek Zamanlı Kalp Atışı", color='white', fontsize=14, pad=12)
ax.set_xlabel("Zaman (örnek)", color='white')
ax.set_ylabel("ADC Değeri", color='white')
ax.tick_params(colors='white')

bpm_box = ax.text(0.75, 0.90, "", transform=ax.transAxes,
                  fontsize=16, fontweight='bold', color='white',
                  bbox=dict(facecolor='black', edgecolor='gold', boxstyle='round,pad=0.5'))

# --- Güncelleme Fonksiyonu ---
def update(frame):
    global bpm, peak_times

    try:
        line_raw = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line_raw.startswith("ADC:"):  # Hızlı filtreleme
            return line, bpm_box

        value_str = line_raw[4:].strip()  # "ADC: 2345" -> "2345"
        value = int(value_str)

        adc_values.append(value)
        if len(adc_values) > 100:
            adc_values.pop(0)

        # Tepe tespiti
        t = time.time()
        if value > threshold and len(adc_values) >= 3:
            if adc_values[-2] > adc_values[-3] and adc_values[-2] > adc_values[-1]:
                peak_times.append(t)

        peak_times = [pt for pt in peak_times if t - pt <= 2.0]

        if len(peak_times) > 1:
            intervals = [j - i for i, j in zip(peak_times[:-1], peak_times[1:])]
            avg_interval = sum(intervals) / len(intervals)
            bpm = int(60 / avg_interval)
        else:
            bpm = 0

        # Grafik güncelle
        line.set_data(range(len(adc_values)), adc_values)
        ax.set_xlim(0, len(adc_values))
        bpm_box.set_text(f"♥  {bpm} BPM")

    except Exception:
        pass  # Hataları yut, animasyon kesilmesin

    return line, bpm_box

# --- Animasyonu Başlat ---
ani = animation.FuncAnimation(fig, update, interval=30, blit=True)  # Daha düşük gecikme için interval=30ms

plt.tight_layout()
plt.show()

ser.close()
