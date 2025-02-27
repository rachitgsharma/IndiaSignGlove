import sqlite3

def create_database():
    conn = sqlite3.connect("indiaSign_words.db")
    cursor = conn.cursor()
    
    cursor.execute("DROP TABLE IF EXISTS words")
    
    cursor.execute('''
        CREATE TABLE words (
            gesture_id TEXT PRIMARY KEY,
            word TEXT NOT NULL,
            hand_image TEXT,
            gesture_description TEXT,
            sign_image TEXT,
            hand TEXT,
            hand_orientation TEXT,
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
    print("✅ Database and table created successfully!")

if __name__ == "__main__":
    create_database()
