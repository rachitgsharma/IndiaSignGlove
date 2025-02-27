import sqlite3
from tabulate import tabulate

def view_database():
    conn = sqlite3.connect("indiaSign_words.db")
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM words")
    rows = cursor.fetchall()
    conn.close()
    
    if not rows:
        print("⚠️ No data found in the database!")
        return

    headers = [
        "Gesture ID", "Word", "Hand Image", "Gesture Description", "Sign Image", "Hand", "Hand Orientation",
        "Res 1", "Res 2", "Res 3", "Res 4", "Res 5",
        "Bend 1", "Bend 2", "Bend 3", "Bend 4", "Bend 5",
        "Gyro 1", "Gyro 2", "Gyro 3", "Acc 1", "Acc 2", "Acc 3",
        "Touch Sensor 1", "Touch Sensor 2", "Touch Sensor 3", "Touch Sensor 4", "Predefined"
    ]
    
    print(tabulate(rows, headers=headers, tablefmt="grid"))

if __name__ == "__main__":
    view_database()
