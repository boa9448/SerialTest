#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "Serial.h"

std::vector<std::wstring> SelectComPort()
{
    std::vector<std::wstring> comList;
    WCHAR lpTargetPath[MAX_PATH];

    for (int i = 0; i < 255; i++)
    {
        //COM1 과 같은 문자열로 만듬
        std::wstring str = L"COM" + std::to_wstring(i);

        //해당 장치가 사용 가능한지 체크
        DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, MAX_PATH);

        if (test != 0)
        {
            //사용 가능하다면 리스트에 추가
            comList.push_back(str);
        }
    }

    return comList;
}

void SerialTest()
{
    LPCWSTR lpPortName = L"COM4";
    HANDLE hPort = CreateFileW(lpPortName,  //포트 이름
        GENERIC_READ | GENERIC_WRITE,       //포트에 READ 와 WRITE가 가능하게 옵션 설정
        0,                                  //해당 포트를 공유하지 않음
        NULL,                               //보안 속성은 기본 보안속성으로
        OPEN_EXISTING,                      //CreateFile을 파일이 아닐 장치로 사용할떄는 항상 이 옵션
        FILE_ATTRIBUTE_NORMAL,              //일반 속성
        NULL);                              //템플릿핸들은 사용하지않음


    DCB dcbSerialParams = { 0 };            //직렬 통신 장치에 대한 제어 구조체
    GetCommState(hPort, &dcbSerialParams);  //열린 포트에 대한 설정을 가져옴


    dcbSerialParams.BaudRate = CBR_9600;    //baud rate를 9600으로 설정
    dcbSerialParams.ByteSize = 8;           //byte 크기를 8로 설정
    dcbSerialParams.StopBits = ONESTOPBIT;  //사용할 정지비트의 수를 1개로 설정
    dcbSerialParams.Parity = NOPARITY;      //패리티 체계를 사용하지 않음
    //DTR_CONTROL_ENABLE를 설정하면 아두이노가 올바르게 작동합니다.
    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE; //장치가 열리고 계속 켜져있을 때 DTR 라인을 활성화합니다.


    SetCommState(hPort, &dcbSerialParams);  //변경한 정보를 해당 포트에 셋팅합니다.
    PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR); //지정한 포트의 입출력 버퍼를 모두 지웁니다. 송수신중이라면 작업을 중지시킵니다.

    Sleep(2000);

    BYTE buffer[] = "serial communication test!";
    DWORD dwWrite = 0;
    BOOL bWrite = WriteFile(hPort   //데이터를 쓸 포트에 대한 핸들
        , (LPVOID)buffer            //포트에 쓸 데이터
        , sizeof(buffer)            //보낼 데이터의 크기
        , &dwWrite                  //보낸 데이터의 양
        , NULL);                    //비동기 중첩 IO에 대한 구조체, 사용x

    Sleep(1000);

    BYTE RecvBuffer[255] = { 0, };
    DWORD dwRead = 0;
    BOOL bRead = ReadFile(hPort     //데이터를 쓸 포트에 대한 핸들
        , (LPVOID)RecvBuffer        //포트에서 읽어온 데이터를 저장할 버퍼
        , 255                       //얼마나 읽어올지
        , &dwRead                   //읽어오는데 성공한 데이터의 양
        , NULL);                    //비동기 중첩 IO에 대한 구조체, 사용x

    std::cout << (char*)RecvBuffer << std::endl;

    CloseHandle(hPort);
}

void SendString()
{
    //문자열을 시리얼로 보내고 다시 받는 예제
    
    //사용 가능한 com port 열거
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //특정 com port 열기
    Serial comPort(L"COM4");

    //com port로 보낼 데이터
    char data[] = "serial port test!\n";

    std::cout << "send data : " << data << std::endl;

    //com port로 데이터를 씀
    bool bWrite = comPort.WriteData((BYTE*)data, sizeof(data));

    //잠시 대기
    //실제 솔루션에는 이 방법 말고 포트에서 읽을 데이터가 있는지 체크하는 루틴으로...
    Sleep(1000);

    //데이터를 받을 버퍼
    char buf[255] = { 0, };

    //데이터를 읽어들임
    int nReadCount = comPort.ReadData((byte*)buf, 255);

    //읽은 데이터를 출력함
    std::cout << "recv data : " << buf << std::endl;
}

