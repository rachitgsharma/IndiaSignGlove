import sqlite3
import random

def generate_unique_data(index):
    """Generate unique values for each letter using a varying pattern."""
    return {
        "res": [random.randint(180, 250) + index for _ in range(5)],  # Unique resistance values
        "bend": [random.choice(["Straight", "Partial", "Full"]) for _ in range(5)],  # Random bend states
        "gyro": [round(random.uniform(-90, 90) + index * 0.1, 2) for _ in range(3)],  # Unique gyro values
        "acc": [round(random.uniform(-10, 10) + index * 0.1, 2) for _ in range(3)],  # Unique accelerometer values
        "ts": [random.randint(0, 1) for _ in range(4)]  # Random touch sensor states (0 or 1)
    }

def insert_barakhadi_data():
    conn = sqlite3.connect("indiaSign.db")
    cursor = conn.cursor()

    barakhadi_letters = [
        "હ", "હા", "હિ", "હી", "હૂ", "હુ", "હે", "હૈ", "હો", "હૌ", "હં", "હઃ",
        "લ", "લા", "લિ", "લી", "લુ", "લૂ", "લે", "લૈ", "લો", "લૌ", "લં", "લઃ",
        "ક", "કા", "કિ", "કી", "કુ", "કૂ", "કે", "કૈ", "કો", "કૌ", "કં", "કઃ",
        "મ", "મા", "મિ", "મી", "મુ", "મૂ", "મે", "મૈ", "મો", "મૌ", "મં", "મઃ",
        "છ", "છા", "છિ", "છી", "છુ", "છૂ", "છે", "છૈ", "છો", "છૌ", "છં", "છઃ",
        "ત", "તા", "તિ", "તી", "તુ", "તૂ", "તે", "તૈ", "તો", "તૌ", "તં", "તઃ",
        "ર", "રા", "રિ", "રી", "રુ", "રૂ", "રે", "રૈ", "રો", "રૌ", "રં", "રઃ",
        "ન", "ના", "નિ", "ની", "નુ", "નૂ", "ને", "નૈ", "નો", "નૌ", "નં", "નઃ",
        "શ", "શા", "શિ", "શી", "શુ", "શૂ", "શે", "શૈ", "શો", "શૌ", "શં", "શઃ",
        "જ", "જા", "જિ", "જી", "જુ", "જૂ", "જે", "જૈ", "જો", "જૌ", "જં", "જઃ",
        "ગ", "ગા", "ગિ", "ગી", "ગુ", "ગૂ", "ગે", "ગૈ", "ગો", "ગૌ", "ગં", "ગઃ",
        "વ", "વા", "વિ", "વી", "વુ", "વૂ", "વે", "વૈ", "વો", "વૌ", "વં", "વઃ",
        "પ", "પા", "પિ", "પી", "પુ", "પૂ", "પે", "પૈ", "પો", "પૌ", "પં", "પઃ",
        "ડ", "ડા", "ડિ", "ડી", "ડુ", "ડૂ", "ડે", "ડૈ", "ડો", "ડૌ", "ડં", "ડઃ",
        "સ", "સા", "સિ", "સી", "સુ", "સૂ", "સે", "સૈ", "સો", "સૌ", "સં", "સઃ",
        "ધ", "ધા", "ધિ", "ધી", "ધુ", "ધૂ", "ધે", "ધૈ", "ધો", "ધૌ", "ધં", "ધઃ"
    ]

    predefined_data = []

    for index, letter in enumerate(barakhadi_letters):
        unique_values = generate_unique_data(index)

        predefined_data.append((
            letter,
            *unique_values["res"],  # Resistance values
            *unique_values["bend"],  # Bend states
            *unique_values["gyro"],  # Gyroscope values
            *unique_values["acc"],  # Accelerometer values
            *unique_values["ts"],  # Touch sensor states
            "Yes"
        ))

    cursor.executemany('''
        INSERT INTO gestures (
            letter, res_1, res_2, res_3, res_4, res_5,
            bend_1, bend_2, bend_3, bend_4, bend_5,
            gyro_1, gyro_2, gyro_3, acc_1, acc_2, acc_3,
            ts_1, ts_2, ts_3, ts_4, predefined
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    ''', predefined_data)
    
    conn.commit()
    conn.close()
    print("✅ Barakhadi data with unique values inserted successfully!")

if __name__ == "__main__":
    insert_barakhadi_data()