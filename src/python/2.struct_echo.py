import time
from ctypes import Structure, c_ubyte, c_ushort, c_uint32

import serial

# ctypes를 이용한 구조체 클래스 작성
class Packet(Structure):
    # pragma pack(push, 1)과 같은 표현
    # 구조체 정렬을 1바이트 단위로 하도록 설정
    _pack_ = 1

    # 실제 구조체 멤버 선언
    _fields_ = [("start_sign", c_ubyte)
                , ("data_type", c_ubyte)
                , ("data", c_uint32)
                , ("checksum", c_ushort)]

# 보낼 데이터를 생성
# 시작 사인은 #, 데이터 타입과 체크섬은 임의의 값으로 생성
# 데이터는 123을 입력
MY_DATA_TYPE = 1
data = Packet(ord('#'), MY_DATA_TYPE, 123, 456)

# 바이트로 변환해서 출력
print(bytes(data))

arduino = serial.Serial(port = "COM6", baudrate = 9600)
time.sleep(3)

# 시리얼에 데이터를 보냄
arduino.write(bytes(data))
# 잠시 대기
time.sleep(1)
data = arduino.read_all()
print(data)

arduino.close()