void SendProtocal()
{
    //사용자정의 프로토콜을 시리얼에 보내고 다시 받는 예제
    
    //사용 가능한 com port 열거
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //특정 com port 열기
    Serial comPort(L"COM4");

    // ==========================================================
    // | start sign (1) | op code (2) | data (6) | end sign (1) |
    // ==========================================================
    //테스트를 위한 간단한 패킷 정의
    //시작 사인 : 1바이트
    //명령어 코드 : 2바이트
    //명령어 코드 부가 데이터 6바이트
    //종료 사인 : 1바이트

    constexpr int kBufferSize = 10;
    byte data[kBufferSize] = { 0, };
    byte startSign = '#';
    unsigned short opCode = 3;
    byte opCodeData[6] = { 1, 2, 3, 4, 5, 6};
    byte endSign = '$';

    //시작 사인을 버퍼에 기록
    int idx = 0;
    memcpy_s(&data[idx], sizeof(data) - idx, &startSign, sizeof(startSign));
    idx += sizeof(startSign);

    //명령어 코드를 버퍼에 기록
    memcpy_s(&data[idx], sizeof(data) - idx, &opCode, sizeof(opCode));
    idx += sizeof(opCode);

    //명령어 부가 데이터를 버퍼에 기록
    memcpy_s(&data[idx], sizeof(data) - idx, opCodeData, sizeof(opCodeData));
    idx += sizeof(opCodeData);

    //종료 사인을 버퍼에 기록
    memcpy_s(&data[idx], sizeof(data) - idx, &endSign, sizeof(endSign));
    idx += sizeof(endSign);

    //보낼 데이터들 확인용 출력
    std::cout << "send data" << std::endl;
    std::cout << "start sign : " << data[0] << std::endl;
    std::cout << "opCode : " << *(unsigned short*)&data[1] << std::endl;
    
    std::cout << "opCode Data : ";
    for (int a = 3; a < 9; a++)
    {
        std::cout << std::to_string(data[a]) << ", ";
    }
    std::cout << std::endl;
    
    std::cout << "end sign : " << data[9] << std::endl << std::endl;

    //만들어진 프로토콜이 담긴 버퍼를 시리얼로 전송
    bool bWrite = comPort.WriteData(data, idx);

    //잠시 대기
    //실제 솔루션에는 이 방법 말고 포트에서 읽을 데이터가 있는지 체크하는 루틴으로...
    Sleep(1000);

    //데이터를 받을 버퍼
    byte buf[kBufferSize] = { 0, };

    //데이터를 읽어들임
    int nReadCount = comPort.ReadData(buf, kBufferSize);



    //읽은 데이터를 출력함
    std::cout << "recv data" << std::endl;
    //보낼 데이터들 확인용 출력
    std::cout << "start sign : " << buf[0] << std::endl;
    std::cout << "opCode : " << *(unsigned short*)&buf[1] << std::endl;

    std::cout << "opCode Data : ";
    for (int a = 3; a < 9; a++)
    {
        std::cout << std::to_string(buf[a]) << ", ";
    }
    std::cout << std::endl;

    std::cout << "end sign : " << buf[9] << std::endl;

}

#pragma pack(push, 1)
typedef struct _UserPacket
{
    byte startSign;
    unsigned short opCode;
    byte opCodeData[6];
    byte endSign;
}USER_PACKET;
#pragma pack(pop)

void SendProtocal2()
{
    //사용자정의 프로토콜을 시리얼에 보내고 다시 받는 예제

    //사용 가능한 com port 열거
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //특정 com port 열기
    Serial comPort(L"COM4");

    // ==========================================================
    // | start sign (1) | op code (2) | data (6) | end sign (1) |
    // ==========================================================
    //테스트를 위한 간단한 패킷 정의
    //시작 사인 : 1바이트
    //명령어 코드 : 2바이트
    //명령어 코드 부가 데이터 6바이트
    //종료 사인 : 1바이트

    USER_PACKET data = { 0, };
    byte opCodeData[6] = { 1, 2, 3, 4, 5, 6 };

    data.startSign = '#';
    data.opCode = 3;
    memcpy_s(data.opCodeData, sizeof(data.opCodeData), opCodeData, sizeof(opCodeData));
    data.endSign = '$';

    //보낼 데이터들 확인용 출력
    std::cout << "send data" << std::endl;
    std::cout << "start sign : " << data.startSign << std::endl;
    std::cout << "opCode : " << data.opCode << std::endl;

    std::cout << "opCode Data : ";
    for (int a = 0; a < 6; a++)
    {
        std::cout << std::to_string(data.opCodeData[a]) << ", ";
    }
    std::cout << std::endl;

    std::cout << "end sign : " << data.endSign << std::endl << std::endl;

    //만들어진 프로토콜이 담긴 버퍼를 시리얼로 전송
    bool bWrite = comPort.WriteData((BYTE*)&data, sizeof(data));

    //잠시 대기
    //실제 솔루션에는 이 방법 말고 포트에서 읽을 데이터가 있는지 체크하는 루틴으로...
    Sleep(1000);

    //데이터를 받을 버퍼
    USER_PACKET buf = { 0, };

    //데이터를 읽어들임
    int nReadCount = comPort.ReadData((BYTE*)&buf, sizeof(buf));



    //읽은 데이터를 출력함
    std::cout << "recv data" << std::endl;
    //보낼 데이터들 확인용 출력
    std::cout << "start sign : " << buf.startSign << std::endl;
    std::cout << "opCode : " << buf.opCode << std::endl;

    std::cout << "opCode Data : ";
    for (int a = 0; a < 6; a++)
    {
        std::cout << std::to_string(buf.opCodeData[a]) << ", ";
    }
    std::cout << std::endl;

    std::cout << "end sign : " << buf.endSign << std::endl;

}

int main()
{
    std::cout << "==================SerialTest Test==================" << std::endl;
    SerialTest();

    std::cout << "==================SendString Test==================" << std::endl;
    SendString();

    std::cout << "==================SendProtocal Test==================" << std::endl;
    SendProtocal();

    std::cout << "==================SendProtocal2 Test==================" << std::endl;
    SendProtocal2();

	return 0;
}