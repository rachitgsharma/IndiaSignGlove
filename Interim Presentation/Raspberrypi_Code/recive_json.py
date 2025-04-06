import serial
import json
import tkinter as tk

# Define predefined patterns
patterns = [
    {"pattern": ["B", "B", "S", "S", "S"], "touch": [1, 0, 0, 0], "meaning": "હેલો"},
    {"pattern": ["S", "B", "B", "B", "B"], "touch": [0, 0, 0, 0], "meaning": "કેમ છો?"},
    {"pattern": ["S", "S", "B", "B", "B"], "touch": [0, 0, 0, 0], "meaning": "તમારું"},
    {"pattern": ["S", "B", "B", "B", "S"], "touch": [0, 0, 0, 0], "meaning": "નામ, શું છે?"},
    {"pattern": ["B", "S", "B", "B", "S"], "touch": [0, 1, 1, 0], "meaning": "આભાર"},
    {"pattern": ["B", "S", "S", "B", "B"], "touch": [0, 0, 1, 1], "meaning": "હા"},
    {"pattern": ["B", "B", "B", "B", "B"], "touch": [0, 0, 0, 0], "meaning": "ના"}
]

# Initialize serial communication
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

def display_word(word):
    """Create a Tkinter window to display the detected Gujarati text."""
    root = tk.Tk()
    root.title("Gujarati Text Display")
    root.configure(bg="white")
    root.geometry("800x480")  # Set window size for a 4.3-inch HDMI display

    label = tk.Label(root, text=word, font=("Noto Sans Gujarati", 40), bg="white", fg="black", padx=20, pady=10)
    label.pack(expand=True)

    root.after(3000, root.destroy)  # Close window after 3 seconds
    root.mainloop()

while True:
    try:
        line = ser.readline().decode('utf-8').strip()
        if not line:
            continue  # Ignore empty lines

        # Ensure the received data is JSON
        if not line.startswith('{') or not line.endswith('}'):
            print("Skipping non-JSON data:", line)
            continue

        data = json.loads(line)  # Convert JSON string to Python dictionary
        
        # Ensure the parsed data is a dictionary
        if not isinstance(data, dict):
            print("Skipping non-dictionary JSON:", data)
            continue

        print("Received:", data)

        bend_states = data.get("bendStates", [])
        touch_states = data.get("touchStates", [])

        # Check if received data matches any pattern
        for pattern in patterns:
            if bend_states == pattern["pattern"] and touch_states == pattern["touch"]:
                print("Detected Sign:", pattern["meaning"])
                display_word(pattern["meaning"])  # Display in Tkinter window
                break
        else:
            print("No matching sign found.")

    except json.JSONDecodeError:
        print("Invalid JSON:", line)
    except Exception as e:
        print("Error:", e)
