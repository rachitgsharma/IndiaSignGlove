import sqlite3

def create_database():
    conn = sqlite3.connect("indiaSign_words.db")
    cursor = conn.cursor()

    # Create words table
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS words (
            gesture_id TEXT PRIMARY KEY,
            word TEXT,
            gesture_description TEXT,
            sign_image BLOB,
            hand TEXT,
            hand_orientation TEXT,
            bend_1 TEXT,
            bend_2 TEXT,
            bend_3 TEXT,
            bend_4 TEXT,
            bend_5 TEXT,
            gyro_1 TEXT,
            gyro_2 TEXT,
            gyro_3 TEXT,
            acc_1 TEXT,
            acc_2 TEXT,
            acc_3 TEXT,
            touch_sensor_1 INTEGER,
            touch_sensor_2 INTEGER,
            touch_sensor_3 INTEGER,
            touch_sensor_4 INTEGER,
            predefined TEXT
        )
    ''')

    conn.commit()
    conn.close()
    print("✅ Database and table created successfully!")

if __name__ == "__main__":
    create_database()
