import time

import serial

# port : 앞에서 arduino ide에서 확인한 포트 이름을 입력합니다
# baudrate : 아두이노 setup함수에서 serial.begin에 사용된 값을 사용합니다
# 포트가 연결되면 보드가 껐다 켜집니다
# 초기화가 완료될 시간을 약간 기다려줍니다
arduino = serial.Serial(port = "COM6", baudrate = 9600)
time.sleep(3)

# 시리얼에 test string을 보냄
arduino.write(b"test string")
# 잠시 대기
time.sleep(1)
data = arduino.read_all()
print(data)

arduino.close()