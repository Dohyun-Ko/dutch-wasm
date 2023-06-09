#include "receivePage.h"

#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

#include "../../components/style/Style.h"
#include "../../components/state/State.cpp"
#include "../../components/button/Button.h"
#include "../../globalState/userState/userState.h"
#include "../../globalState/receiveDutchState/receiveDutchState.h"
#include "../../utils/Constants.h"

using json = nlohmann::json;

ReceivePageStates::ReceivePageStates()
{
    sendUserList = new State<string>("");
    targetAmount = new State<string>("");
    usernameText = new State<string>("");
    dutchUUID = "";
    dutchType = "";
}

ReceivePageStates::~ReceivePageStates()
{
    delete sendUserList;
    delete targetAmount;
    delete usernameText;
}

State<string> *ReceivePageStates::getSendUserList()
{
    return sendUserList;
}

State<string> *ReceivePageStates::getTargetAmount()
{
    return targetAmount;
}

State<string> *ReceivePageStates::getUsernameText()
{
    return usernameText;
}

string ReceivePageStates::getDutchUUID()
{
    return dutchUUID;
}

void ReceivePageStates::setDutchUUID(string uuid)
{
    dutchUUID = uuid;
}

string ReceivePageStates::getDutchType()
{
    return dutchType;
}

void ReceivePageStates::setDutchType(string type)
{
    dutchType = type;
}

ReceivePage *ReceivePage::instance = nullptr;
ReceivePageStates *ReceivePage::dutchList[6] = {new ReceivePageStates(), new ReceivePageStates(), new ReceivePageStates(), new ReceivePageStates(), new ReceivePageStates(), new ReceivePageStates()};
State<vector<string>> *ReceivePage::dutchUUIDList = ReceiveDutchState::getInstance()->getReceiveUUIDs();
int ReceivePage::currentPageNumber = 0;

ReceivePage::ReceivePage() : Element("div")
{
    currentPageNumber = 0;
    for (size_t i = 0; i< 6; i++){
        dutchList[i]->getSendUserList()->setState("");
        dutchList[i]->getTargetAmount()->setState("");
        dutchList[i]->setDutchUUID("");
    }

    ReceivePage::getStyle()
        .setWidth("100%")
        .setHeight("100%")
        .setDisplay("flex")
        .setFlexDirection("row")
        .setAlignItems("center")
        .setJustifyContent("center");
    prevButton = new Button(new State<string>("⬅"));
    nextButton = new Button(new State<string>("➡"));

    dutchItemContainer = std::make_unique<Element>("div");
    dutchItemContainer->getStyle()
        .setWidth("70%")
        .setHeight("50%")
        .setDisplay("grid")
        .setGridTemplateColumns("repeat(3, 1fr)")
        .setGap("25px");

    dutchItemWrapperStyle = new Style();
    dutchItemWrapperStyle->setWidth("100%")
        .setDisplay("flex")
        .setFlexDirection("column")
        .setAlignItems("center")
        .setJustifyContent("space-between")
        .setBackground(Style::secondaryVariant)
        .setBorder("none")
        .setBorderRadius("8px")
        .setAspectRatio("1.618")
        .setPadding("10px 0")
        .setColor(Style::primaryVariant);

    dutchItemUserNameStyle = new Style();
    dutchItemUserNameStyle->setFontSize("12px")
        .setFontWeight("600")
        .setWidth("calc(100\% - 20px)")
        .setTextAlign("left")
        .setBackground(Style::primary)
        .setPadding("10px 10px 0px 10px")
        .setMargin("0")
        .setHeight("20%");

    dutchItemChargeStyle = new Style();
    dutchItemChargeStyle->setFontSize("16px")
        .setFontWeight("400")
        .setWidth("calc(100\% - 20px)")
        .setTextAlign("right")
        .setPadding("0px 10px 10px 10px")
        .setMargin("0")
        .setHeight("20%");
    dutchItemChargeStyle = new Style();
    dutchItemChargeStyle->setFontSize("16px")
        .setFontWeight("400")
        .setWidth("calc(100\% - 20px)")
        .setTextAlign("right")
        .setPadding("0px 10px 10px 10px")
        .setMargin("0")
        .setHeight("20%");

    dutchItemTitleStyle = new Style();
    dutchItemTitleStyle->setFontSize("16px")
        .setBorder("none")
        .setBorderRadius("8px")
        .setFontSize("20px")
        .setFontWeight("bold")
        .setColor(Style::primaryVariant)
        .setMargin("0");

    string username = UserState::getInstance()->getCurrentUser()->getValue().getUsername();

    dutchItemButtonStyle = new Style();
    dutchItemButtonStyle
        ->setBorder("none")
        .setBorderRadius("6px")
        .setPadding("5px 15px")
        .setMargin("0")
        .setBackground(Style::primaryVariant);

    dutchItemButtonTextState = std::make_shared<State<std::string>>("View Detail");

    dutchItem1 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem1");

    dutchItem2 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem2");

    dutchItem3 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem3");

    dutchItem4 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem4");

    dutchItem5 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem5");

    dutchItem6 = new Button(dutchItemButtonTextState, dutchItemButtonStyle, "dutchItem6");

    dutchItemWrapper1 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper1->appendChildren({new Text(dutchList[0]->getSendUserList(), dutchItemTitleStyle),
                                       new Text(dutchList[0]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[0]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem1});

    dutchItemWrapper2 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper2->appendChildren({new Text(dutchList[1]->getSendUserList(), dutchItemTitleStyle),
                                       new Text(dutchList[1]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[1]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem2});

    dutchItemWrapper3 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper3->appendChildren({new Text(dutchList[2]->getSendUserList(), dutchItemTitleStyle),
                                       new Text(dutchList[2]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[2]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem3});

    dutchItemWrapper4 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper4->appendChildren({new Text(dutchList[3]->getSendUserList()),
                                       new Text(dutchList[3]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[3]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem4});

    dutchItemWrapper5 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper5->appendChildren({new Text(dutchList[4]->getSendUserList(), dutchItemTitleStyle),
                                       new Text(dutchList[4]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[4]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem5});

    dutchItemWrapper6 = new Element("div", dutchItemWrapperStyle);
    dutchItemWrapper6->appendChildren({new Text(dutchList[5]->getSendUserList(), dutchItemTitleStyle),
                                       new Text(dutchList[5]->getUsernameText(), dutchItemUserNameStyle),
                                       new Text(dutchList[5]->getTargetAmount(), dutchItemChargeStyle),
                                       dutchItem6});

    dutchItemContainer->appendChildren({dutchItemWrapper1,
                                        dutchItemWrapper2,
                                        dutchItemWrapper3,
                                        dutchItemWrapper4,
                                        dutchItemWrapper5,
                                        dutchItemWrapper6});

    ReceivePage::appendChildren({prevButton, dutchItemContainer.get(), nextButton});

    nextButton->getElement().set("onclick", emscripten::val::module_property("ReceivePage.nextButtonHandler"));
    prevButton->getElement().set("onclick", emscripten::val::module_property("ReceivePage.prevButtonHandler"));
    dutchItem1->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));
    dutchItem2->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));
    dutchItem3->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));
    dutchItem4->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));
    dutchItem5->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));
    dutchItem6->getElement().set("onclick", emscripten::val::module_property("ReceivePage.receiveDutchButtonHandler"));

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = ReceivePage::getDutchListHandler;

    string url = Constants::API_URL + "/dutch/all?user_uuid=" + UserState::getInstance()->getCurrentUser()->getValue().getUUID();
    emscripten_fetch(&attr, url.c_str());
}

