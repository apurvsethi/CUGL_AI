//
//  CUBehaviorParser.cpp
//  Cornell University Game Library (CUGL)
//
//  This module provides support for a behavior parser in order to create a
//  BodyDefNode from a JSON.
//
//  You should never instantiate an object of this class, as it is only used
//  for static methods. It has no allocators, initializer, or constructor.
//
//
//  This class uses our standard shared-pointer architecture.
//
//  1. The constructor does not perform any initialization; it just sets all
//     attributes to their defaults.
//
//  2. All initialization takes place via init methods, which can fail if an
//     object is initialized more than once.
//
//  3. All allocation takes place via static constructors which return a shared
//     pointer.
//
//  Author: Apurv Sethi and Andrew Matsumoto
//  Version: 5/22/2018
//

#include <cugl/ai/behaviorTree/CUBehaviorParser.h>
#include <cugl/util/CUDebug.h>
#include <unordered_map>

using namespace cugl;

/** A mapping of the string values to the behavior node types. */
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
