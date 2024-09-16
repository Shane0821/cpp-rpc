#include <llbc.h>
#include <spsc_queue.h>

// 连接管理组件
class ConnComp : public llbc::LLBC_Component {
   public:
    ConnComp();
    virtual ~ConnComp() {}

    virtual bool OnInit(bool &initFinished);
    virtual void OnDestroy(bool &destroyFinished);
    virtual void OnUpdate();

    // monitor network event
    virtual void OnSessionCreate(const llbc::LLBC_SessionInfo &sessionInfo);
    virtual void OnSessionDestroy(const llbc::LLBC_SessionDestroyInfo &destroyInfo);
    virtual void OnAsyncConnResult(const llbc::LLBC_AsyncConnResult &result);
    virtual void OnUnHandledPacket(const llbc::LLBC_Packet &packet);
    virtual void OnProtoReport(const llbc::LLBC_ProtoReport &report);

    // send packet
    int PushPacket(llbc::LLBC_Packet &sendPacket);
    // recv packet
    int PopPacket(llbc::LLBC_Packet &recvPacket);
    // callback when recv packet
    void OnRecvPacket(llbc::LLBC_Packet &packet);

    static constexpr int MAX_QUEUE_SIZE = 1024;

   private:
    SPSCQueue<llbc::LLBC_Packet, MAX_QUEUE_SIZE> sendQueue_;
    SPSCQueue<llbc::LLBC_Packet, MAX_QUEUE_SIZE> recvQueue_;
};