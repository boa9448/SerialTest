#include <Windows.h>
#include <iostream>

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

int main()
{
    SerialTest();

	return 0;
}