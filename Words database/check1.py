import sqlite3

# Convert image to binary
def convert_to_binary(filename):
    with open(filename, 'rb') as file:
        return file.read()

def insert_words_data():
    conn = sqlite3.connect("indiaSign_words.db")
    cursor = conn.cursor()

    words_data = [
        ("G001", "હેલો", "Greeting gesture", convert_to_binary("C:\\Users\\Labdh\\OneDrive\\Desktop\\Capstone\\database\\Words database\\DBImages\\Hello.png"), 
         "Right", "Palm Forward", "B", "B", "S", "S", "S", "", "", "", "", "", "", 1, 0, 0, 0, "yes"),
        
        ("G002", "કેમ છો?", "How are you?", convert_to_binary("C:/Users/Labdh/OneDrive/Desktop/Capstone/database/Words database/DBImages/Kemcho.png"), 
         "Right", "Palm Side", "S", "B", "B", "B", "B", "", "", "", "", "", "", 0, 0, 0, 0, "yes"),
        
        ("G003", "તમારું", "Your gesture", convert_to_binary("C:/Users/Labdh/OneDrive/Desktop/Capstone/database/Words database/DBImages/tamaru.png"), 
         "Right", "Palm Forward", "S", "S", "B", "B", "B", "", "", "", "", "", "", 0, 0, 0, 0, "yes")
    ]

    cursor.executemany('''
        INSERT INTO words (
            gesture_id, word, gesture_description, sign_image_blob, hand, hand_orientation, 
            bend_1, bend_2, bend_3, bend_4, bend_5, 
            gyro_1, gyro_2, gyro_3, acc_1, acc_2, acc_3, 
            touch_sensor_1, touch_sensor_2, touch_sensor_3, touch_sensor_4, predefined
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    ''', words_data)

    conn.commit()
    conn.close()
    print("✅ Words data inserted successfully!")

if __name__ == "__main__":
    insert_words_data()
