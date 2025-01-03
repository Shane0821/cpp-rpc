#include <llbc.h>
#include <spsc_queue.h>

// Connection management component
class RpcConnComp : public llbc::LLBC_Component {
   public:
    RpcConnComp();
    virtual ~RpcConnComp() {}

    virtual bool OnInit(bool &initFinished);
    virtual void OnDestroy(bool &destroyFinished);
    virtual void OnUpdate();

    // monitor network event
    virtual void OnSessionCreate(const llbc::LLBC_SessionInfo &sessionInfo);
    virtual void OnSessionDestroy(const llbc::LLBC_SessionDestroyInfo &destroyInfo);
    virtual void OnAsyncConnResult(const llbc::LLBC_AsyncConnResult &result);
    virtual void OnUnHandledPacket(const llbc::LLBC_Packet &packet);
    virtual void OnProtoReport(const llbc::LLBC_ProtoReport &report);

    // push send packet
    int PushSendPacket(llbc::LLBC_Packet *sendPacket) noexcept;
    // pop recv packet
    int PopRecvPacket(llbc::LLBC_Packet *&recvPacket) noexcept;
    // callback when recv packet
    void OnRecvPacket(llbc::LLBC_Packet &packet) noexcept;

    static constexpr int MAX_QUEUE_SIZE = 4096;

   private:
    SPSCQueue<llbc::LLBC_Packet *, MAX_QUEUE_SIZE> sendQueue_;
    SPSCQueue<llbc::LLBC_Packet *, MAX_QUEUE_SIZE> recvQueue_;
};