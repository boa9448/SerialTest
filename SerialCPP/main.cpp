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
        //COM1 �� ���� ���ڿ��� ����
        std::wstring str = L"COM" + std::to_wstring(i);

        //�ش� ��ġ�� ��� �������� üũ
        DWORD test = QueryDosDevice(str.c_str(), lpTargetPath, MAX_PATH);

        if (test != 0)
        {
            //��� �����ϴٸ� ����Ʈ�� �߰�
            comList.push_back(str);
        }
    }

    return comList;
}

void SerialTest()
{
    LPCWSTR lpPortName = L"COM4";
    HANDLE hPort = CreateFileW(lpPortName,  //��Ʈ �̸�
        GENERIC_READ | GENERIC_WRITE,       //��Ʈ�� READ �� WRITE�� �����ϰ� �ɼ� ����
        0,                                  //�ش� ��Ʈ�� �������� ����
        NULL,                               //���� �Ӽ��� �⺻ ���ȼӼ�����
        OPEN_EXISTING,                      //CreateFile�� ������ �ƴ� ��ġ�� ����ҋ��� �׻� �� �ɼ�
        FILE_ATTRIBUTE_NORMAL,              //�Ϲ� �Ӽ�
        NULL);                              //���ø��ڵ��� �����������


    DCB dcbSerialParams = { 0 };            //���� ��� ��ġ�� ���� ���� ����ü
    GetCommState(hPort, &dcbSerialParams);  //���� ��Ʈ�� ���� ������ ������


    dcbSerialParams.BaudRate = CBR_9600;    //baud rate�� 9600���� ����
    dcbSerialParams.ByteSize = 8;           //byte ũ�⸦ 8�� ����
    dcbSerialParams.StopBits = ONESTOPBIT;  //����� ������Ʈ�� ���� 1���� ����
    dcbSerialParams.Parity = NOPARITY;      //�и�Ƽ ü�踦 ������� ����
    //DTR_CONTROL_ENABLE�� �����ϸ� �Ƶ��̳밡 �ùٸ��� �۵��մϴ�.
    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE; //��ġ�� ������ ��� �������� �� DTR ������ Ȱ��ȭ�մϴ�.


    SetCommState(hPort, &dcbSerialParams);  //������ ������ �ش� ��Ʈ�� �����մϴ�.
    PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR); //������ ��Ʈ�� ����� ���۸� ��� ����ϴ�. �ۼ������̶�� �۾��� ������ŵ�ϴ�.

    Sleep(2000);

    BYTE buffer[] = "serial communication test!";
    DWORD dwWrite = 0;
    BOOL bWrite = WriteFile(hPort   //�����͸� �� ��Ʈ�� ���� �ڵ�
        , (LPVOID)buffer            //��Ʈ�� �� ������
        , sizeof(buffer)            //���� �������� ũ��
        , &dwWrite                  //���� �������� ��
        , NULL);                    //�񵿱� ��ø IO�� ���� ����ü, ���x

    Sleep(1000);

    BYTE RecvBuffer[255] = { 0, };
    DWORD dwRead = 0;
    BOOL bRead = ReadFile(hPort     //�����͸� �� ��Ʈ�� ���� �ڵ�
        , (LPVOID)RecvBuffer        //��Ʈ���� �о�� �����͸� ������ ����
        , 255                       //�󸶳� �о����
        , &dwRead                   //�о���µ� ������ �������� ��
        , NULL);                    //�񵿱� ��ø IO�� ���� ����ü, ���x

    std::cout << (char*)RecvBuffer << std::endl;

    CloseHandle(hPort);
}

void SendString()
{
    //���ڿ��� �ø���� ������ �ٽ� �޴� ����
    
    //��� ������ com port ����
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //Ư�� com port ����
    Serial comPort(L"COM4");

    //com port�� ���� ������
    char data[] = "serial port test!\n";

    std::cout << "send data : " << data << std::endl;

    //com port�� �����͸� ��
    bool bWrite = comPort.WriteData((BYTE*)data, sizeof(data));

    //��� ���
    //���� �ַ�ǿ��� �� ��� ���� ��Ʈ���� ���� �����Ͱ� �ִ��� üũ�ϴ� ��ƾ����...
    Sleep(1000);

    //�����͸� ���� ����
    char buf[255] = { 0, };

    //�����͸� �о����
    int nReadCount = comPort.ReadData((byte*)buf, 255);

    //���� �����͸� �����
    std::cout << "recv data : " << buf << std::endl;
}

