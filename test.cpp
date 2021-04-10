#include <Windows.h>
#include <iostream>

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

int main()
{
    SerialTest();

	return 0;
}