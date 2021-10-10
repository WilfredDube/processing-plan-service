#include "AMQPEventEmitter.h"
#include "../contracts/ProcessPlanningComplete.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <amqpcpp/linux_tcp/tcpchannel.h>

AMQPEventEmitter::AMQPEventEmitter(std::shared_ptr<AMQP::TcpConnection> connection,
                                   std::string exchange, Logger loggingService)
    : connection{connection}, exchange{exchange}, loggingService{loggingService}
{
    setUp();
}

void AMQPEventEmitter::setUp()
{
    AMQP::TcpChannel channel(connection.get());
    connection->heartbeat();

    arguments["x-event-name"] = "processPlanningComplete";

    channel.declareExchange(exchange, AMQP::topic, AMQP::durable, arguments);
}

bool AMQPEventEmitter::Emit(std::shared_ptr<Event> event)
{
    json jsonBody;
    AMQP::TcpChannel channel(connection.get());
    channel.startTransaction();

    jsonBody = event->toJson();

    std::string S = jsonBody.dump();

    AMQP::Envelope envelope(S.c_str(), S.size());

    envelope.setHeaders(arguments);

    auto res = channel.publish(exchange, event->EventName(), envelope);

    channel.commitTransaction()
        .onSuccess([&]()
                   {
                       // all messages were successfully published })
                       loggingService->writeInfoEntry(__FILE__, __LINE__, "Successfully published");
                   })
        .onError([&](const char *message)
                 {
                     // none of the messages were published
                     // now we have to do it all over again
                     loggingService->writeErrorEntry(__FILE__, __LINE__, "Failed to publish" + std::string(message));
                 });

    return res;
}
