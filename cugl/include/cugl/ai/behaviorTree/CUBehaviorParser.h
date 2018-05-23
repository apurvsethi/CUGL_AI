//
//  CUBehaviorParser.h
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

#ifndef __CU_BEHAVIOR_PARSER_H__
#define __CU_BEHAVIOR_PARSER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/io/CUJsonReader.h>

namespace cugl {

/**
 * This class provides methods to parse a JSON file describing a behavior tree.
 *
 * This class parses a JSON file to create a mapping from the names of the
 * of each behavior tree to the {@link BehaviorTreeDef} of that tree. After
 * parsing a JSON file, you must add an action definition to each leaf, and
 * any required priority function to the defs.
 *
 * This class contains only static methods, and should not be instantiated.
 */
class BehaviorParser {
#pragma mark -
#pragma mark Parsing
public:
	/**
	 * Parses the JSON file provided to create behavior node defs, and adds these
	 * BehaviorNodeDefs to a map, used to allow the user to get individual
	 * BehaviorNodeDefs by using the name of the root as the key.
	 *
	 * This function assumes that the file name is a relative path. It will
	 * search the application assert directory
	 * {@see Application#getAssetDirectory()} for the file and return false if
	 * it cannot find it there.
	 *
	 * @param file  The relative path to the file.
	 */
	static std::unordered_map<std::string, std::shared_ptr<BehaviorNodeDef>> parseFile(const std::string& file) {
		return parseFile(file.c_str());
	}

	/**
	 * Parses the JSON file provided to create behavior node defs, and adds these
	 * BehaviorNodeDefs to a map, used to allow the user to get individual
	 * BehaviorNodeDefs by using the name of the root as the key.
	 *
	 * This function assumes that the file name is a relative path. It will
	 * search the application assert directory
	 * {@see Application#getAssetDirectory()} for the file and return false if
	 * it cannot find it there.
	 *
	 * @param file  The relative path to the file.
	 */
	static std::unordered_map<std::string, std::shared_ptr<BehaviorNodeDef>> parseFile(const char* file);

private:
	/**
	 * Returns a BehaviorNodeDef constructed from the given JsonValue.
	 *
	 * This function uses a JSON in order to construct a BehaviorNodeDef. The
	 * JSON must follow certain guidelines required to create a BehaviorNodeDef
	 * in order to be parsed by the function.
	 *
	 * @param json	The JsonValue representing a BehaviorNodeDef.
	 *
	 * @return a BehaviorNodeDef constructed from the given JsonValue.
	 */
	static std::shared_ptr<BehaviorNodeDef> parseJson(const std::shared_ptr<JsonValue>& json);
};


}
#endif /* __CU_BEHAVIOR_PARSER_H__ */