void SendProtocal()
{
    //��������� ���������� �ø��� ������ �ٽ� �޴� ����
    
    //��� ������ com port ����
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //Ư�� com port ����
    Serial comPort(L"COM4");

    // ==========================================================
    // | start sign (1) | op code (2) | data (6) | end sign (1) |
    // ==========================================================
    //�׽�Ʈ�� ���� ������ ��Ŷ ����
    //���� ���� : 1����Ʈ
    //��ɾ� �ڵ� : 2����Ʈ
    //��ɾ� �ڵ� �ΰ� ������ 6����Ʈ
    //���� ���� : 1����Ʈ

    constexpr int kBufferSize = 10;
    byte data[kBufferSize] = { 0, };
    byte startSign = '#';
    unsigned short opCode = 3;
    byte opCodeData[6] = { 1, 2, 3, 4, 5, 6};
    byte endSign = '$';

    //���� ������ ���ۿ� ���
    int idx = 0;
    memcpy_s(&data[idx], sizeof(data) - idx, &startSign, sizeof(startSign));
    idx += sizeof(startSign);

    //��ɾ� �ڵ带 ���ۿ� ���
    memcpy_s(&data[idx], sizeof(data) - idx, &opCode, sizeof(opCode));
    idx += sizeof(opCode);

    //��ɾ� �ΰ� �����͸� ���ۿ� ���
    memcpy_s(&data[idx], sizeof(data) - idx, opCodeData, sizeof(opCodeData));
    idx += sizeof(opCodeData);

    //���� ������ ���ۿ� ���
    memcpy_s(&data[idx], sizeof(data) - idx, &endSign, sizeof(endSign));
    idx += sizeof(endSign);

    //���� �����͵� Ȯ�ο� ���
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

    //������� ���������� ��� ���۸� �ø���� ����
    bool bWrite = comPort.WriteData(data, idx);

    //��� ���
    //���� �ַ�ǿ��� �� ��� ���� ��Ʈ���� ���� �����Ͱ� �ִ��� üũ�ϴ� ��ƾ����...
    Sleep(1000);

    //�����͸� ���� ����
    byte buf[kBufferSize] = { 0, };

    //�����͸� �о����
    int nReadCount = comPort.ReadData(buf, kBufferSize);



    //���� �����͸� �����
    std::cout << "recv data" << std::endl;
    //���� �����͵� Ȯ�ο� ���
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
    //��������� ���������� �ø��� ������ �ٽ� �޴� ����

    //��� ������ com port ����
    auto comList = SelectComPort();
    for (auto& com : comList)
    {
        std::wcout << com << std::endl;
    }

    //Ư�� com port ����
    Serial comPort(L"COM4");

    // ==========================================================
    // | start sign (1) | op code (2) | data (6) | end sign (1) |
    // ==========================================================
    //�׽�Ʈ�� ���� ������ ��Ŷ ����
    //���� ���� : 1����Ʈ
    //��ɾ� �ڵ� : 2����Ʈ
    //��ɾ� �ڵ� �ΰ� ������ 6����Ʈ
    //���� ���� : 1����Ʈ

    USER_PACKET data = { 0, };
    byte opCodeData[6] = { 1, 2, 3, 4, 5, 6 };

    data.startSign = '#';
    data.opCode = 3;
    memcpy_s(data.opCodeData, sizeof(data.opCodeData), opCodeData, sizeof(opCodeData));
    data.endSign = '$';

    //���� �����͵� Ȯ�ο� ���
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

    //������� ���������� ��� ���۸� �ø���� ����
    bool bWrite = comPort.WriteData((BYTE*)&data, sizeof(data));

    //��� ���
    //���� �ַ�ǿ��� �� ��� ���� ��Ʈ���� ���� �����Ͱ� �ִ��� üũ�ϴ� ��ƾ����...
    Sleep(1000);

    //�����͸� ���� ����
    USER_PACKET buf = { 0, };

    //�����͸� �о����
    int nReadCount = comPort.ReadData((BYTE*)&buf, sizeof(buf));



    //���� �����͸� �����
    std::cout << "recv data" << std::endl;
    //���� �����͵� Ȯ�ο� ���
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