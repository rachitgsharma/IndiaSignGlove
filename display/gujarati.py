import tkinter as tk
import time

def display_word(word):
    root = tk.Tk()
    root.title("Gujarati Text Display")
    root.configure(bg="white")
    root.geometry("800x480")  # Set window size to match 4.3-inch HDMI display
    
    label = tk.Label(root, text=word, font=("Noto Sans Gujarati", 24), bg="white", fg="black", padx=20, pady=10)
    label.pack(expand=True)
    
    root.after(3000, root.destroy)  # Close window after 3 seconds
    root.mainloop()

# Predefined sensor values and words
data_mapping = {
    "S001": "હેલો",
    "S002": "કેમ છો?",
    "S003": "આભાર",
    "S004": "શુભ દિવસ",
    "S005": "આવજો"
}

def show_predefined_words():
    for sensor_id, word in data_mapping.items():
        print(f"Displaying: {word}")
        display_word(word)
        time.sleep(1)  # Wait before displaying the next word

# Run the predefined display
show_predefined_words()
