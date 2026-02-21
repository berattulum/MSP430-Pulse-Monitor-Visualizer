# MSP430-Pulse-Monitor-Visualizer
This project is a hybrid embedded system application that captures real-time heart pulse data using a TI MSP430 microcontroller and visualizes the heartbeats with a Python-based GUI. It demonstrates low-level hardware control (ADC, UART, Timers) combined with high-level data processing and visualization.

Precision Sampling: 12-bit ADC resolution on MSP430 for accurate pulse signal capture.

Real-Time Processing: Efficient peak detection algorithm implemented in Python for BPM (Beats Per Minute) calculation.

Live Visualization: Dynamic, low-latency (30ms) oscilloscope-style graph using Matplotlib.

Dual-Platform Integration: Seamless UART communication between C (Embedded) and Python (Desktop).