ReceivePage *ReceivePage::getInstance()
{
    if (ReceivePage::instance == nullptr)
    {
        ReceivePage::instance = new ReceivePage();
    }

    return ReceivePage::instance;
}

ReceivePage::~ReceivePage()
{
    ReceivePage::instance = nullptr;
}

void ReceivePage::receiveDutchButtonHandler(emscripten::val event)
{
    std::cout << "ReceivePage::receiveDutchButtonHandler" << std::endl;
    string buttonId = event["target"]["id"].as<std::string>();
    buttonId.erase(buttonId.begin(), buttonId.begin() + 9);
    std::stringstream ssInt(buttonId);
    int index = 0;
    ssInt >> index;
    index--;

    ReceiveDutchState::getInstance()->getNowUUID()->setState(dutchList[index]->getDutchUUID());
    ReceiveDutchState::getInstance()->getdutchType()->setState(dutchList[index]->getDutchType());

    Router::getInstance()->navigate("/receiveDetail");
}

void ReceivePage::nextButtonHandler(emscripten::val event)
{
    std::cout << "ReceivePage::nextButtonHandler" << std::endl;
    if (currentPageNumber * 6 + 6 >= dutchUUIDList->getValue().size())
    {
        return;
    }
    currentPageNumber++;

    for (size_t i = 0; i < 6; i++)
    {
        if (dutchUUIDList->getValue().size() <= currentPageNumber * 6 + i)
        {
            dutchList[i]->getSendUserList()->setState("");
            dutchList[i]->getTargetAmount()->setState("");
            dutchList[i]->getUsernameText()->setState("");
            continue;
        }
        std::cout << "ReceiveDutchList: " << dutchUUIDList->getValue()[currentPageNumber * 6 + i] << std::endl;
        dutchList[i]->setDutchUUID(dutchUUIDList->getValue()[currentPageNumber * 6 + i]);

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = ReceivePage::getDutchInfoHandler;

        string url = Constants::API_URL + "/dutch?dutch_uuid=" + dutchUUIDList->getValue()[currentPageNumber * 6 + i];
        emscripten_fetch(&attr, url.c_str());
    }
}

