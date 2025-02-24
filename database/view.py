import sqlite3

def view_database():
    conn = sqlite3.connect("indiaSign.db")
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM gestures")
    rows = cursor.fetchall()

    column_names = [description[0] for description in cursor.description]
    print("\t".join(column_names))  

    for row in rows:
        print("\t".join(str(item) if item is not None else "NULL" for item in row))

    conn.close()

if __name__ == "__main__":
    view_database()
