/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "pbop/PipeConnection.h"

//https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-client

#include <Windows.h>

namespace pbop
{
  std::string GetErrorDesription(DWORD code)
  {
    const DWORD error_buffer_size = 10240;
    char error_buffer[error_buffer_size] = { 0 };
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      code,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      error_buffer,
      error_buffer_size - 1,
      NULL);
    std::string error_desc = error_buffer;
    return error_desc;
  }

  PipeConnection::PipeConnection() : hPipe_(INVALID_HANDLE_VALUE)
  {
  }

  PipeConnection::~PipeConnection()
  {
    if (hPipe_ != INVALID_HANDLE_VALUE)
    {
      // Flush the pipe to allow the client to read the pipe's contents 
      // before disconnecting. Then disconnect the pipe, and close the 
      // handle to this pipe instance.
      FlushFileBuffers(hPipe_); 
      DisconnectNamedPipe(hPipe_); 

      CloseHandle(hPipe_);
    }
    hPipe_ = INVALID_HANDLE_VALUE;
  }

  void PipeConnection::Assign(HANDLE hPipe)
  {
    //Close existing connection
    if (hPipe_ != INVALID_HANDLE_VALUE)
    {
      CloseHandle(hPipe);
      hPipe = INVALID_HANDLE_VALUE;
    }

    hPipe_ = hPipe;
  }

  Status PipeConnection::Connect(const char * pipe_name)
  {
    HANDLE hPipe = NULL;
    BOOL fSuccess = FALSE; 

    // Try to open a named pipe; wait for it, if necessary. 
    while (1) 
    { 
      hPipe = CreateFile( 
          pipe_name,      // pipe name 
          GENERIC_READ |  // read and write access 
          GENERIC_WRITE, 
          0,              // no sharing 
          NULL,           // default security attributes
          OPEN_EXISTING,  // opens existing pipe 
          0,              // default attributes 
          NULL);          // no template file 
 
      // Break if the pipe handle is valid. 
      if (hPipe != INVALID_HANDLE_VALUE) 
          break; 
 
      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
      DWORD wLastError = GetLastError();
      if (wLastError != ERROR_PIPE_BUSY) 
      {
        // Error, unable to connect
        std::string error_description = GetErrorDesription(wLastError);
        return Status(STATUS_CODE_PIPE_ERROR, error_description);
      }
 
      // All pipe instances are busy, so wait for 20 seconds. 
      if (!WaitNamedPipe(pipe_name, 20000)) 
      {
        std::string error_description = "Could not open pipe: 20 second wait timed out.";
        return Status(STATUS_CODE_PIPE_ERROR, error_description);
      } 
    } 
 
    // The pipe connected; change to message-read mode. 
    DWORD dwMode = PIPE_READMODE_MESSAGE; 
    fSuccess = SetNamedPipeHandleState( 
      hPipe,    // pipe handle 
      &dwMode,  // new pipe mode 
      NULL,     // don't set maximum bytes 
      NULL);    // don't set maximum time 
    if ( !fSuccess) 
    {
      std::string error_description = std::string("SetNamedPipeHandleState failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    // Connection succesful
    hPipe_ = hPipe;
    name_ = pipe_name;

    return Status::OK;
  }

  Status PipeConnection::Write(const std::string & buffer)
  {
    if (hPipe_ == INVALID_HANDLE_VALUE)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe is invalid.");

    DWORD wBytesWritten = 0;
    BOOL fSuccess = WriteFile( 
      hPipe_,                 // pipe handle 
      buffer.data(),          // message 
      buffer.size(),          // message length 
      &wBytesWritten,         // bytes written 
      NULL);                  // not overlapped 

    if (!fSuccess || wBytesWritten != buffer.size())
    {
      std::string error_description = std::string("WriteFile to pipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

  Status PipeConnection::Read(std::string & buffer)
  {
    buffer.clear();

    if (hPipe_ == INVALID_HANDLE_VALUE)
      return Status(STATUS_CODE_PIPE_ERROR, "Pipe is invalid.");

    BOOL fSuccess = FALSE;
    static const size_t BUFFER_SIZE = 10240;
    char tmp[BUFFER_SIZE];
    do 
    { 
      // Read from the pipe.
      DWORD wBytesReaded = 0;
      fSuccess = ReadFile( 
          hPipe_,       // pipe handle 
          tmp,          // buffer to receive reply 
          BUFFER_SIZE,  // size of buffer 
          &wBytesReaded,// number of bytes read 
          NULL);        // not overlapped 

      if (fSuccess)
        buffer.append(tmp, wBytesReaded);

      if ( !fSuccess && GetLastError() != ERROR_MORE_DATA )
        break; //nothing more to read
 
      //received message is incomplete.
      //loop again

    } while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

    if (!fSuccess)
    {
      std::string error_description = std::string("ReadFile from pipe failed: ") + GetErrorDesription(GetLastError());
      return Status(STATUS_CODE_PIPE_ERROR, error_description);
    }

    return Status::OK;
  }

}; //namespace pbop
