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

class GreeterServiceImpl : public Greeter::ServerStub
{
public:
  GreeterServiceImpl() {}
  virtual ~GreeterServiceImpl() {}

  Status SayHello(const SayHelloRequest & request, SayHelloResponse & response)
  {
    response.set_message("Hello " + request.name());
    return Status::OK;
  }

  Status SayGoodbye(const SayGoodbyeRequest & request, SayGoodbyeResponse & response)
  {
    response.set_message("Hello " + request.name());
    return Status::OK;
  }
};

int main(int argc, char* argv[])
{
  printf("Launching server...\n");

  GreeterServiceImpl * impl = new GreeterServiceImpl();

  Server server;
  server.RegisterService(impl);
  Status status = server.Run(kPipeName);
  if (!status.Success())
  {
    printf("Error in main(): %d, %s\n", status.GetCode(), status.GetMessage().c_str());
    return status.GetCode();
  }
  return 0;
}