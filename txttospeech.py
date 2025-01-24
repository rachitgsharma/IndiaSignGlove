# Import the required module for text-to-speech conversion
from gtts import gTTS
import os

# Function to convert text to speech
def text_to_speech(text, language, filename):
    """
    Convert the given text to speech and save it as an audio file.

    :param text: The text to be converted to speech
    :param language: The language code for the text (e.g., 'en' for English, 'gu' for Gujarati)
    :param filename: The name of the file to save the audio
    """
    try:
        # Convert text to speech
        tts = gTTS(text=text, lang=language, slow=False)
        # Save the audio file
        tts.save(filename)
        # Play the audio file
        os.system(f"start {filename}")
        print(f"Audio saved as {filename} and played successfully.")
    except Exception as e:
        print(f"An error occurred: {e}")

# Gujarati text and language code
guju1 = 'હેલ્લો, કેમ ચો,'
guju2 = 'તમારુ નામ સુ છે'
gujarati_text = guju1 + guju2
gujarati_language = 'gu'
gujarati_filename = "gujarati_welcome.mp3"

# Call the function for Gujarati text
text_to_speech(gujarati_text, gujarati_language, gujarati_filename)