void ReceivePage::prevButtonHandler(emscripten::val event)
{
    std::cout << "ReceivePage::prevButtonHandler" << std::endl;
    if (currentPageNumber == 0)
    {
        return;
    }
    currentPageNumber--;

    for (size_t i = 0; i < 6; i++)
    {
        if (dutchUUIDList->getValue().size() <= currentPageNumber * 6 + i)
        {
            dutchList[i]->getSendUserList()->setState("");
            dutchList[i]->getTargetAmount()->setState("");
            dutchList[i]->getUsernameText()->setState("");
            continue;
        }
        std::cout << "ReceiveDutchList: " << dutchUUIDList->getValue()[currentPageNumber * 6 + i] << std::endl;
        dutchList[i]->setDutchUUID(dutchUUIDList->getValue()[currentPageNumber * 6 + i]);

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = ReceivePage::getDutchInfoHandler;

        string url = Constants::API_URL +"/dutch?dutch_uuid=" + dutchUUIDList->getValue()[i + currentPageNumber * 6];
        emscripten_fetch(&attr, url.c_str());
    }
}

void ReceivePage::getDutchListHandler(emscripten_fetch_t *fetch)
{
    std::cout << "ReceivePage::getDutchLIstHandler" << std::endl;
    std::cout << "status: " << fetch->status << std::endl;
    try
    {
        json j = json::parse(string(fetch->data, fetch->numBytes));
        dutchUUIDList->setState(j["dutch_owner_list"]); 
        for (size_t i = 0; i < (dutchUUIDList->getValue().size() < 6 ? dutchUUIDList->getValue().size() : 6); i++)
        {
            std::cout << "ReceiveDutchList: " << dutchUUIDList->getValue()[i] << std::endl;
            dutchList[i]->setDutchUUID(dutchUUIDList->getValue()[i]);
            dutchList[i]->getUsernameText()->setState("");

            emscripten_fetch_attr_t attr;
            emscripten_fetch_attr_init(&attr);
            strcpy(attr.requestMethod, "GET");
            attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
            attr.onsuccess = ReceivePage::getDutchInfoHandler;

            string url = Constants::API_URL+"/dutch?dutch_uuid=" + dutchUUIDList->getValue()[i];
            emscripten_fetch(&attr, url.c_str());
        }
    }
    catch (json::parse_error &e)
    {
        cout << "ReceivePage::getDutchListHandler: parse error" << e.what() << endl;
    }
    emscripten_fetch_close(fetch);
}

void ReceivePage::getDutchInfoHandler(emscripten_fetch_t *fetch)
{
    std::cout << "ReceivePage::getDutchInfoHandler" << std::endl;
    std::cout << "status: " << fetch->status << std::endl;
    try
    {
        json j = json::parse(string(fetch->data, fetch->numBytes));

        int index;
        for (size_t i = 0; i < 6; i++)
        {
            if (dutchList[i]->getDutchUUID() == j["dutch_uuid"])
            {
                index = i;
                break;
            }
        }

        string username = UserState::getInstance()->getCurrentUser()->getValue().getUsername();

        dutchList[index]->setDutchType(j["type"]);
        dutchList[index]->getUsernameText()->setState(username);

        vector<string> sended = j["send_user_list"];
        vector<string> senders = j["user_name_list"];
        string senderString = "Dutch to ";
        for (string sender : senders)
        {
            senderString += sender + ", ";
        }
        senderString.erase(senderString.end() - 2, senderString.end());
        dutchList[index]->getSendUserList()->setState(senderString);

        int targetAmount = j["target_balance"];
        int currentAmount = j["current_balance"];
        
        if (sended.size() == senders.size() && currentAmount <= 0)
        {
            dutchList[index]->getTargetAmount()->setState("Completed");
            return;
        }

        dutchList[index]->getTargetAmount()->setState("$" + to_string(currentAmount) + " / $" + to_string(targetAmount));
    }
    catch (json::parse_error &e)
    {
        cout << "ReceivePage::getDutchInfoHandler: parse error" << e.what() << endl;
    }
    catch (exception &e)
    {
        std::cout << "ReceivePage::getDutchInfoHandler: " << e.what() << std::endl;
    }
    emscripten_fetch_close(fetch);
}

EMSCRIPTEN_BINDINGS(ReceivePage)
{
    emscripten::function("ReceivePage.receiveDutchButtonHandler", &ReceivePage::receiveDutchButtonHandler);
    emscripten::function("ReceivePage.nextButtonHandler", &ReceivePage::nextButtonHandler);
    emscripten::function("ReceivePage.prevButtonHandler", &ReceivePage::prevButtonHandler);
}