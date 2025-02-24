import sqlite3

def create_database():
    conn = sqlite3.connect("indiaSign.db")
    cursor = conn.cursor()
    
    # Drop table if it exists (to prevent old structures from interfering)
    cursor.execute("DROP TABLE IF EXISTS gestures")
    
    # Create table with the correct schema
    cursor.execute('''
        CREATE TABLE gestures (
            gesture_id INTEGER PRIMARY KEY AUTOINCREMENT,
            letter TEXT NOT NULL,
            res_1 REAL, res_2 REAL, res_3 REAL, res_4 REAL, res_5 REAL,
            bend_1 TEXT, bend_2 TEXT, bend_3 TEXT, bend_4 TEXT, bend_5 TEXT,
            gyro_1 REAL, gyro_2 REAL, gyro_3 REAL,
            acc_1 REAL, acc_2 REAL, acc_3 REAL,
            ts_1 INTEGER, ts_2 INTEGER, ts_3 INTEGER, ts_4 INTEGER,
            predefined TEXT
        )
    ''')
    
    conn.commit()
    conn.close()
    print("✅ Database and table recreated successfully!")

if __name__ == "__main__":
    create_database()
