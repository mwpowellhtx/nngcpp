#ifndef CPPNNG_PROT_BUS_H
#define CPPNNG_PROT_BUS_H

#include "../../core/socket.h"

namespace nng {

    namespace protocol {

        namespace v0 {

            class _BusSocket : public _Socket {
            public:

                _BusSocket();

                virtual ~_BusSocket();

            public:

                virtual _OptionReaderWriter* const GetOptions() override;

                // TODO: TBD: may want to comprehend nng's NNG_MAXADDRLEN at some level... expose as a static constant, for instance, bare minimum
                virtual void Listen(const std::string& addr, flag_type flags = flag_none) override;
                virtual void Listen(const std::string& addr, _Listener* const l, flag_type flags = flag_none) override;

                virtual void Dial(const std::string& addr, flag_type flags = flag_none) override;
                virtual void Dial(const std::string& addr, _Dialer* const d, flag_type flags = flag_none) override;

                virtual void Close() override;
                //virtual void Shutdown() override;

                virtual bool HasOne() const override;

            public:

                virtual void Send(binary_message& m, flag_type flags = flag_none) override;

                virtual void Send(const buffer_vector_type& buf, flag_type flags = flag_none) override;
                virtual void Send(const buffer_vector_type& buf, size_type sz, flag_type flags = flag_none) override;

                virtual void SendAsync(const basic_async_service* const svcp) override;

                //// TODO: TBD: will cross the IReceiver bridge in a bit...
                //virtual std::unique_ptr<binary_message> Receive(flag_type flags = flag_none) override;
                //virtual bool TryReceive(binary_message* const bmp, flag_type flags = flag_none) override;

                //virtual buffer_vector_type Receive(size_type& sz, flag_type flags = flag_none) override;
                //virtual bool TryReceive(buffer_vector_type* const bufp, size_type& sz, flag_type flags = flag_none) override;

                //virtual void ReceiveAsync(basic_async_service* const svcp) override;
            };

            typedef _BusSocket bus_socket;
        }

        typedef v0::_BusSocket _LatestBusSocket;
    }
}

#endif // CPPNNG_PROT_BUS_H
