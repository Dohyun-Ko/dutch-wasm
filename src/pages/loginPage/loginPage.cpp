#include "loginPage.h"
#include "../../components/state/State.cpp"
#include "../../components/button/Button.h"
#include "../../components/style/Style.h"
#include "../../components/flex/Flex.h"
#include "../../components/input/Input.h"
#include "../../components/style/Style.h"
#include "../../components/text/Text.h"
#include "../../router/Router.h"
#include "../../components/element/Element.h"
#include "../../globalState/userState/userState.h"
#include "../../utils/Constants.h"

#include <iostream>
#include <functional>
#include <emscripten/bind.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

LoginPage *LoginPage::instance = nullptr;

State<string> *LoginPage::usernameState = new State<string>("");
State<string> *LoginPage::passwordState = new State<string>("");
State<string> *LoginPage::loginState = UserState::getInstance()->getLoginState();
State<User> *LoginPage::userState = UserState::getInstance()->getCurrentUser();

LoginPage::LoginPage() : Element("div")
{
    loginState->setState("Login");
    userState->setState(User("","",""));

    loginTextState = new State<string>("Login");
    signUpTextState = new State<string>("Sign Up");

    container = make_unique<Flex>("column", "center", "center", "10px");
    loginButton = new Button(loginTextState, Style::defaultButtonStyle());
    signUpButton = new Button(signUpTextState, Style::defaultButtonStyle());
    usernameInput = new Input(new State<string>("Username"), Style::defaultInputStyle());
    passwordInput = new Input(new State<string>("Password"), Style::defaultInputStyle());
    loginText = new Text(loginState);

    passwordInput->getElement().set("type", "password");

    container->appendChildren({usernameInput, passwordInput, loginButton, signUpButton, loginText});

    usernameInput->getElement().set("onchange", emscripten::val::module_property("LoginPage.getUsername"));
    passwordInput->getElement().set("onchange", emscripten::val::module_property("LoginPage.getPassword"));
    loginButton->getElement().set("onclick", emscripten::val::module_property("LoginPage.LoginButtonHandler"));
    signUpButton->getElement().set("onclick", emscripten::val::module_property("LoginPage.SignUpButtonHandler"));

    LoginPage::appendChildren(container.get());
}

LoginPage::~LoginPage()
{
    LoginPage::instance = nullptr;

    delete loginButton;
    delete signUpButton;
    delete usernameInput;
    delete passwordInput;
    delete loginText;
}

LoginPage *LoginPage::getInstance()
{
    if (LoginPage::instance == nullptr)
    {
        LoginPage::instance = new LoginPage();
    }

    return LoginPage::instance;
}

void LoginPage::LoginButtonHandler(emscripten::val e)
{
    std::cout << "LoginPage::LoginButtonHander()" << std::endl;

    if (LoginPage::usernameState->getValue() == "" || LoginPage::passwordState->getValue() == "")
    {
        LoginPage::loginState->setState("username or password is empty");
        return;
    }

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = LoginPage::LoginSuccessHandler;
    attr.onerror = LoginPage::LoginfailedHandler;
    loginState->setState("loading...");

    string url = Constants::API_URL + "/user/login?username=" + LoginPage::usernameState->getValue() + "&password=" + LoginPage::passwordState->getValue();
    emscripten_fetch(&attr, url.c_str());

    return;
}

void LoginPage::LoginSuccessHandler(emscripten_fetch_t *fetch)
{
    cout << "LoginPage::LoginNetworkHandler()" << endl;
    cout << "fetch->status: " << fetch->status << endl;
    Router *router = Router::getInstance();
    try
    {
        json j = json::parse(string(fetch->data, fetch->numBytes));
        string username = j["username"];
        string successText = "Hello! " + username;
        LoginPage::loginState->setState(successText);
        LoginPage::userState->setState(User(j["uuid"], j["username"], j["email"]));
        cout << userState->getValue().getEmail() << endl;
    }
    catch (json::parse_error &e)
    {
        cout << "parse error: " << e.what() << endl;
    }
    catch (exception &e)
    {
        cout << "network error" << endl;
    }

    emscripten_fetch_close(fetch);
    router->navigate("/main");
}

void LoginPage::LoginfailedHandler(emscripten_fetch_t *fetch)
{
    cout << "LoginPage::LoginfailedHandler()" << endl;
    cout << "fetch->status: " << fetch->status << endl;
    if (fetch->status == 401)
    {
        LoginPage::loginState->setState("login failed");
    }
    else
    {
        LoginPage::loginState->setState("network error");
    }
    emscripten_fetch_close(fetch);
}

void LoginPage::SignUpButtonHandler(emscripten::val e)
{
    std::cout << "LoginPage::SignUpButtonHander()" << std::endl;
    Router *router = Router::getInstance();
    std::cout << "router: " << router << std::endl;
    router->navigate("/signUp"); // TODO: maybe it's better to change navigate to a static method
    std::cout << "router->navigate" << std::endl;
}

void LoginPage::getUsername(emscripten::val e)
{
    cout << "LoginPage::getUsername()" << endl;
    string username = e["target"]["value"].as<string>();
    LoginPage::usernameState->setState(username);
}

void LoginPage::getPassword(emscripten::val e)
{
    cout << "LoginPage::getPassword()" << endl;
    string password = e["target"]["value"].as<string>();
    LoginPage::passwordState->setState(password);
}

EMSCRIPTEN_BINDINGS(LoginPage)
{
    emscripten::function("LoginPage.LoginButtonHandler", &LoginPage::LoginButtonHandler);
    emscripten::function("LoginPage.SignUpButtonHandler", &LoginPage::SignUpButtonHandler);
    emscripten::function("LoginPage.getUsername", &LoginPage::getUsername);
    emscripten::function("LoginPage.getPassword", &LoginPage::getPassword);
}
