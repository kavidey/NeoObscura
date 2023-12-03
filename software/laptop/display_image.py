import threading
import serial
import numpy as np
import cv2
import time


def split_into_groups(data, n):
    return [data[i : i + n] for i in range(0, len(data), n)]


# Constants
BYTES_PER_CHANNEL = 2
NUM_CHANNELS = 3
HORIZONAL_RESOLUTION = 40
VERTICAL_RESOLUTION = 30
IMAGE_BYTES = (
    HORIZONAL_RESOLUTION * VERTICAL_RESOLUTION * NUM_CHANNELS * BYTES_PER_CHANNEL
)

# Global variables
image = np.zeros(
    (VERTICAL_RESOLUTION, HORIZONAL_RESOLUTION, NUM_CHANNELS), dtype=np.uint8
)
running = True
fps = 0

# Read image data from the serial port
def read_serial():
    global image
    global running
    global fps

    while running:
        try:
            ser = serial.Serial("/dev/tty.usbmodem0007700041561", 115200)

            while running:
                start_time = time.time()
                data = ser.readline()[:-1]
                if len(data) == IMAGE_BYTES:
                    pixels_strings = split_into_groups(data, BYTES_PER_CHANNEL)
                    pixels = [int(pixel, 16) for pixel in pixels_strings]
                    pixels = np.array(pixels, dtype=np.uint8).reshape(
                        (VERTICAL_RESOLUTION, HORIZONAL_RESOLUTION, NUM_CHANNELS)
                    )
                    image = pixels
                    fps = 1 / (time.time() - start_time)
                else:
                    print(f"Recieved invalid image with {len(data)} bytes")
        except OSError or ValueError:
            # print("Connection failed, reconnecting")
            time.sleep(1)

# Displays (and upscales) the image
def display_image():
    global image
    while True:
        displayImage = cv2.resize(image, (400, 300))
        cv2.putText(
            displayImage,
            f"{fps:.1f} FPS",
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (0, 0, 0),
            3,
            cv2.LINE_AA,
        )
        cv2.putText(
            displayImage,
            f"{fps:.1f} FPS",
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            1,
            (255, 255, 255),
            2,
            cv2.LINE_AA,
        )
        cv2.imshow("NeoObscura", displayImage)

        time.sleep(1 / 60)

        if cv2.waitKey(1) & 0xFF == ord("q"):
            break


if __name__ == "__main__":
    serial_thread = threading.Thread(target=read_serial)
    serial_thread.start()
    display_image()
    running = False
    serial_thread.join()

    cv2.destroyAllWindows()
