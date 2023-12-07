import threading
import serial
import numpy as np
import cv2
import time
import argparse
from pathlib import Path
import qoi


def split_into_groups(data, n):
    return [data[i : i + n] for i in range(0, len(data), n)]


# Read image data from the serial port
def read_serial():
    global image
    global running
    global fps

    while running:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUDRATE)

            while running:
                start_time = time.time()
                if COMPRESSION == "none":
                    data = ser.readline()[:-1]
                    if len(data) == IMAGE_BYTES:
                        try:
                            pixels_strings = split_into_groups(data, BYTES_PER_CHANNEL)
                            pixels = [int(pixel, 16) for pixel in pixels_strings]
                            pixels = np.array(pixels, dtype=np.uint8).reshape(
                                (VERTICAL_RESOLUTION, HORIZONTAL_RESOLUTION, NUM_CHANNELS)
                            )
                            image = pixels
                        except Exception as e:
                            print(f"Recieved invalid image: {e}")
                    else:
                        print(f"Recieved invalid image with {len(data)} bytes")
                elif COMPRESSION == "qoi":
                    try:
                        data = ser.read_until(bytes([0,0,0,0,0,0,0,1]))
                        # print(data)
                        # print(len(data))
                        image = qoi.decode(data)
                        # print(image)
                        if SAVE_FIRST_IMAGE:
                            i = 0
                            while (OUTPUT_DIR / f"image_{i}.qoi").exists():
                                i += 1
                            
                            with (OUTPUT_DIR / f"image_{i}.qoi").open("wb") as f:
                                f.write(data)
                            running = False
                            break
                    except Exception as e:
                        print(f"Recieved invalid image qoi image: {e}")
                fps = 1 / (time.time() - start_time)
        except OSError or ValueError:
            # print("Connection failed, reconnecting")
            time.sleep(1)


# Displays (and upscales) the image
def display_image():
    global image
    global running
    while running:
        displayImage = cv2.cvtColor(cv2.resize(image, (400, 300)), cv2.COLOR_RGB2BGR)
        if SHOW_FPS:
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

        if cv2.waitKey(1) & 0xFF == ord("s"):
            i = 0
            while (OUTPUT_DIR / f"image_{i}.png").exists():
                i += 1
            cv2.imwrite(str(OUTPUT_DIR / f"image_{i}.png"), image)
            print("saved image")
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Reads and displays image data from the serial port of the NeoObscura camera"
    )
    parser.add_argument(
        "-sp",
        "--serialport",
        help="Serial port to read from",
        type=str,
        default="/dev/tty.usbmodem0007700041561",
    )
    parser.add_argument(
        "-br",
        "--baudrate",
        help="Baudrate of the serial port",
        type=int,
        default=921600,
    )
    parser.add_argument(
        "-c", "--channels", help="Number of channels per pixel", type=int, default=3
    )
    parser.add_argument(
        "-pw", "--width", help="Width of the image in pixels", type=int, default=40
    )
    parser.add_argument(
        "-ph", "--height", help="Height of the image in pixels", type=int, default=30
    )
    parser.add_argument(
        "--showfps",
        help="Display the FPS on the image",
        action="store_const",
        const=False,
        default=True,
    )
    parser.add_argument(
        "-cmp",
        "--compression",
        help="Decompress the image before displaying (overrides: --channels, --width, --height)",
        type=str,
        default="none",
    )
    parser.add_argument(
        "-o", "--output", help="Output file directory", type=str, default="snapshots"
    )
    parser.add_argument(
        "--save-first-image",
        help="Saves the first image to the output directory and exits. This is only valid when used with --compression",
        action="store_const",
        const=True,
        default=False,
    )

    args = parser.parse_args()
    # print(args)

    # Parameters
    SERIAL_PORT = args.serialport
    BAUDRATE = args.baudrate
    SHOW_FPS = args.showfps
    OUTPUT_DIR = Path(args.output)
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    COMPRESSION = args.compression
    SAVE_FIRST_IMAGE = args.save_first_image

    # Constants
    BYTES_PER_CHANNEL = 2
    NUM_CHANNELS = args.channels
    HORIZONTAL_RESOLUTION = args.width
    VERTICAL_RESOLUTION = args.height
    IMAGE_BYTES = (
        HORIZONTAL_RESOLUTION * VERTICAL_RESOLUTION * NUM_CHANNELS * BYTES_PER_CHANNEL
    )

    # Global variables
    image = np.zeros(
        (VERTICAL_RESOLUTION, HORIZONTAL_RESOLUTION, NUM_CHANNELS), dtype=np.uint8
    )
    running = True
    fps = 0

    serial_thread = threading.Thread(target=read_serial)
    serial_thread.start()
    if not SAVE_FIRST_IMAGE:
        display_image()
        running = False
    serial_thread.join()

    cv2.destroyAllWindows()
