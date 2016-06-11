#include "GameScene.h"
#include "LoginScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
#include <sstream>

using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;


    score_field = TextField::create("Score", "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
    this->addChild(submit_button, 2);
	submit_button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setRequestType(HttpRequest::Type::POST);
			request->setUrl("http://localhost:8080/submit");
			request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestCompleted, this));
			request->setTag("Post of score");

			std::string score = "score=" + score_field->getString();
			log(score.c_str());
			const char* postData = score.c_str();
			request->setRequestData(postData, strlen(postData));

			vector<string> headers;
			headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
			request->setHeaders(headers);

			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();
		}
	});

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
    this->addChild(rank_button, 2);

	rank_button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("http://localhost:8080/rank?top=10");
			request->setRequestType(HttpRequest::Type::GET);

			request->setResponseCallback(CC_CALLBACK_2(GameScene::onHttpRequestRank, this));
			request->setTag("GET of rank");

			vector<string> headers;
			headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
			request->setHeaders(headers);

			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();
		}
	});

	signOut_button = Button::create();
	signOut_button->setTitleText("Exit");
	signOut_button->setTitleFontSize(30);
	signOut_button->setPosition(Size(visibleWidth / 2, signOut_button->getContentSize().height));
	this->addChild(signOut_button, 2);
	signOut_button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			Global::gameSessionId = "";
			Global::score = 0;
			Global::mode = 1;
			auto transition = TransitionCrossFade::create(0.2f, LoginScene::createScene());
			Director::getInstance()->replaceScene(transition);
		}
	});
    return true;
}

void GameScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
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
	
	std::vector<char> *buffer = response->getResponseData();
	for (unsigned int i = 0; i < buffer->size(); i++) {
		result += (*buffer)[i];
	}
	log(result.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.c_str());

	if (d.HasParseError())  //打印解析错误
	{
		CCLOG("GetParseError %s\n", d.GetParseError());
	}
	if (d.IsObject() && d.HasMember("info")) {
		std::stringstream ss;
		long score, newScore;
		ss << d["info"].GetString();
		ss >> score;
		ss << score_field->getString();
		ss >> newScore;
		if (newScore >= score) {
			Global::score = newScore;
		} else {
			score_field->setString(d["info"].GetString());
		}
		log(Global::score);
		CCLOG("%s\n", d["info"].GetString());//打印获取hello的值
	}
}

void GameScene::onHttpRequestRank(HttpClient* sender, HttpResponse* response) {
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

	std::vector<char> *buffer = response->getResponseData();
	for (unsigned int i = 0; i < buffer->size(); i++) {
		result += (*buffer)[i];
	}
	log(result.c_str());

	rapidjson::Document d;
	d.Parse<0>(result.c_str());
	if (d.HasParseError())  //打印解析错误
	{
		CCLOG("GetParseError %s\n", d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("info")) {
		CCLOG("%s\n", d["info"].GetString());//打印获取hello的值
		string rank = d["info"].GetString(), rankChange = "";
		if (rank != "") {
			for (unsigned i = 1, len = rank.size(); i < len; i++) {
				if (rank[i] == '|') {
					rankChange += '\n';
				} else {
					rankChange += rank[i];
				}
			}
		}
		
		rank_field->setString(rankChange);
		log(rankChange.c_str());
	}
}



