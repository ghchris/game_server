#ifndef FRAMEWORK_GAME_PRIVATEROOM_H_
#define FRAMEWORK_GAME_PRIVATEROOM_H_

#include "roombase.h"

class CardGenerator;
class PrivateRoomImpl;
class PrivateRoom:public RoomBase
{
public:
    //0:��׳,1:����,2:���ܺ�,3:ǿ�ƽ�ɢ,4:���ں�,5:���׺�,6:������,7:���ϻ�,8:������
    enum class HuType{CHOUZHUANG,ZIMOHU,QINAGGANGHU,NOTPLAYED,DIANPAOHU,HAIDIHU,HAIDIPAO,GANGHU,GANGPAO};
    //0:�ƾ��Ѵ���,1:��ɢ��ʱ,2:������ͬ���ɢ
    enum class DisbandType {NUM_NULL,TIME_OUT,ALL_AGREE,NOT_START};
public:
    PrivateRoom(std::uint32_t id, std::string type);
    virtual ~PrivateRoom();

    virtual std::int32_t OnMessage(std::shared_ptr<Agent > player, assistx2::Stream * packet);

    virtual std::int32_t Enter(std::shared_ptr<Agent > player);
    virtual std::int32_t Leave(std::shared_ptr<Agent > player);
    virtual std::int32_t Disband();

    virtual void StringToRoomData(const std::string& str);

    void set_played_num(std::int32_t num);
    void set_proxy_uid(const uid_type mid);
    const uid_type proxy_uid() const;
protected:
    std::shared_ptr<CardGenerator> card_generator();
    const std::int32_t banker_seatno() const;
    const std::int32_t played_num() const;
    void set_banker_seatno(const std::int32_t seatno);
protected:
    virtual void OnGameStart();
    virtual void OnGameOver();
    virtual void OnDisbandRoom(DisbandType type);
    virtual void OnReConect(std::shared_ptr<Agent > player);
    virtual void OnTimer(boost::shared_ptr<assistx2::timer2::TimerContext > context);
private:
    friend class PrivateRoomImpl;
    std::unique_ptr< PrivateRoomImpl > pImpl_;
};

#endif //FRAMEWORK_GAME_PRIVATEROOM_H_
