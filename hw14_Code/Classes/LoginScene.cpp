#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include <regex>
#include <sstream>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Player Name", "Arial", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

    auto button = Button::create();
    button->setTitleText("Login");
    button->setTitleFontSize(30);
    button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
    this->addChild(button, 2);
	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		log(textField->getString().c_str());
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setRequestType(HttpRequest::Type::POST);
			request->setUrl("http://localhost:8080/login");
			request->setResponseCallback(CC_CALLBACK_2(LoginScene::onHttpRequestCompleted, this));
			request->setTag("Post of login");
			
			if (textField->getString() == "") {
				Global::mode = 0;
				Global::gameSessionId = UserDefault::getInstance()->getStringForKey("Id", "");
			} else {
				std::string username = "username=" + textField->getString();
				log(username.c_str());
				const char* postData = username.c_str();
				request->setRequestData(postData, strlen(postData));

				cocos2d::network::HttpClient::getInstance()->send(request);
				request->release();
			}
			auto transition = TransitionCrossFade::create(0.2f, GameScene::createScene());
			Director::getInstance()->replaceScene(transition);
		}
	});
    return true;
}

void LoginScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
	if (Global::mode == 1) {
		if (!response) {
			return;
		}
		if (!response->isSucceed()) {
			log("response failed");
			log("error buffer: %s", response->getErrorBuffer());
			return;
		}
		log("Login succeed");

		std::string result = "";
		std::vector<char> *header = response->getResponseHeader();
		for (unsigned int i = 0; i < header->size(); i++) {
			result += (*header)[i];
		}

		Global::gameSessionId = Global::getSessionIdFromHeader(Global::toString(header));
		UserDefault::getInstance()->setStringForKey("Id", Global::gameSessionId);
		UserDefault::getInstance()->flush();
	
		result = "";
		std::vector<char> *buffer = response->getResponseData();
		result = Global::toString(buffer);
		log(result.c_str());

		rapidjson::Document d;
		d.Parse<0>(result.c_str());
		if (d.HasParseError())  //打印解析错误
		{
			CCLOG("GetParseError %s\n", d.GetParseError());
		}

		if (d.IsObject() && d.HasMember("info")) {
			std::stringstream ss;
			ss << d["info"].GetString();
			ss >> Global::score;
			CCLOG("%s\n", d["info"].GetString());//打印获取hello的值
		}
	}

}

