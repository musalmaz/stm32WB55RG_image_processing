import serial
import time

# Print the mesage that comes with UART

# Configure your serial port connection
ser = serial.Serial(
    port='COM11',  # Replace 'COM_PORT' with your serial port name, e.g., 'COM3' or '/dev/ttyUSB0'
    baudrate=230400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1  # Timeout for read operation in seconds
)

def read_uart_message():
    buffer = ""
    index = 0
    try:
        while True:
            if ser.in_waiting > 0:
                # Read available data
                data = ser.read(ser.in_waiting).decode(errors='replace')
                buffer += data
                
                # Split the buffer at newline characters
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    print(index,"Received:", line)
                    index += 1
            time.sleep(0.001)
    except KeyboardInterrupt:
        print("Program interrupted by user.")
    finally:
        ser.close()

if __name__ == "__main__":
    read_uart_message()
