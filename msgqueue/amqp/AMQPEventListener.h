#include "../Listener.h"
#include "../Event.h"

#include "../../libfxtract/include/Processor.h"
#include "../../logging/include/LoggingFacility.h"

#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

#include <amqpcpp/linux_tcp/tcpchannel.h>

const std::string eventNameHeader = "x-event-name";
const std::string eventName = "processPlanningStarted";
const std::string QueueName = "PROCESSPLANNING";
struct AMQPEventListener : public EventListener
{
    std::shared_ptr<AMQP::TcpConnection> connection;
    std::shared_ptr<EventEmitter> eventEmitter;
    std::string exchange, queue;
    EventMapper mapper;
    Logger loggingService;

public:
    AMQPEventListener(std::shared_ptr<AMQP::TcpConnection> &connection,
                      std::shared_ptr<EventEmitter> eventEmitter,
                      std::string exchange, std::string queue, Logger loggingService);

    void setUp();
    void Listen(std::vector<std::string> &list,
                std::function<std::shared_ptr<Event>(EventPtr event, Logger loggingService)> &);
};
