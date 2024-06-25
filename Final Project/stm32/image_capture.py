import serial
import numpy as np
from PIL import Image

# Setup the serial port
ser = serial.Serial('COM11', 1000000)  # Replace 'COM_PORT' with your actual COM port

def save_grayscale_image(image_data, width, height, output_filename):
    """
    Convert raw grayscale pixel values into an image and save it.
    
    Args:
        image_data (bytes or list): Grayscale pixel values.
        width (int): Width of the image in pixels.
        height (int): Height of the image in pixels.
        output_filename (str): Name of the output image file to save.
    """
    # Ensure that the image data is in the form of a 1D NumPy array
    if isinstance(image_data, (bytes, bytearray, list)):
        image_array = np.array(image_data, dtype=np.uint8)
    else:
        raise TypeError("image_data must be bytes, bytearray, or a list of pixel values.")

    # Ensure the array is the expected size
    #if len(image_array) != width * height:
    #    raise ValueError(f"Expected {width * height} pixels, but got {len(image_array)}.")
    expected_size = width * height
    if len(image_array) != expected_size:
        print(f"Warning: Expected {expected_size} pixels, but got {len(image_array)}.")
        # Pad the image array with zeros
        padded_image_array = image_array + [0] * (expected_size - len(image_array))
    else:
        padded_image_array = image_array

    # Reshape the array into a 2D array
    image_2d = padded_image_array.reshape((height, width))

    # Create a grayscale image object
    grayscale_image = Image.fromarray(image_2d, mode='L')

    # Save the grayscale image to the specified file
    grayscale_image.save(output_filename)
    print(f"Image successfully saved to {output_filename}")


index = 0
while 1:
    
    try:
        # Buffer to accumulate incoming data
        data_buf = bytearray()
        
        # Loop until '*RDY*' sequence is detected
        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)  # Read all available data
                print(data)
                data_buf.extend(data)
                # Check if '*RDY*' is in the buffer
                if b'*RDY*' in data_buf:
                    break

        print("READY sequence detected, processing image data...")

        img_width = 80*2*2
        img_height = 60*2*2
        num_pixels = img_width * img_height
        
        # YUV422 has 2 pixels per 4 bytes, so we need to read this many bytes:
        num_bytes = num_pixels * 2  # 2 pixels in 4 bytes means num_pixels * 2 bytes
        # Continue to read the pixel data
        pixel_data = bytearray()
        while len(pixel_data) < num_bytes:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)
                print(data)
                pixel_data.extend(data)
                # Check if '*RDY*' is in the buffer
                
                if b'*RDY*' in pixel_data:
                    print(len(pixel_data))
                    print("NEW FRAME START")
                    break

        #pixel_data = pixel_data[: len(pixel_data) - 5]

        print("Data read complete. Total bytes read:", len(pixel_data))

    finally:
        print("CAPTURING IMAGE DATA")

        
        pixel_data = pixel_data[:num_pixels]
        save_grayscale_image(pixel_data, img_width, img_height, "demo/graytestimg_stm_testcase_original_320x240_qvgaTEST_" + str(index) + ".png") # working for stm32
    index += 1
