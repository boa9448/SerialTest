#include "Serial.h"

Serial::Serial(LPCWSTR lpPortName) : m_dwErrors(0)
    , m_Status()
{
    //We're not yet connected
    this->m_bConnected = false;

    //Try to connect to the given port throuh CreateFile
    this->m_hSerial = CreateFileW(lpPortName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    //Check if the connection was successfull
    if (this->m_hSerial == INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {

            //Print Error if neccessary
            wprintf(L"ERROR: Handle was not attached. Reason: %s not available.\n", lpPortName);

        }
        else
        {
            wprintf(L"ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = { 0 };

        //Try to get the current
        if (!GetCommState(this->m_hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            wprintf(L"failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate = CBR_9600;
            dcbSerialParams.ByteSize = 8;
            dcbSerialParams.StopBits = ONESTOPBIT;
            dcbSerialParams.Parity = NOPARITY;
            //Setting the DTR to Control_Enable ensures that the Arduino is properly
            //reset upon establishing a connection
            dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

            //Set the parameters and check for their proper application
            if (!SetCommState(this->m_hSerial, &dcbSerialParams))
            {
                printf("ALERT: Could not set Serial Port parameters");
            }
            else
            {
                //If everything went fine we're connected
                this->m_bConnected = true;
                //Flush any remaining characters in the buffers 
                PurgeComm(this->m_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
                //We wait 2s as the arduino board will be reseting
                Sleep(SERIAL_WAIT_TIME);
            }
        }
    }

}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if (this->m_bConnected)
    {
        //We're no longer connected
        this->m_bConnected = false;
        //Close the serial handler
        CloseHandle(this->m_hSerial);
    }
}

int Serial::ReadData(BYTE* lpBuffer, UINT nBufSize)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    UINT toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->m_hSerial, &this->m_dwErrors, &this->m_Status);

    //Check if there is something to read
    if (this->m_Status.cbInQue > 0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if (this->m_Status.cbInQue > nBufSize)
        {
            toRead = nBufSize;
        }
        else
        {
            toRead = this->m_Status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if (ReadFile(this->m_hSerial, lpBuffer, toRead, &bytesRead, NULL))
        {
            return bytesRead;
        }

    }

    //If nothing has been read, or that an error was detected return 0
    return 0;

}


bool Serial::WriteData(BYTE* lpBuffer, UINT nWriteSize)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if (!WriteFile(this->m_hSerial, (void*)lpBuffer, nWriteSize, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->m_hSerial, &this->m_dwErrors, &this->m_Status);

        return FALSE;
    }
    else
        return TRUE;
}

bool Serial::IsConnected()
{
    //Simply return the connection status
    return this->m_bConnected;
}