import sqlite3
from tabulate import tabulate

def fetch_and_verify():
    conn = sqlite3.connect("indiaSign_words.db")
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM words")
    rows = cursor.fetchall()

    if not rows:
        print("⚠️ No records found in the database.")
    else:
        headers = [
            "gesture_id", "word", "hand_image", "gesture_description", "sign_image", "hand", "hand_orientation",
            "res_1", "res_2", "res_3", "res_4", "res_5",
            "bend_1", "bend_2", "bend_3", "bend_4", "bend_5",
            "gyro_1", "gyro_2", "gyro_3", "acc_1", "acc_2", "acc_3",
            "ts_1", "ts_2", "ts_3", "ts_4", "predefined"
        ]
        print(tabulate(rows, headers=headers, tablefmt="grid"))

    conn.close()

if __name__ == "__main__":
    fetch_and_verify()
