import serial
import numpy as np
from PIL import Image

# Setup the serial port
ser = serial.Serial('COM11', 115200)  # Replace 'COM_PORT' with your actual COM port

def yuv422_to_rgb(image_data, width, height):
    """
    Convert YUV422 formatted bytes to RGB numpy array.
    """
    # Initialize the output RGB array
    rgb_array = np.zeros((height, width, 3), dtype=np.uint8)
    
    # Step through the image_data 4 bytes at a time
    for i in range(0, len(image_data), 4):
        # Extract YUV components
        v = image_data[i]
        y1 = image_data[i+1]
        u = image_data[i+2]
        y2 = image_data[i+3]
        
        # Calculate the RGB values for the first pixel
        rgb_array[i // 2 // width][(i // 2) % width] = yuv_to_rgb(y1, u, v)
        # Calculate the RGB values for the second pixel
        rgb_array[i // 2 // width][(i // 2) % width + 1] = yuv_to_rgb(y2, u, v)
    
    return rgb_array

def yuv_to_rgb(y, u, v):
    """
    Convert a single YUV pixel to RGB.
    """
    c = y - 16
    d = u - 128
    e = v - 128
    return [
        max(0, min(255, (298 * c + 409 * e + 128) >> 8)),  # Red
        max(0, min(255, (298 * c - 100 * d - 208 * e + 128) >> 8)),  # Green
        max(0, min(255, (298 * c + 516 * d + 128) >> 8))  # Blue
    ]

def yuv422_to_gray(image_data, width, height):
    """
    Convert YUV422 formatted bytes to a grayscale numpy array.
    """
    # Initialize the output grayscale array
    gray_array = np.zeros((height, width), dtype=np.uint8)
    
    # Step through the image_data 4 bytes at a time
    # We only need the Y components, which are the first and third bytes of every four bytes
    index = 0
    for i in range(0, len(image_data), 4):
        y1 = image_data[i+1]     # First Y component
        y2 = image_data[i+3]   # Second Y component
        print(y1, y2)
        
        # Place the Y components in the array
        gray_array[index // width][index % width] = y1
        index += 1
        gray_array[index // width][index % width] = y2
        index += 1
    
    return gray_array

def wait_for_ready_signal():
    """
    Wait for the 'RDY' signal from Arduino before reading image data.
    """
    signal = b''
    while signal != b'RDY':
        signal = ser.read(3)  # Read 3 bytes from the serial, expecting 'RDY'
        if signal == b'RDY':
            break
        else:
            # Optionally, reset or keep reading until 'RDY' is found
            signal = b''


def read_image_from_serial():

   # wait_for_ready_signal()
    # Assuming dimensions and format (YUV422 for a 320x240 image)
    img_width = 320
    img_height = 240
    num_pixels = img_width * img_height
    
    # YUV422 has 2 pixels per 4 bytes, so we need to read this many bytes:
    num_bytes = num_pixels * 2  # 2 pixels in 4 bytes means num_pixels * 2 bytes
    img_data = ser.read(num_bytes)
    
    # Convert the YUV422 data to an RGB image
    #rgb_image = yuv422_to_rgb(img_data, img_width, img_height)
    gray = yuv422_to_gray(img_data, img_width, img_height)
    
    # Create an image from the RGB array
    image = Image.fromarray(gray, 'L')
    
    return image

# Read an image
image = read_image_from_serial()

# Show the image
image.show()

# Optionally, save the image
image.save("output.jpg")
