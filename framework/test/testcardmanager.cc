#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <json_spirit_reader_template.h>
#include <json_spirit_writer_template.h>
#include "testcardmanager.h"
#include <string>
#include <fstream>
#include <algorithm>
#include "common.h"

const std::string JSON_FILE_NAME = "testcard.json";

class TestCardManagerImpl
{
public:
    TestCardManagerImpl();
    ~TestCardManagerImpl();
    void CreateCustomCard();
public:
    std::string json_string_;
    Cards cards_;
};

TestCardManager::TestCardManager():
  pImpl_(new TestCardManagerImpl)
{

}

TestCardManager::~TestCardManager()
{

}

void TestCardManager::Update()
{
    std::ifstream ifs;
    ifs.open(JSON_FILE_NAME);
    if (ifs.is_open() == false)
    {
        DLOG(ERROR) << "TestCardManager::Update File Open Failed!";
        return;
    }

    ifs.seekg(0, ifs.end);
    auto len = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    auto buffer = new char[len];
    
    ifs.read(buffer, len);
    
    pImpl_->json_string_ = std::string(buffer,len);

    delete[] buffer;

    pImpl_->CreateCustomCard();
}

void TestCardManager::GetCustomCard(Cards& cards)
{
    Cards clone(cards);
    for (auto iter : pImpl_->cards_)
    {
        auto it = std::find_if(clone.begin(), clone.end(), [iter](const std::shared_ptr<Card> card) {
            return iter->getFace() == card->getFace() && iter->getType() == card->getType();
        });
        if (it != clone.end())
        {
            clone.erase(it);
        }
    }
    cards.clear();
    std::merge(pImpl_->cards_.begin(), pImpl_->cards_.end(), clone.begin(), clone.end(), std::back_inserter(cards));
}

void TestCardManagerImpl::CreateCustomCard()
{
    auto res = Common::StringToJson(json_string_, json_spirit::obj_type);
    if (res.type() != json_spirit::obj_type)
    {
        DLOG(ERROR) << "TestCardManager::MakeTestCard File format Failed!";
        return;
    }
    std::vector<std::string> cards;
    auto obj = res.get_obj();
    for (auto iter : obj)
    {
        auto array = iter.value_.get_array();
        for (auto it : array)
        {
            cards.push_back(it.get_str());
        }
    }
    for (auto iter = cards.rbegin(); iter != cards.rend(); iter++)
    {
        auto card = CardFactory::MakeMajiangCard(*iter);
        if (card == nullptr)
        {
            DLOG(ERROR) << "TestCardManager::MakeTestCard card Failed! card:=" << *iter;
            cards_.clear();
            return;
        }
        else
        {
            cards_.push_back(card);
        }
    }
}

TestCardManagerImpl::TestCardManagerImpl()
{
}

TestCardManagerImpl::~TestCardManagerImpl()
{
}