//
//  CUBehaviorParser.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior tree manager.
//
//  Author: Apurv Sethi
//  Version: 3/28/2018
//

#include <cugl/ai/behaviorTree/CUBehaviorParser.h>
#include <cugl/util/CUDebug.h>
#include <unordered_map>

using namespace cugl;

std::unordered_map<std::string, BehaviorNodeDef::Type> typeMap = {
	{"priority", BehaviorNodeDef::Type::PRIORITY_NODE},
	{"selector", BehaviorNodeDef::Type::SELECTOR_NODE},
	{"random", BehaviorNodeDef::Type::RANDOM_NODE},
	{"inverter", BehaviorNodeDef::Type::INVERTER_NODE},
	{"timer", BehaviorNodeDef::Type::TIMER_NODE},
	{"leaf", BehaviorNodeDef::Type::LEAF_NODE}
};

/**
 * Returns a BehaviorNodeDef constructed from the given file.
 *
 * This function assumes that the file name is a relative path. It will
 * search the application assert directory
 * {@see Application#getAssetDirectory()} for the file and return false if
 * it cannot find it there.
 *
 * @param file  The relative path to the file.
 *
 * @return a BehaviorNodeDef constructed from the given file.
 */
std::shared_ptr<BehaviorNodeDef> BehaviorParser::parseFile(const char* file) {
	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
	return parseJson(reader->readJson()->get(0));
}

/**
 * Returns a BehaviorNodeDef constructed from the given JsonValue.
 *
 * This function uses a json in order to construct a BehaviorNodeDef. The
 * json must follow certain guidelines required to create a BehaviorNodeDef
 * in order to be parsed by the function.
 *
 * @param json	The JsonValue representing a BehaviorNodeDef.
 *
 * @return a BehaviorNodeDef constructed from the given JsonValue.
 */
std::shared_ptr<BehaviorNodeDef> BehaviorParser::parseJson(const std::shared_ptr<JsonValue>& json) {
	std::shared_ptr<BehaviorNodeDef> node = std::make_shared<BehaviorNodeDef>();
	node->_name = json->key();
	std::string type = json->getString("type");
	CUAssertLog(!type.empty(), "The type of a BehaviorNodeDef must be defined");
	node->_type = typeMap.at(type);

	node->_preempt = json->getBool("preempt", false);
	node->_uniformRandom = json->getBool("uniformRandom", true);
	node->_timeDelay = json->getBool("timeDelay", true);
	node->_delay = json->getFloat("delay", 1.0f);
	std::shared_ptr<JsonValue> children = json->get("children");
	if (children != nullptr) {
		for (int ii = 0; ii < children->size(); ii++) {
			node->_children.push_back(parseJson(children->get(ii)));
		}
	}
	return node;
}
