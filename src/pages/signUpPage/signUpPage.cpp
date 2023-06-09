#include <emscripten/bind.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

#include "signUpPage.h"
#include "../../components/state/State.cpp"
#include "../../components/button/Button.h"
#include "../../components/flex/Flex.h"
#include "../../components/input/Input.h"
#include "../../components/style/Style.h"
#include "../../utils/Constants.h"
#include "../../router/Router.h"

using json = nlohmann::json;
using namespace std;

State<string> *SignUpPage::usernameState = new State<string>("");
State<string> *SignUpPage::passwordState = new State<string>("");
State<string> *SignUpPage::emailState = new State<string>("");
State<string> *SignUpPage::signUpSuccessState = new State<string>("");

SignUpPage *SignUpPage::instance = nullptr;

SignUpPage::SignUpPage() : Element("div")
{
    container = make_unique<Flex>("column", "center", "center", "10px");
    usernameInput = new Input(new State<string>("Username"), Style::defaultInputStyle());
    passwordInput = new Input(new State<string>("Password"), Style::defaultInputStyle());
    emailInput = new Input(new State<string>("Email"), Style::defaultInputStyle());
    signUpButton = new Button(new State<string>("Sign Up"), Style::defaultButtonStyle());
    signUpText = make_unique<Text>(signUpSuccessState);

    container->appendChildren({usernameInput, passwordInput, emailInput, signUpButton, signUpText.get()});

    signUpButton->getElement().set("onclick", emscripten::val::module_property("SignUpPage.SignUpButtonHander"));
    usernameInput->getElement().set("onchange", emscripten::val::module_property("SignUpPage.getUsername"));
    passwordInput->getElement().set("onchange", emscripten::val::module_property("SignUpPage.getPassword"));
    emailInput->getElement().set("onchange", emscripten::val::module_property("SignUpPage.getEmail"));

    passwordInput->getElement().set("type", "password");

    SignUpPage::appendChildren(container.get());
}

SignUpPage *SignUpPage::getInstance()
{
    if (SignUpPage::instance == nullptr)
    {
        SignUpPage::instance = new SignUpPage();
    }

    return SignUpPage::instance;
}

SignUpPage::~SignUpPage()
{
    SignUpPage::instance = nullptr;
    delete signUpButton;
    delete usernameInput;
    delete passwordInput;
    delete emailInput;
}

void SignUpPage::getUsername(emscripten::val e)
{
    cout << "SignUpPage.getUsername" << endl;
    string username = e["target"]["value"].as<string>();
    SignUpPage::usernameState->setState(username);
}

void SignUpPage::getPassword(emscripten::val e)
{
    cout << "SignUpPage.getPassword" << endl;
    string password = e["target"]["value"].as<string>();
    SignUpPage::passwordState->setState(password);
}

void SignUpPage::getEmail(emscripten::val e)
{
    cout << "SignUpPage.getEmail" << endl;
    string email = e["target"]["value"].as<string>();
    SignUpPage::emailState->setState(email);
}

void SignUpPage::SignUpButtonHander(emscripten::val e)
{
    std::cout << "SignUpPage.SignUpButtonHander" << std::endl;
    std::cout << "username: " << SignUpPage::usernameState->getValue() << std::endl;
    std::cout << "password: " << SignUpPage::passwordState->getValue() << std::endl;
    std::cout << "email: " << SignUpPage::emailState->getValue() << std::endl;
    if (SignUpPage::usernameState->getValue() == "" || SignUpPage::passwordState->getValue() == "" || SignUpPage::emailState->getValue() == "")
    {
        SignUpPage::signUpSuccessState->setState("Please fill out all fields");
        return;
    }

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = SignUpPage::SignUpNetworkHandler;
    attr.onerror = SignUpPage::SignUPFailedHandler;
    signUpSuccessState->setState("Signing Up...");

    string url = Constants::API_URL + "/user?username=" + SignUpPage::usernameState->getValue() + "&password=" + SignUpPage::passwordState->getValue() + "&email=" + SignUpPage::emailState->getValue();
    emscripten_fetch(&attr, url.c_str());
}

void SignUpPage::SignUpNetworkHandler(emscripten_fetch_t *fetch)
{
    std::cout << "SignUpPage.SignUpNetworkHandler" << std::endl;
    std::cout << "status: " << fetch->status << std::endl;
    try
    {
        json j = json::parse(string(fetch->data, fetch->numBytes));
        SignUpPage::signUpSuccessState->setState(j["uuid"]);
        Router::getInstance()->navigate("/login");
    }
    catch (json::parse_error &e)
    {
        std::cout << "parse error: " << e.what() << std::endl;
    }
    emscripten_fetch_close(fetch);
}

void SignUpPage::SignUPFailedHandler(emscripten_fetch_t *fetch)
{
    std::cout << "SignUpPage.SignUPFailedHandler" << std::endl;
    std::cout << "status: " << fetch->status << std::endl;
    if (fetch->status == 401)
    {
        SignUpPage::signUpSuccessState->setState("Username already exists");
    }
    else
    {
        SignUpPage::signUpSuccessState->setState("Sign Up Failed");
    }
    emscripten_fetch_close(fetch);
}

EMSCRIPTEN_BINDINGS(SignUpPage)
{
    emscripten::function("SignUpPage.SignUpButtonHander", &SignUpPage::SignUpButtonHander);
    emscripten::function("SignUpPage.getUsername", &SignUpPage::getUsername);
    emscripten::function("SignUpPage.getPassword", &SignUpPage::getPassword);
    emscripten::function("SignUpPage.getEmail", &SignUpPage::getEmail);
}