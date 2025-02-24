import sqlite3

# Function to fetch gesture data from the database
def fetch_gesture_data():
    conn = sqlite3.connect("indiaSign.db")
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM gestures")
    rows = cursor.fetchall()

    conn.close()
    return rows

# Function to verify input data against stored gestures
def verify_gesture(input_data):
    gestures = fetch_gesture_data()

    for row in gestures:
        (gesture_id, letter, res_1, res_2, res_3, res_4, res_5, 
         bend_1, bend_2, bend_3, bend_4, bend_5, 
         gyro_1, gyro_2, gyro_3, 
         acc_1, acc_2, acc_3, 
         ts_1, ts_2, ts_3, ts_4, predefined) = row

        # Checking if input data matches the stored gesture
        if (input_data["res_1"] == res_1 and 
            input_data["res_2"] == res_2 and 
            input_data["res_3"] == res_3 and 
            input_data["res_4"] == res_4 and 
            input_data["res_5"] == res_5 and 
            input_data["bend_1"] == bend_1 and 
            input_data["bend_2"] == bend_2 and 
            input_data["bend_3"] == bend_3 and 
            input_data["bend_4"] == bend_4 and 
            input_data["bend_5"] == bend_5 and 
            input_data["gyro_1"] == gyro_1 and 
            input_data["gyro_2"] == gyro_2 and 
            input_data["gyro_3"] == gyro_3 and 
            input_data["acc_1"] == acc_1 and 
            input_data["acc_2"] == acc_2 and 
            input_data["acc_3"] == acc_3 and 
            input_data["ts_1"] == ts_1 and 
            input_data["ts_2"] == ts_2 and 
            input_data["ts_3"] == ts_3 and 
            input_data["ts_4"] == ts_4):

            return {"Matched Letter": letter}

    return {"Error": "No match found"}

# Simulated input data (Replace this with real sensor input)
input_data = {
    "res_1": 220, "res_2": 225, "res_3": 215, "res_4": 210, "res_5": 205,
    "bend_1": "Straight", "bend_2": "Full", "bend_3": "Partial", "bend_4": "Full", "bend_5": "Full",
    "gyro_1": 0.6, "gyro_2": 0.5, "gyro_3": 0.4,
    "acc_1": 9.3, "acc_2": 0.6, "acc_3": 0.5,
    "ts_1": 1, "ts_2": 0, "ts_3": 1, "ts_4": 0
}

# Verify the gesture
result = verify_gesture(input_data)
print("🔍 Verification Result:", result)
