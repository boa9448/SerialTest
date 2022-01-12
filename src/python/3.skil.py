from serial.tools import list_ports

port_list = list_ports.comports()
print(port_list)
print([port.name for port in port_list])

import os
def upload(port : str, src_path : str, arduino_dir : str = "C:\\Program Files (x86)\\Arduino", use_debug : bool = True) -> bool:
    arduino_bin = "arduino_debug.exe" if use_debug else "arduino.exe"
    arduino_bin_full_path = os.path.join(arduino_dir, arduino_bin)
    if not os.path.isfile(arduino_bin_full_path):
        return False

    upload_command = f"\"{arduino_bin_full_path}\" --board arduino:avr:uno --port {port} --upload {src_path}"

    ret_code = os.system(upload_command)
    return True if ret_code == 0 else False