#pragma once
#include "rsg-datatypes.h"

//abstract definition for gui components

class RsgEngine;

namespace rsgui {
	/* Component abstract class */
	class Component {
	protected:
		//Component* parent; //not needed if always accessed top-down? we shall see
		rsd::uint2 position;
		rsd::uint2 size;
		rsd::float4 fgCol;
		rsd::float4 bgCol;
	public:
		virtual void Repaint(RsgEngine* engine) {};
		virtual void Paint(RsgEngine* engine) {};
	};

	/* components that change when moused over/click or kb select */
	class Selectable {
	protected:
		bool highlightable;
		rsd::float4 highlightedFgCol;
		rsd::float4 highlightedBgCol;
		bool selectable;
		rsd::float4 selectedFgCol;
		rsd::float4 selectedBgCol;
	public:
		/* Highlight methods */
		void SetHighlightedForegroundColour(rsd::float4 col) {
			highlightedFgCol = col;
		}
		void SetHighlightedBackgroundColour(rsd::float4 col) {
			highlightedBgCol = col;
		}
		void SetHighlightable(bool highlightable) {
			this->highlightable = highlightable;
		}
		bool GetHighlightable() {
			return highlightable;
		}

		/* Select methods */
		void SetSelectedForegroundColour(rsd::float4 col) {
			highlightedFgCol = col;
		}
		void SetSelectedBackgroundColour(rsd::float4 col) {
			highlightedBgCol = col;
		}
		void SetSelectable(bool selectable) {
			this->selectable = selectable;
		}
		bool GetSelectable() {
			return selectable;
		}

		/* Callback methods */
		//virtual void OnHighlighted() {};
		//virtual void OnSelected() {};
	};

	/* components that can be turned on or off */
	class Inhibitable {
	protected:
		bool inhibited;
		rsd::float4 inhibitedFgCol;
		rsd::float4 inhibitedBgCol;
	public:
		void SetInhibitedForegroundColour(rsd::float4 col) {
			inhibitedFgCol = col;
		}
		void SetInhibitedBackgroundColour(rsd::float4 col) {
			inhibitedBgCol = col;
		}
		void SetInhibited(bool inhibited) {
			this->inhibited = inhibited;
		}
		bool GetInhibited() {
			return inhibited;
		}
	};

	/* Component List Node class */
	//entry in linked list object
	class ComponentListNode {
	public:
		Component* component;
		ComponentListNode* prev;
		ComponentListNode* next;

		ComponentListNode(
			Component* comp, 
			ComponentListNode* p, 
			ComponentListNode* n
		) :
			component(comp), 
			prev(p), 
			next(n) 
		{};
	};

	/* Component List class */
	//linked list controller object
	class ComponentList {
		Uint32 n_items;
		ComponentListNode* head;
		ComponentListNode* tail;
	private:
		ComponentListNode* GetNode(Uint32 index) {
			//reject out of range
			if (index >= n_items) {
				return NULL;
			}
			//iteratively deref, is there a better way?
			ComponentListNode* node = head;
			for (Uint32 i = 0; i < index; ++i) {
				if (node == NULL) {
					break;
				}
				node = node->next;
			}
			return node;
		}
	public:
		ComponentList() {
			n_items = 0;
			head = NULL;
			tail = NULL;
		}
		Uint32 Length() {
			return n_items;
		}
		Component* GetComponent(Uint32 index) {
			ComponentListNode* node = GetNode(index);
			//don't deref null ptr
			if (node == NULL) { return NULL; }
			//return component ptr if node exists
			return node->component;
		}
		ComponentListNode* AddComponent(Component* comp) {
			//calm down with the components m8ty
			if (n_items == UINT32_MAX) { return NULL; }
			//create new list node object:
			// referencing comp param, prev = current tail, next = null (end of list)
			ComponentListNode* new_node = new ComponentListNode(comp, tail, NULL);

			//should be equivalent to n_items > 0
			if (tail != NULL) {
				//set old tail's next ptr
				tail->next = new_node;
			}

			//set head if this is the first node
			if (n_items == 0) {
				head = new_node;
			}

			//set tail and increment item count
			tail = new_node;
			++n_items;

			return new_node;
		}
		/* Returns ptr to component if found */
		Component* RemoveComponent(Uint32 index) {
			//get node to delete
			ComponentListNode* del_node = GetNode(index);
			if (del_node == NULL) { return NULL; }

			Component* comp_at_node = del_node->component;

			ComponentListNode* prev_node = del_node->prev;
			ComponentListNode* next_node = del_node->next;

			if (prev_node != NULL) {
				//not first in list
				//join prev to next
				prev_node->next = next_node;
			}
			else {
				//first in list = at head
				head = NULL;
			}

			if (next_node != NULL) {
				//not last in list
				next_node->prev = prev_node;
			}
			else {
				//last in list = at tail
				tail = prev_node;
				//prev null if also at head/last node in list
			}

			//delete list node object and decrement item count
			delete(del_node);
			--n_items;

			//remember to delete object at returned ptr!
			return comp_at_node;
		}
	};

	/* Container abstract class */
	class Container {
	public:
		ComponentList* children;
	};
};