#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten/fetch.h>
#include <string>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>

#include "components/button/Button.h"
#include "components/flex/Flex.h"
#include "components/input/Input.h"
#include "components/state/State.cpp"
#include "components/style/Style.h"
#include "pages/loginPage/loginPage.h"
#include "pages/signUpPage/signUpPage.h"
#include "pages/sendPage/sendPage.h"
#include "pages/sendDetailPage/sendDetailPage.h"
#include "pages/receivePage/receivePage.h"
#include "pages/receiveDetailPage/receiveDetailPage.h"
#include "pages/makeDutchPage/makeDutchPage.h"
#include "pages/mainPage/mainPage.h"
#include "pages/addBalancePage/addBalancePage.h"
#include "router/Router.h"

using namespace emscripten;
using json = nlohmann::json;

val getElementById(std::string id)
{
    return val::global("document").call<val>("getElementById", val(id));
}

static void backButtonHander(emscripten::val e)
{
    std::cout << "backButtonHander" << std::endl;
    Router::getInstance()->navigate("back");
}

EMSCRIPTEN_BINDINGS(layout)
{
    emscripten::function("layout.backButtonHander", &backButtonHander);
}

int main()
{
    val root = getElementById("root");

    std::unique_ptr<Element> layout = make_unique<Element>("div");

    layout->getStyle()
        .setPosition("relative")
        .setDisplay("flex")
        .setFlexDirection("column")
        .setAlignItems("center");

    std::unique_ptr<Element> header = make_unique<Element>("div");
    header->getStyle()
        .setPosition("absolute")
        .setTop("0")
        .setLeft("0")
        .setDisplay("flex")
        .setAlignItems("center")
        .setWidth("100%")
        .setHeight("80px");

    std::shared_ptr<Element> body = make_shared<Element>("div");
    body->getStyle()
        .setDisplay("flex")
        .setFlexDirection("column")
        .setAlignItems("center")
        .setJustifyContent("center")
        .setWidth("100%")
        .setHeight("calc(100vh)")
        .setBackground(Style::secondary);

    std::unique_ptr<Button> backButton = make_unique<Button>(new State<string>("⬅️"));
    backButton->getStyle()
        .setBorder("none")
        .setBackground("transparent")
        .setFontSize("2rem")
        .setCursor("pointer");

    backButton->getElement().set("onclick", emscripten::val::module_property("layout.backButtonHander"));
    header->appendChildren(backButton.get());
    layout->appendChildren({header.get(), body.get()});

    root.call<void>("appendChild", layout->getElement());

    Router router(
        body,
        {
            {"/login", []()
             { return LoginPage::getInstance(); }},
            {"/signUp", []()
             { return SignUpPage::getInstance(); }},
            {"/main", []()
             { return MainPage::getInstance(); }},
            {"/send", []()
             { return SendPage::getInstance(); }},
            {"/receive", []()
             { return ReceivePage::getInstance(); }},
            {"/addBalance", []()
             { return AddBalancePage::getInstance(); }},
            {"/sendDetail", []()
             { return SendDetailPage::getInstance(); }},
            {"/receiveDetail", []()
             { return ReceiveDetailPage::getInstance(); }},
            {"/makeDutch", []()
             { return MakeDutchPage::getInstance(); }},
        },
        "/main");

    while (true)
    {
        emscripten_sleep(100);
    }
}
