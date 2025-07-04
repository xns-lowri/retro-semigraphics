#pragma once
#include "rsg-datatypes.h"

//abstract definition for gui components

namespace rsgui {
	/* Component abstract class */
	class Component {
		rsd::uint2 position;
		rsd::uint2 size;
		Component* parent;

	};


	/* Container abstract class structs */
	struct ComponentList {
		Uint32 length;
		ComponentListNode* head;
		ComponentListNode* tail;
	};

	struct ComponentListNode {
		Component* component;

		ComponentListNode* prev;
		ComponentListNode* next;
	};

	/* Container abstract class */
	class Container {
		ComponentList* children;
	};
};