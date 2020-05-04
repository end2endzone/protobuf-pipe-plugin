#include "calculator.pipe.pb.h"
#include "libProtobufPipePlugin/PipeMessages.pb.h"

using namespace ::libProtobufPipePlugin;

namespace calculus
{
namespace CalculatorService
{
  Client::Client(Connection * connection) : connection_(connection)
  {
  }

  Client::~Client()
  {
    if (connection_)
      delete connection_;
    connection_ = NULL;
  }

  const std::string & Client::GetPackageName() const
  {
    static const std::string package = "calculus";
    return package;
  }

  const std::string & Client::GetServiceName() const
  {
    static const std::string service = "CalculatorService";
    return service;
  }

  const std::vector<std::string> & Client::GetFunctionIdentifiers() const
  {
    // Not supported by client
    static const std::vector<std::string> functions;
    return functions;
  }

  Status Client::DispatchMessage(const size_t & index, const std::string & input, std::string & output)
  {
    static const Status status(STATUS_CODE_NOT_IMPLEMENTED_ERROR, "CalculatorService client cannot dispatch messages");
    return status;
  }

  Status Client::Add(const AddRequest & request, AddResponse & response)
  {
    FunctionIdentifier * function_identifier = new FunctionIdentifier();
    if (!function_identifier)
      return Status::BuildOutOfMemoryStatus(__FUNCTION__);
    function_identifier->set_allocated_package(new std::string(GetPackageName()));
    function_identifier->set_allocated_service(new std::string(GetServiceName()));
    function_identifier->set_allocated_function_name(new std::string("Add"));

    ClientRequest client_message;
    client_message.set_allocated_function_identifier(function_identifier);
    
    // Serialize the request to a string and add to the client_message
    std::string * request_buffer = new std::string();
    if (!request_buffer)
      return Status::BuildOutOfMemoryStatus(__FUNCTION__);
    bool success = request.SerializeToString(request_buffer);
    if (!success)
      return Status::BuildSerializationStatus(__FUNCTION__, request);
    client_message.set_allocated_request_buffer(request_buffer);

    // Serialize the client_message ready for sending to the connection
    std::string write_buffer;
    success = client_message.SerializeToString(&write_buffer);
    if (!success)
      return Status::BuildSerializationStatus(__FUNCTION__, request);

    // Send
    Status status = connection_->Write(write_buffer);
    if (!status.Success())
      return status;

    // Wait for a response.
    std::string read_buffer;
    status = connection_->Read(read_buffer);
    if (!status.Success())
      return status;

    // Derialize the ServerResponse
    ServerResponse server_response;
    success = server_response.ParseFromString(read_buffer);
    if (!success)
      return Status::BuildDeserializationStatus(__FUNCTION__, server_response);

    // Read server status
    if (!server_response.has_status())
      return Status::BuildMissingFieldStatus(__FUNCTION__, "status", server_response);

    // Convert ServerStatus to Status
    status.SetCode( static_cast<StatusCode>(server_response.status().code()) );
    status.SetMessage(server_response.status().description());
    if (!status.Success())
      return status;

    // Try to deserialize AddResponse
    response.ParseFromString(server_response.response_buffer());
    if (!success)
      return Status::BuildDeserializationStatus(__FUNCTION__, response);

    // Success
    return Status::OK;
  }

  ServerStub::ServerStub()
  {
    functions_.push_back("Add");
  }

  ServerStub::~ServerStub()
  {
  }

  const std::string & ServerStub::GetPackageName() const
  {
    static const std::string package = "calculus";
    return package;
  }

  const std::string & ServerStub::GetServiceName() const
  {
    static const std::string service = "CalculatorService";
    return service;
  }

  const std::vector<std::string> & ServerStub::GetFunctionIdentifiers() const
  {
    return functions_;
  }

  libProtobufPipePlugin::Status ServerStub::DispatchMessage(const size_t & index, const std::string & input, std::string & output)
  {
    bool success = false;
    Status status;

    switch(index)
    {
    case 0:
      {
        AddRequest request;
        AddResponse response;
        success = request.ParseFromString(input);
        if (!success)
          return Status::BuildDeserializationStatus(__FUNCTION__, request);
        status = this->Add(request, response);
        if (!status.Success())
          return status;
        bool serialized = response.SerializeToString(&output);
        if (!serialized)
          status = Status::BuildDeserializationStatus(__FUNCTION__, response);
      }
      break;
    default:
      //Not implemented
      std::string error_message = "Function at index " + std::to_string((unsigned long long)index) + " is not implemented.";
      status.SetCode(STATUS_CODE_NOT_IMPLEMENTED_ERROR);
      status.SetMessage(error_message);
    };

    return status;
  }

  //CalculatorService service implementation
  libProtobufPipePlugin::Status ServerStub::Add(const AddRequest & request, AddResponse & response)
  {
    static const Status status = Status::BuildNotImplementedStatus(__FUNCTION__);
    return status;
  }

}; //namespace CalculatorService
}; //namespace calculus