#include <stdio.h>

#ifdef _WIN32
//google/protobuf/io/coded_stream.h(869): warning C4800: 'google::protobuf::internal::Atomic32' : forcing value to bool 'true' or 'false' (performance warning)
//google/protobuf/wire_format_lite.h(863): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/wire_format_lite.h(874): warning C4146: unary minus operator applied to unsigned type, result still unsigned
//google/protobuf/generated_message_util.h(160): warning C4800: 'const google::protobuf::uint32' : forcing value to bool 'true' or 'false' (performance warning)
__pragma( warning(push) )
__pragma( warning(disable: 4800))
__pragma( warning(disable: 4146))
#endif //_WIN32

#include "greetings.pb.h"
#include "greetings.pipe.pb.h"
#include "libProtobufPipePlugin/Server.h"

#ifdef _WIN32
__pragma( warning(pop) )
#endif //_WIN32

using namespace libProtobufPipePlugin;
using namespace greetings;

static const char * kPipeName = "\\\\.\\pipe\\greetingspipe";

int main(int argc, char* argv[])
{
  printf("Launching client...\n");

  PipeConnection * connection = new PipeConnection();
  Status status = connection->Connect(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  Greeter::Client client(connection);

  SayHelloRequest request;
  SayHelloResponse response;

  request.set_name("bob");

  printf("%s says hello.\n", request.name().c_str());
  status = client.SayHello(request, response);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }

  printf("Message from server: %s\n", response.message().c_str());

  return 0;
}