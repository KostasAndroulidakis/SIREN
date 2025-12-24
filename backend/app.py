# app.py
from flask import Flask, jsonify, render_template
from flask_cors import CORS
import serial
import time
import threading
import os

app = Flask(__name__)
CORS(app)

# Only initialize serial in main process (not reloader)
if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
    ser = serial.Serial('/dev/cu.usbmodem2101', 115200, timeout=1)
    time.sleep(2)
else:
    ser = None

latest_data = {
    'angle': 0, 
    'distance': 0, 
    'humidity': 0, 
    'temperatureC': 0, 
    'temperatureF': 0
}

def read_serial():
    global latest_data
    if ser is None:
        return
    while True:
        try:
            if ser.in_waiting:
                line = ser.readline().decode().strip()
                print(f"Received: {line}")
                parts = line.split(',')
                if len(parts) == 5:
                    latest_data = {
                        'angle': int(parts[0]),
                        'distance': float(parts[1]),
                        'humidity': float(parts[2]),
                        'temperatureC': float(parts[3]),
                        'temperatureF': float(parts[4])
                    }
                    print(f"Parsed: {latest_data}")
        except Exception as e:
            print(f"Serial error: {e}")
            time.sleep(1)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data')
def get_data():
    return jsonify(latest_data)

if __name__ == '__main__':
    if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
        threading.Thread(target=read_serial, daemon=True).start()
    app.run(debug=True, host='0.0.0.0', port=5001)