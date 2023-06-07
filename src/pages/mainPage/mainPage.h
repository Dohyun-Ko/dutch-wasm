#pragma once

#include "../../components/element/Element.h"
#include "../../components/button/Button.h"
#include "../../components/flex/Flex.h"
#include "../../components/text/Text.h"
#include "../../core/user/user.h"

class MainPage : public Element
{

    State<string> *myBalance;

    Element *container;
    Flex *leftSide;
    Flex *rightSide;
    Text *balanceText;
    Text *myBalanceText;
    Text *loginSuccessText;

    Button *sendButton;
    Button *receiveButton;
    Button *loginButton; // global state에 유저 정보가 있으면 이 버튼을 안보이게 해야함 - branch 20 참고
    Button *makeDutchButton;

    static MainPage *instance;
    MainPage();

public:
    ~MainPage();

    static State<User>* userState;
    static State<string>* loginState;

    static MainPage *getInstance();
    static void sendButtonHandler(emscripten::val event);
    static void receiveButtonHandler(emscripten::val event);
    static void loginButtonHandler(emscripten::val event);
    static void makeDutchButtonHandler(emscripten::val event);
};