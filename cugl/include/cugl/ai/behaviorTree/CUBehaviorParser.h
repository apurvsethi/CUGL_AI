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
#include <cugl/ai/behaviorTree/CUBehaviorNode.h>
#include <cugl/io/CUJsonReader.h>

namespace cugl {

/**
 * This class provides a behavior node for a behavior tree.
 *
 * A behavior node refers to any given node within a beahvior tree,
 * whether it is a CompositeNode, DecoratorNode, or LeafNode.
 *
 * A behavior tree is a construction of behavior nodes, with the top
 * node that does not have a parent referring to the root of the tree.
 * The tree must use an update function to run on each tick, updating
 * the state of each node.
 *
 * The root node of a behavior tree returns the state of the currently
 * running node, or the node that ran during the update cycle.
 */
class BehaviorParser {
public:
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
	static std::shared_ptr<BehaviorNodeDef> parseFile(const std::string& file) {
		return parseFile(file.c_str());
	}

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
	static std::shared_ptr<BehaviorNodeDef> parseFile(const char* file);

private:
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
	static std::shared_ptr<BehaviorNodeDef> parseJson(const std::shared_ptr<JsonValue>& json);
};


}
#endif /* __CU_BEHAVIOR_PARSER_H__ */
