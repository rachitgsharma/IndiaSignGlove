import sqlite3
import os

def view_words():
    db_path = "indiaSign_words.db"

    # Check if database file exists
    if not os.path.exists(db_path):
        print("❌ Database file not found!")
        return

    # Connect to the database
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    # Fetch data from words table
    try:
        cursor.execute("SELECT gesture_id, word, gesture_description FROM words")
        rows = cursor.fetchall()

        if not rows:
            print("⚠️ No data found in the words table.")
        else:
            print("\n✅ Words Table Data:\n")
            print("{:<10} {:<15} {:<30}".format("ID", "Word", "Description"))
            print("=" * 55)
            for row in rows:
                print("{:<10} {:<15} {:<30}".format(row[0], row[1], row[2]))

    except sqlite3.Error as e:
        print(f"❌ Database error: {e}")

    # Close the connection
    conn.close()

if __name__ == "__main__":
    view_words()
