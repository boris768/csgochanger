#pragma once
#include "SourceEngine/UtlMemory.hpp"
#include <cassert>


struct base_utlmap_t
{
	// This enum exists so that FOR_EACH_MAP and FOR_EACH_MAP_FAST cannot accidentally
	// be used on a type that is not a CUtlMap. If the code compiles then all is well.
	// The check for IsUtlMap being true should be free.
	// Using an enum rather than a static const bool ensures that this trick works even
	// with optimizations disabled on gcc.
	enum { IsUtlMap = true };
};

// For use with FindClosest
// Move these to a common area if anyone else ever uses them
enum CompareOperands_t
{
	k_EEqual = 0x1,
	k_EGreaterThan = 0x2,
	k_ELessThan = 0x4,
	k_EGreaterThanOrEqualTo = k_EGreaterThan | k_EEqual,
	k_ELessThanOrEqualTo = k_ELessThan | k_EEqual,
};

#define FOR_EACH_VEC( vecName, iteratorName ) \
	for ( int (iteratorName) = 0; (iteratorName) < (vecName).Count(); (iteratorName)++ )
 
#define FOR_EACH_MAP( mapName, iteratorName ) \
	for ( int (iteratorName) = (mapName).FirstInorder(); (iteratorName) != -1; (iteratorName) = (mapName).NextInorder( iteratorName ) )
 
#define FOR_EACH_MAP_FAST( mapName, iteratorName ) \
	for ( int (iteratorName) = 0; (iteratorName) < (mapName).MaxElement(); ++(iteratorName) ) if ( !(mapName).IsValidIndex( iteratorName ) ) continue; else


namespace SourceEngine
{
	template <typename I>
	struct UtlRBTreeLinks_t
	{
		I m_Left;
		I m_Right;
		I m_Parent;
		I m_Tag;
	};
	 
	template <typename T, typename I>
	struct UtlRBTreeNode_t : UtlRBTreeLinks_t<I>
	{
		T m_Data;
	};


	enum { IsUtlRBTree = true }; // Used to match this at compiletime 
	template < class T, class I = unsigned short, typename L = bool (__cdecl*)(const T&, const T&), class M = CUtlMemory< UtlRBTreeNode_t< T, I >, I > >
	class CUtlRBTree
	{
	public:

		typedef T KeyType_t;
		typedef T ElemType_t;
		typedef I IndexType_t;

		// Less func typedef
		// Returns true if the first parameter is "less" than the second
		typedef L LessFunc_t;

		// constructor, destructor
		// Left at growSize = 0, the memory will first allocate 1 element and double in size
		// at each increment.
		// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
		explicit CUtlRBTree(int growSize = 0, int initSize = 0, const LessFunc_t& lessfunc = 0);
		explicit CUtlRBTree(const LessFunc_t& lessfunc);
		~CUtlRBTree();

		void EnsureCapacity(int num);

		void CopyFrom(const CUtlRBTree<T, I, L, M>& other);

		// gets particular elements
		T& Element(I i);
		T const& Element(I i) const;
		T& operator[](I i);
		T const& operator[](I i) const;

		// Gets the root
		I  Root() const;

		// Num elements
		unsigned int Count() const;

		// Max "size" of the vector
		// it's not generally safe to iterate from index 0 to MaxElement()-1 (you could do this as a potential
		// iteration optimization, IF CUtlMemory is the allocator, and IF IsValidIndex() is tested for each element...
		//  but this should be implemented inside the CUtlRBTree iteration API, if anywhere)
		I  MaxElement() const;

		// Gets the children                               
		I  Parent(I i) const;
		I  LeftChild(I i) const;
		I  RightChild(I i) const;

		// Tests if a node is a left or right child
		bool  IsLeftChild(I i) const;
		bool  IsRightChild(I i) const;

		// Tests if root or leaf
		bool  IsRoot(I i) const;
		bool  IsLeaf(I i) const;

		// Checks if a node is valid and in the tree
		bool  IsValidIndex(I i) const;

		// Checks if the tree as a whole is valid
		bool  IsValid() const;

		// Invalid index
		static I InvalidIndex();

		// returns the tree depth (not a very fast operation)
		int   Depth(I node) const;
		int   Depth() const;

		// Sets the less func
		void SetLessFunc(const LessFunc_t& func);

		// Allocation method
		I  NewNode();

		// Insert method (inserts in order)
		// NOTE: the returned 'index' will be valid as long as the element remains in the tree
		//       (other elements being added/removed will not affect it)
		I  Insert(T const& insert);
		void Insert(const T* pArray, int nItems);
		I  InsertIfNotFound(T const& insert);

		// Find method
		I  Find(T const& search) const;

		// FindFirst method ( finds first inorder if there are duplicates )
		I  FindFirst(T const& search) const;

		// First element >= key
		I  FindClosest(T const& search, CompareOperands_t eFindCriteria) const;

		// Remove methods
		void     RemoveAt(I i);
		bool     Remove(T const& remove);
		void     RemoveAll();
		void	 Purge();

		// Allocation, deletion
		void  FreeNode(I i);

		// Iteration
		I  FirstInorder() const;
		I  NextInorder(I i) const;
		I  PrevInorder(I i) const;
		I  LastInorder() const;

		I  FirstPreorder() const;
		I  NextPreorder(I i) const;
		I  PrevPreorder(I i) const;
		I  LastPreorder() const;

		I  FirstPostorder() const;
		I  NextPostorder(I i) const;

		// If you change the search key, this can be used to reinsert the 
		// element into the tree.
		void	Reinsert(I elem);

		// swap in place
		void Swap(CUtlRBTree< T, I, L >& that);

		// Can't copy the tree this way!
		CUtlRBTree<T, I, L, M>& operator=(const CUtlRBTree<T, I, L, M>& other) = delete;
		
		// copy constructors not allowed
		CUtlRBTree(CUtlRBTree<T, I, L, M> const& tree) = delete;

		enum NodeColor_t
		{
			RED = 0,
			BLACK
		};

		typedef UtlRBTreeNode_t< T, I > Node_t;
		typedef UtlRBTreeLinks_t< I > Links_t;

		// Sets the children
		void  SetParent(I i, I parent);
		void  SetLeftChild(I i, I child);
		void  SetRightChild(I i, I child);
		void  LinkToParent(I i, I parent, bool isLeft);

		// Gets at the links
		Links_t const& Links(I i) const;
		Links_t& Links(I i);

		// Checks if a link is red or black
		bool IsRed(I i) const;
		bool IsBlack(I i) const;

		// Sets/gets node color
		NodeColor_t Color(I i) const;
		void        SetColor(I i, NodeColor_t c);

		// operations required to preserve tree balance
		void RotateLeft(I i);
		void RotateRight(I i);
		void InsertRebalance(I i);
		void RemoveRebalance(I i);

		// Insertion, removal
		I  InsertAt(I parent, bool leftchild);

		// Inserts a node into the tree, doesn't copy the data in.
		void FindInsertionPosition(T const& insert, I& parent, bool& leftchild);

		// Remove and add back an element in the tree.
		void	Unlink(I elem);
		void	Link(I elem);

		// Used for sorting.
		LessFunc_t m_LessFunc;

		M m_Elements;
		I m_Root;
		I m_NumElements;
		I m_FirstFree;
		typename M::Iterator_t m_LastAlloc; // the last index allocated

		Node_t* m_pElements;

		__forceinline M & Elements()
		{
			return m_Elements;
		}


		void ResetDbgInfo()
		{
			m_pElements = (Node_t*)m_Elements.Base();
		}
	};

	template < class T, class I, typename L, class M >
	CUtlRBTree<T, I, L, M>::CUtlRBTree(int growSize, int initSize, const LessFunc_t& lessfunc) :
		m_LessFunc(lessfunc),
		m_Elements(growSize, initSize),
		m_Root(InvalidIndex()),
		m_NumElements(0),
		m_FirstFree(InvalidIndex()),
		m_LastAlloc(m_Elements.InvalidIterator())
	{
		ResetDbgInfo();
	}

	template < class T, class I, typename L, class M >
	CUtlRBTree<T, I, L, M>::CUtlRBTree(const LessFunc_t& lessfunc) :
		m_Elements(0, 0),
		m_LessFunc(lessfunc),
		m_Root(InvalidIndex()),
		m_NumElements(0),
		m_FirstFree(InvalidIndex()),
		m_LastAlloc(m_Elements.InvalidIterator())
	{
		ResetDbgInfo();
	}

	template < class T, class I, typename L, class M >
	CUtlRBTree<T, I, L, M>::~CUtlRBTree()
	{
		Purge();
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::EnsureCapacity(int num)
	{
		m_Elements.EnsureCapacity(num);
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::CopyFrom(const CUtlRBTree<T, I, L, M>& other)
	{
		Purge();
		m_Elements.EnsureCapacity(other.m_Elements.Count());
		memcpy(m_Elements.Base(), other.m_Elements.Base(), other.m_Elements.Count() * sizeof(T));
		m_LessFunc = other.m_LessFunc;
		m_Root = other.m_Root;
		m_NumElements = other.m_NumElements;
		m_FirstFree = other.m_FirstFree;
		m_LastAlloc = other.m_LastAlloc;
		ResetDbgInfo();
	}

	template < class T, class I, typename L, class M >
	T& CUtlRBTree<T, I, L, M>::Element(I i)
	{
		return m_Elements[i].m_Data;
	}

	template < class T, class I, typename L, class M >
	T const& CUtlRBTree<T, I, L, M>::Element(I i) const
	{
		return m_Elements[i].m_Data;
	}

	template < class T, class I, typename L, class M >
	T& CUtlRBTree<T, I, L, M>::operator[](I i)
	{
		return Element(i);
	}

	template < class T, class I, typename L, class M >
	T const& CUtlRBTree<T, I, L, M>::operator[](I i) const
	{
		return Element(i);
	}

	template < class T, class I, typename L, class M >
	I  CUtlRBTree<T, I, L, M>::Root() const
	{
		return m_Root;
	}

	template < class T, class I, typename L, class M >
	unsigned int CUtlRBTree<T, I, L, M>::Count() const
	{
		return (unsigned int)m_NumElements;
	}

	template < class T, class I, typename L, class M >
	I  CUtlRBTree<T, I, L, M>::MaxElement() const
	{
		return (I)m_Elements.NumAllocated();
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::Parent(I i) const
	{
		return Links(i).m_Parent;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::LeftChild(I i) const
	{
		return Links(i).m_Left;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::RightChild(I i) const
	{
		return Links(i).m_Right;
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsLeftChild(I i) const
	{
		return LeftChild(Parent(i)) == i;
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsRightChild(I i) const
	{
		return RightChild(Parent(i)) == i;
	}
	
	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsRoot(I i) const
	{
		return i == m_Root;
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsLeaf(I i) const
	{
		return (LeftChild(i) == InvalidIndex()) && (RightChild(i) == InvalidIndex());
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsValidIndex(I i) const
	{
		if (!m_Elements.IsIdxValid(i))
			return false;

		if (m_Elements.IsIdxAfter(i, m_LastAlloc))
			return false; // don't read values that have been allocated, but not constructed

		return LeftChild(i) != i;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::InvalidIndex()
	{
		return (I)M::InvalidIndex();
	}

	template < class T, class I, typename L, class M >
	int CUtlRBTree<T, I, L, M>::Depth() const
	{
		return Depth(Root());
	}

	template < class T, class I, typename L, class M >
	void  CUtlRBTree<T, I, L, M>::SetParent(I i, I parent)
	{
		Links(i).m_Parent = parent;
	}

	template < class T, class I, typename L, class M >
	void  CUtlRBTree<T, I, L, M>::SetLeftChild(I i, I child)
	{
		Links(i).m_Left = child;
	}

	template < class T, class I, typename L, class M >
	void  CUtlRBTree<T, I, L, M>::SetRightChild(I i, I child)
	{
		Links(i).m_Right = child;
	}

	template < class T, class I, typename L, class M >
	typename CUtlRBTree<T, I, L, M>::Links_t const& CUtlRBTree<T, I, L, M>::Links(I i) const
	{
		// Sentinel node, makes life easier
		static Links_t s_Sentinel =
		{
			InvalidIndex(), InvalidIndex(), InvalidIndex(), CUtlRBTree<T, I, L, M>::BLACK
		};

		return (i != InvalidIndex()) ? *(Links_t*)& m_Elements[i] : *(Links_t*)& s_Sentinel;
	}

	template < class T, class I, typename L, class M >
	typename CUtlRBTree<T, I, L, M>::Links_t& CUtlRBTree<T, I, L, M>::Links(I i)
	{
		assert(i != InvalidIndex());
		return *(Links_t*)& m_Elements[i];
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsRed(I i) const
	{
		return (Links(i).m_Tag == RED);
	}

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsBlack(I i) const
	{
		return (Links(i).m_Tag == BLACK);
	}

	template < class T, class I, typename L, class M >
	typename CUtlRBTree<T, I, L, M>::NodeColor_t  CUtlRBTree<T, I, L, M>::Color(I i) const
	{
		return (NodeColor_t)Links(i).m_Tag;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::SetColor(I i, typename CUtlRBTree<T, I, L, M>::NodeColor_t c)
	{
		Links(i).m_Tag = (I)c;
	}

#pragma warning(push)
#pragma warning(disable:4389) // '==' : signed/unsigned mismatch
	template < class T, class I, typename L, class M >
	I  CUtlRBTree<T, I, L, M>::NewNode()
	{
		I elem;

		// Nothing in the free list; add.
		if (m_FirstFree == InvalidIndex())
		{
			assert(m_Elements.IsValidIterator(m_LastAlloc) || m_NumElements == 0);
			typename M::Iterator_t it = m_Elements.IsValidIterator(m_LastAlloc) ? m_Elements.Next(m_LastAlloc) : m_Elements.First();
			if (!m_Elements.IsValidIterator(it))
			{
				//MEM_ALLOC_CREDIT_CLASS();
				m_Elements.Grow();

				it = m_Elements.IsValidIterator(m_LastAlloc) ? m_Elements.Next(m_LastAlloc) : m_Elements.First();

				assert(m_Elements.IsValidIterator(it));
				//if (!m_Elements.IsValidIterator(it))
				//{

				//	//Error("CUtlRBTree overflow!\n");
				//}
			}
			m_LastAlloc = it;
			elem = m_Elements.GetIndex(m_LastAlloc);
			assert(m_Elements.IsValidIterator(m_LastAlloc));
		}
		else
		{
			elem = m_FirstFree;
			m_FirstFree = Links(m_FirstFree).m_Right;
		}

#ifdef _DEBUG
		// reset links to invalid....
		Links_t& node = Links(elem);
		node.m_Left = node.m_Right = node.m_Parent = InvalidIndex();
#endif

		Construct(&Element(elem));
		ResetDbgInfo();

		return elem;
	}
#pragma warning(pop)

	template < class T, class I, typename L, class M >
	void  CUtlRBTree<T, I, L, M>::FreeNode(I i)
	{
		assert(IsValidIndex(i) && (i != InvalidIndex()));
		Destruct(&Element(i));
		SetLeftChild(i, i); // indicates it's in not in the tree
		SetRightChild(i, m_FirstFree);
		m_FirstFree = i;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::RotateLeft(I elem)
	{
		I rightchild = RightChild(elem);
		SetRightChild(elem, LeftChild(rightchild));
		if (LeftChild(rightchild) != InvalidIndex())
			SetParent(LeftChild(rightchild), elem);

		if (rightchild != InvalidIndex())
			SetParent(rightchild, Parent(elem));
		if (!IsRoot(elem))
		{
			if (IsLeftChild(elem))
				SetLeftChild(Parent(elem), rightchild);
			else
				SetRightChild(Parent(elem), rightchild);
		}
		else
			m_Root = rightchild;

		SetLeftChild(rightchild, elem);
		if (elem != InvalidIndex())
			SetParent(elem, rightchild);
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::RotateRight(I elem)
	{
		I leftchild = LeftChild(elem);
		SetLeftChild(elem, RightChild(leftchild));
		if (RightChild(leftchild) != InvalidIndex())
			SetParent(RightChild(leftchild), elem);

		if (leftchild != InvalidIndex())
			SetParent(leftchild, Parent(elem));
		if (!IsRoot(elem))
		{
			if (IsRightChild(elem))
				SetRightChild(Parent(elem), leftchild);
			else
				SetLeftChild(Parent(elem), leftchild);
		}
		else
			m_Root = leftchild;

		SetRightChild(leftchild, elem);
		if (elem != InvalidIndex())
			SetParent(elem, leftchild);
	}


	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::InsertRebalance(I elem)
	{
		while (!IsRoot(elem) && (Color(Parent(elem)) == RED))
		{
			I parent = Parent(elem);
			I grandparent = Parent(parent);

			/* we have a violation */
			if (IsLeftChild(parent))
			{
				I uncle = RightChild(grandparent);
				if (IsRed(uncle))
				{
					/* uncle is RED */
					SetColor(parent, BLACK);
					SetColor(uncle, BLACK);
					SetColor(grandparent, RED);
					elem = grandparent;
				}
				else
				{
					/* uncle is BLACK */
					if (IsRightChild(elem))
					{
						/* make x a left child, will change parent and grandparent */
						elem = parent;
						RotateLeft(elem);
						parent = Parent(elem);
						grandparent = Parent(parent);
					}
					/* recolor and rotate */
					SetColor(parent, BLACK);
					SetColor(grandparent, RED);
					RotateRight(grandparent);
				}
			}
			else
			{
				/* mirror image of above code */
				I uncle = LeftChild(grandparent);
				if (IsRed(uncle))
				{
					/* uncle is RED */
					SetColor(parent, BLACK);
					SetColor(uncle, BLACK);
					SetColor(grandparent, RED);
					elem = grandparent;
				}
				else
				{
					/* uncle is BLACK */
					if (IsLeftChild(elem))
					{
						/* make x a right child, will change parent and grandparent */
						elem = parent;
						RotateRight(parent);
						parent = Parent(elem);
						grandparent = Parent(parent);
					}
					/* recolor and rotate */
					SetColor(parent, BLACK);
					SetColor(grandparent, RED);
					RotateLeft(grandparent);
				}
			}
		}
		SetColor(m_Root, BLACK);
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::InsertAt(I parent, bool leftchild)
	{
		I i = NewNode();
		LinkToParent(i, parent, leftchild);
		++m_NumElements;

		assert(IsValid());

		return i;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::LinkToParent(I i, I parent, bool isLeft)
	{
		Links_t& elem = Links(i);
		elem.m_Parent = parent;
		elem.m_Left = elem.m_Right = InvalidIndex();
		elem.m_Tag = RED;

		/* insert node in tree */
		if (parent != InvalidIndex())
		{
			if (isLeft)
				Links(parent).m_Left = i;
			else
				Links(parent).m_Right = i;
		}
		else
		{
			m_Root = i;
		}

		InsertRebalance(i);
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::RemoveRebalance(I elem)
	{
		while (elem != m_Root && IsBlack(elem))
		{
			I parent = Parent(elem);

			// If elem is the left child of the parent
			if (elem == LeftChild(parent))
			{
				// Get our sibling
				I sibling = RightChild(parent);
				if (IsRed(sibling))
				{
					SetColor(sibling, BLACK);
					SetColor(parent, RED);
					RotateLeft(parent);

					// We may have a new parent now
					parent = Parent(elem);
					sibling = RightChild(parent);
				}
				if ((IsBlack(LeftChild(sibling))) && (IsBlack(RightChild(sibling))))
				{
					if (sibling != InvalidIndex())
						SetColor(sibling, RED);
					elem = parent;
				}
				else
				{
					if (IsBlack(RightChild(sibling)))
					{
						SetColor(LeftChild(sibling), BLACK);
						SetColor(sibling, RED);
						RotateRight(sibling);

						// rotation may have changed this
						parent = Parent(elem);
						sibling = RightChild(parent);
					}
					SetColor(sibling, Color(parent));
					SetColor(parent, BLACK);
					SetColor(RightChild(sibling), BLACK);
					RotateLeft(parent);
					elem = m_Root;
				}
			}
			else
			{
				// Elem is the right child of the parent
				I sibling = LeftChild(parent);
				if (IsRed(sibling))
				{
					SetColor(sibling, BLACK);
					SetColor(parent, RED);
					RotateRight(parent);

					// We may have a new parent now
					parent = Parent(elem);
					sibling = LeftChild(parent);
				}
				if ((IsBlack(RightChild(sibling))) && (IsBlack(LeftChild(sibling))))
				{
					if (sibling != InvalidIndex())
						SetColor(sibling, RED);
					elem = parent;
				}
				else
				{
					if (IsBlack(LeftChild(sibling)))
					{
						SetColor(RightChild(sibling), BLACK);
						SetColor(sibling, RED);
						RotateLeft(sibling);

						// rotation may have changed this
						parent = Parent(elem);
						sibling = LeftChild(parent);
					}
					SetColor(sibling, Color(parent));
					SetColor(parent, BLACK);
					SetColor(LeftChild(sibling), BLACK);
					RotateRight(parent);
					elem = m_Root;
				}
			}
		}
		SetColor(elem, BLACK);
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Unlink(I elem)
	{
		if (elem != InvalidIndex())
		{
			I x, y;

			if ((LeftChild(elem) == InvalidIndex()) ||
				(RightChild(elem) == InvalidIndex()))
			{
				/* y has a NIL node as a child */
				y = elem;
			}
			else
			{
				/* find tree successor with a NIL node as a child */
				y = RightChild(elem);
				while (LeftChild(y) != InvalidIndex())
					y = LeftChild(y);
			}

			/* x is y's only child */
			if (LeftChild(y) != InvalidIndex())
				x = LeftChild(y);
			else
				x = RightChild(y);

			/* remove y from the parent chain */
			if (x != InvalidIndex())
				SetParent(x, Parent(y));
			if (!IsRoot(y))
			{
				if (IsLeftChild(y))
					SetLeftChild(Parent(y), x);
				else
					SetRightChild(Parent(y), x);
			}
			else
				m_Root = x;

			// need to store this off now, we'll be resetting y's color
			NodeColor_t ycolor = Color(y);
			if (y != elem)
			{
				// Standard implementations copy the data around, we cannot here.
				// Hook in y to link to the same stuff elem used to.
				SetParent(y, Parent(elem));
				SetRightChild(y, RightChild(elem));
				SetLeftChild(y, LeftChild(elem));

				if (!IsRoot(elem))
					if (IsLeftChild(elem))
						SetLeftChild(Parent(elem), y);
					else
						SetRightChild(Parent(elem), y);
				else
					m_Root = y;

				if (LeftChild(y) != InvalidIndex())
					SetParent(LeftChild(y), y);
				if (RightChild(y) != InvalidIndex())
					SetParent(RightChild(y), y);

				SetColor(y, Color(elem));
			}

			if ((x != InvalidIndex()) && (ycolor == BLACK))
				RemoveRebalance(x);
		}
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Link(I elem)
	{
		if (elem != InvalidIndex())
		{
			I parent;
			bool leftchild;

			FindInsertionPosition(Element(elem), parent, leftchild);

			LinkToParent(elem, parent, leftchild);

			assert(IsValid());
		}
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::RemoveAt(I elem)
	{
		if (elem != InvalidIndex())
		{
			Unlink(elem);

			FreeNode(elem);
			--m_NumElements;

			assert(IsValid());
		}
	}


	template < class T, class I, typename L, class M > bool CUtlRBTree<T, I, L, M>::Remove(T const& search)
	{
		I node = Find(search);
		if (node != InvalidIndex())
		{
			RemoveAt(node);
			return true;
		}
		return false;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::RemoveAll()
	{
		// Have to do some convoluted stuff to invoke the destructor on all
		// valid elements for the multilist case (since we don't have all elements
		// connected to each other in a list).

		if (m_LastAlloc == m_Elements.InvalidIterator())
		{
			assert(m_Root == InvalidIndex());
			assert(m_FirstFree == InvalidIndex());
			assert(m_NumElements == 0);
			return;
		}

		for (typename M::Iterator_t it = m_Elements.First(); it != m_Elements.InvalidIterator(); it = m_Elements.Next(it))
		{
			I i = m_Elements.GetIndex(it);
			if (IsValidIndex(i)) // skip elements in the free list
			{
				Destruct(&Element(i));
				SetRightChild(i, m_FirstFree);
				SetLeftChild(i, i);
				m_FirstFree = i;
			}

			if (it == m_LastAlloc)
				break; // don't destruct elements that haven't ever been constucted
		}

		// Clear everything else out
		m_Root = InvalidIndex();
		m_NumElements = 0;

		assert(IsValid());
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Purge()
	{
		RemoveAll();
		m_FirstFree = InvalidIndex();
		m_Elements.Purge();
		m_LastAlloc = m_Elements.InvalidIterator();
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::FirstInorder() const
	{
		I i = m_Root;
		while (LeftChild(i) != InvalidIndex())
			i = LeftChild(i);
		return i;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::NextInorder(I i) const
	{
		// Don't go into an infinite loop if it's a bad index
		assert(IsValidIndex(i));
		if (!IsValidIndex(i))
			return InvalidIndex();

		if (RightChild(i) != InvalidIndex())
		{
			i = RightChild(i);
			while (LeftChild(i) != InvalidIndex())
				i = LeftChild(i);
			return i;
		}

		I parent = Parent(i);
		while (IsRightChild(i))
		{
			i = parent;
			if (i == InvalidIndex()) break;
			parent = Parent(i);
		}
		return parent;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::PrevInorder(I i) const
	{
		// Don't go into an infinite loop if it's a bad index
		assert(IsValidIndex(i));
		if (!IsValidIndex(i))
			return InvalidIndex();

		if (LeftChild(i) != InvalidIndex())
		{
			i = LeftChild(i);
			while (RightChild(i) != InvalidIndex())
				i = RightChild(i);
			return i;
		}

		I parent = Parent(i);
		while (IsLeftChild(i))
		{
			i = parent;
			if (i == InvalidIndex()) break;
			parent = Parent(i);
		}
		return parent;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::LastInorder() const
	{
		I i = m_Root;
		while (RightChild(i) != InvalidIndex())
			i = RightChild(i);
		return i;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::FirstPreorder() const
	{
		return m_Root;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::NextPreorder(I i) const
	{
		if (LeftChild(i) != InvalidIndex())
			return LeftChild(i);

		if (RightChild(i) != InvalidIndex())
			return RightChild(i);

		I parent = Parent(i);
		while (parent != InvalidIndex())
		{
			if (IsLeftChild(i) && (RightChild(parent) != InvalidIndex()))
				return RightChild(parent);
			i = parent;
			parent = Parent(parent);
		}
		return InvalidIndex();
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::PrevPreorder(I i) const
	{
		assert(0);  // not implemented yet
		return InvalidIndex();
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::LastPreorder() const
	{
		I i = m_Root;
		while (1)
		{
			while (RightChild(i) != InvalidIndex())
				i = RightChild(i);

			if (LeftChild(i) != InvalidIndex())
				i = LeftChild(i);
			else
				break;
		}
		return i;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::FirstPostorder() const
	{
		I i = m_Root;
		while (!IsLeaf(i))
		{
			if (LeftChild(i))
				i = LeftChild(i);
			else
				i = RightChild(i);
		}
		return i;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::NextPostorder(I i) const
	{
		I parent = Parent(i);
		if (parent == InvalidIndex())
			return InvalidIndex();

		if (IsRightChild(i))
			return parent;

		if (RightChild(parent) == InvalidIndex())
			return parent;

		i = RightChild(parent);
		while (!IsLeaf(i))
		{
			if (LeftChild(i))
				i = LeftChild(i);
			else
				i = RightChild(i);
		}
		return i;
	}


	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Reinsert(I elem)
	{
		Unlink(elem);
		Link(elem);
	}


	//-----------------------------------------------------------------------------
	// returns the tree depth (not a very fast operation)
	//-----------------------------------------------------------------------------

	template < class T, class I, typename L, class M >
	int CUtlRBTree<T, I, L, M>::Depth(I node) const
	{
		if (node == InvalidIndex())
			return 0;

		int depthright = Depth(RightChild(node));
		int depthleft = Depth(LeftChild(node));
		return max(depthright, depthleft) + 1;
	}


	//#define UTLTREE_PARANOID

	template < class T, class I, typename L, class M >
	bool CUtlRBTree<T, I, L, M>::IsValid() const
	{
		if (!Count())
			return true;

		if (m_LastAlloc == m_Elements.InvalidIterator())
			return false;

		if (!m_Elements.IsIdxValid(Root()))
			return false;

		if (Parent(Root()) != InvalidIndex())
			return false;

#ifdef UTLTREE_PARANOID

		// First check to see that mNumEntries matches reality.
		// count items on the free list
		int numFree = 0;
		for (int i = m_FirstFree; i != InvalidIndex(); i = RightChild(i))
		{
			++numFree;
			if (!m_Elements.IsIdxValid(i))
				return false;
		}

		// iterate over all elements, looking for validity 
		// based on the self pointers
		int nElements = 0;
		int numFree2 = 0;
		for (M::Iterator_t it = m_Elements.First(); it != m_Elements.InvalidIterator(); it = m_Elements.Next(it))
		{
			I i = m_Elements.GetIndex(it);
			if (!IsValidIndex(i))
			{
				++numFree2;
			}
			else
			{
				++nElements;

				int right = RightChild(i);
				int left = LeftChild(i);
				if ((right == left) && (right != InvalidIndex()))
					return false;

				if (right != InvalidIndex())
				{
					if (!IsValidIndex(right))
						return false;
					if (Parent(right) != i)
						return false;
					if (IsRed(i) && IsRed(right))
						return false;
				}

				if (left != InvalidIndex())
				{
					if (!IsValidIndex(left))
						return false;
					if (Parent(left) != i)
						return false;
					if (IsRed(i) && IsRed(left))
						return false;
				}
			}

			if (it == m_LastAlloc)
				break;
		}
		if (numFree2 != numFree)
			return false;

		if (nElements != m_NumElements)
			return false;

#endif // UTLTREE_PARANOID

		return true;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::SetLessFunc(const typename CUtlRBTree<T, I, L, M>::LessFunc_t& func)
	{
		if (!m_LessFunc)
		{
			m_LessFunc = func;
		}
		else if (Count() > 0)
		{
			// need to re-sort the tree here....
			assert(0);
		}
	}


	// Inserts a node into the tree, doesn't copy the data in.
	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::FindInsertionPosition(T const& insert, I& parent, bool& leftchild)
	{
		assert(m_LessFunc);

		/* find where node belongs */
		I current = m_Root;
		parent = InvalidIndex();
		leftchild = false;
		while (current != InvalidIndex())
		{
			parent = current;
			if (m_LessFunc(insert, Element(current)))
			{
				leftchild = true; current = LeftChild(current);
			}
			else
			{
				leftchild = false; current = RightChild(current);
			}
		}
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::Insert(T const& insert)
	{
		// use copy constructor to copy it in
		I parent;
		bool leftchild;
		FindInsertionPosition(insert, parent, leftchild);
		I newNode = InsertAt(parent, leftchild);
		CopyConstruct(&Element(newNode), insert);
		return newNode;
	}


	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Insert(const T* pArray, int nItems)
	{
		while (nItems--)
		{
			Insert(*pArray++);
		}
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::InsertIfNotFound(T const& insert)
	{
		I current = m_Root;
		I parent = InvalidIndex();
		bool leftchild = false;
		while (current != InvalidIndex())
		{
			parent = current;
			if (m_LessFunc(insert, Element(current)))
			{
				leftchild = true; current = LeftChild(current);
			}
			else if (m_LessFunc(Element(current), insert))
			{
				leftchild = false; current = RightChild(current);
			}
			else
				// Match found, no insertion
				return InvalidIndex();
		}

		I newNode = InsertAt(parent, leftchild);
		CopyConstruct(&Element(newNode), insert);
		return newNode;
	}

	template < class T, class I, typename L, class M >
	I CUtlRBTree<T, I, L, M>::Find(T const& search) const
	{
		assert(m_LessFunc);

		I current = m_Root;
		while (current != InvalidIndex())
		{
			if (m_LessFunc(search, Element(current)))
				current = LeftChild(current);
			else if (m_LessFunc(Element(current), search))
				current = RightChild(current);
			else
				break;
		}
		return current;
	}

	template < class T, class I, typename L, class M >
	void CUtlRBTree<T, I, L, M>::Swap(CUtlRBTree< T, I, L >& that)
	{
		m_Elements.Swap(that.m_Elements);
		std::swap(m_LessFunc, that.m_LessFunc);
		std::swap(m_Root, that.m_Root);
		std::swap(m_NumElements, that.m_NumElements);
		std::swap(m_FirstFree, that.m_FirstFree);
		std::swap(m_pElements, that.m_pElements);
		std::swap(m_LastAlloc, that.m_LastAlloc);
		assert(IsValid());
		assert(m_Elements.IsValidIterator(m_LastAlloc) || (m_NumElements == 0 && m_FirstFree == InvalidIndex()));
	}

	template <typename K, typename T, typename I = unsigned short, typename LessFunc_t = bool (__cdecl*)(const K&, const K&)>
	class CUtlMap : public base_utlmap_t
	{
	public:
		typedef K KeyType_t;
		typedef T ElemType_t;
		typedef I IndexType_t;

		// constructor, destructor
		// Left at growSize = 0, the memory will first allocate 1 element and double in size
		// at each increment.
		// LessFunc_t is required, but may be set after the constructor using SetLessFunc() below
		CUtlMap(int growSize = 0, int initSize = 0, const LessFunc_t& lessfunc = 0)
			: m_Tree(growSize, initSize, CKeyLess(lessfunc))
		{
		}

		CUtlMap(LessFunc_t lessfunc)
			: m_Tree(CKeyLess(lessfunc))
		{
		}

		void EnsureCapacity(int num) { m_Tree.EnsureCapacity(num); }

		// gets particular elements
		ElemType_t& Element(IndexType_t i) { return m_Tree.Element(i).elem; }
		const ElemType_t& Element(IndexType_t i) const { return m_Tree.Element(i).elem; }
		ElemType_t& operator[](IndexType_t i) { return m_Tree.Element(i).elem; }
		const ElemType_t& operator[](IndexType_t i) const { return m_Tree.Element(i).elem; }
		KeyType_t& Key(IndexType_t i) { return m_Tree.Element(i).key; }
		const KeyType_t& Key(IndexType_t i) const { return m_Tree.Element(i).key; }


		// Num elements
		unsigned int Count() const { return m_Tree.Count(); }

		// Max "size" of the vector
		IndexType_t  MaxElement() const { return m_Tree.MaxElement(); }

		// Checks if a node is valid and in the map
		bool  IsValidIndex(IndexType_t i) const { return m_Tree.IsValidIndex(i); }

		// Checks if the map as a whole is valid
		bool  IsValid() const { return m_Tree.IsValid(); }

		// Invalid index
		static IndexType_t InvalidIndex() { return CTree::InvalidIndex(); }

		// Sets the less func
		void SetLessFunc(LessFunc_t func)
		{
			m_Tree.SetLessFunc(CKeyLess(func));
		}

		// Insert method (inserts in order)
		IndexType_t  Insert(const KeyType_t& key, const ElemType_t& insert)
		{
			Node_t node;
			node.key = key;
			node.elem = insert;
			return m_Tree.Insert(node);
		}

		IndexType_t  Insert(const KeyType_t& key)
		{
			Node_t node;
			node.key = key;
			return m_Tree.Insert(node);
		}

		// API to macth src2 for Panormama
		// Note in src2 straight Insert() calls will assert on duplicates
		// Chosing not to take that change until discussed further 

		IndexType_t  InsertWithDupes(const KeyType_t& key, const ElemType_t& insert)
		{
			Node_t node;
			node.key = key;
			node.elem = insert;
			return m_Tree.Insert(node);
		}

		IndexType_t  InsertWithDupes(const KeyType_t& key)
		{
			Node_t node;
			node.key = key;
			return m_Tree.Insert(node);
		}


		bool HasElement(const KeyType_t& key) const
		{
			Node_t dummyNode;
			dummyNode.key = key;
			return m_Tree.HasElement(dummyNode);
		}


		// Find method
		IndexType_t  Find(const KeyType_t& key) const
		{
			Node_t dummyNode;
			dummyNode.key = key;
			return m_Tree.Find(dummyNode);
		}

		// FindFirst method
		// This finds the first inorder occurrence of key
		IndexType_t  FindFirst(const KeyType_t& key) const
		{
			Node_t dummyNode;
			dummyNode.key = key;
			return m_Tree.FindFirst(dummyNode);
		}


		const ElemType_t& FindElement(const KeyType_t& key, const ElemType_t& defaultValue) const
		{
			IndexType_t i = Find(key);
			if (i == InvalidIndex())
				return defaultValue;
			return Element(i);
		}


		// First element >= key
		IndexType_t  FindClosest(const KeyType_t& key, CompareOperands_t eFindCriteria) const
		{
			Node_t dummyNode;
			dummyNode.key = key;
			return m_Tree.FindClosest(dummyNode, eFindCriteria);
		}

		// Remove methods
		void     RemoveAt(IndexType_t i) { m_Tree.RemoveAt(i); }
		bool     Remove(const KeyType_t& key)
		{
			Node_t dummyNode;
			dummyNode.key = key;
			return m_Tree.Remove(dummyNode);
		}

		void     RemoveAll() { m_Tree.RemoveAll(); }
		void     Purge() { m_Tree.Purge(); }

		// Purges the list and calls delete on each element in it.
		void PurgeAndDeleteElements()
		{
			for (I i = 0; i < MaxElement(); ++i)
			{
				if (!IsValidIndex(i))
					continue;

				delete Element(i);
			}

			Purge();
		}

		// Iteration
		IndexType_t  FirstInorder() const { return m_Tree.FirstInorder(); }
		IndexType_t  NextInorder(IndexType_t i) const { return m_Tree.NextInorder(i); }
		IndexType_t  PrevInorder(IndexType_t i) const { return m_Tree.PrevInorder(i); }
		IndexType_t  LastInorder() const { return m_Tree.LastInorder(); }

		// API Matching src2 for Panorama
		IndexType_t  NextInorderSameKey(IndexType_t i) const
		{
			IndexType_t iNext = NextInorder(i);
			if (!IsValidIndex(iNext))
				return InvalidIndex();
			if (Key(iNext) != Key(i))
				return InvalidIndex();
			return iNext;
		}

		// If you change the search key, this can be used to reinsert the 
		// element into the map.
		void	Reinsert(const KeyType_t& key, IndexType_t i)
		{
			m_Tree[i].key = key;
			m_Tree.Reinsert(i);
		}

		IndexType_t InsertOrReplace(const KeyType_t& key, const ElemType_t& insert)
		{
			IndexType_t i = Find(key);
			if (i != InvalidIndex())
			{
				Element(i) = insert;
				return i;
			}

			return Insert(key, insert);
		}

		void Swap(CUtlMap< K, T, I >& that)
		{
			m_Tree.Swap(that.m_Tree);
		}


		struct Node_t
		{
			Node_t()
			{
			}

			Node_t(const Node_t& from)
				: key(from.key),
				elem(from.elem)
			{
			}

			KeyType_t	key;
			ElemType_t	elem;
		};

		class CKeyLess
		{
		public:
			CKeyLess(const LessFunc_t& lessFunc) : m_LessFunc(lessFunc) {}

			bool operator!() const
			{
				return !m_LessFunc;
			}

			bool operator()(const Node_t& left, const Node_t& right) const
			{
				return m_LessFunc(left.key, right.key);
			}

			LessFunc_t m_LessFunc;
		};

		typedef CUtlRBTree<Node_t, I, CKeyLess> CTree;

		CTree* AccessTree() { return &m_Tree; }

	protected:
		CTree 	   m_Tree;
	};

	//template <typename T, typename I = unsigned short, typename L = bool(__cdecl *)( const T&, const T& ), typename M = CUtlMemory<UtlRBTreeNode_t<T, I>, I>>
	//class CUtlRBTree
	//{
	//public:
	//	T& Element( I i )
	//	{
	//		return m_Elements.Element( i ).m_Data;
	//	}
	// 
	//	const T& Element( I i ) const
	//	{
	//		return m_Elements.Element( i ).m_Data;
	//	}
	// 
	//	I MaxElement() const
	//	{
	//		return I( m_Elements.NumAllocated() );
	//	}
	// 
	//	I LeftChild( I i ) const
	//	{
	//		return Links( i ).m_Left;
	//	}
	// 
	//	I RightChild( I i ) const
	//	{
	//		return Links( i ).m_Right;
	//	}
	// 
	//	I Parent( I i ) const
	//	{
	//		return Links( i ).m_Parent;
	//	}
	// 
	//	bool IsRightChild( I i ) const
	//	{
	//		return RightChild( Parent( i ) ) == i;
	//	}
	// 
	//	bool IsValidIndex( I i ) const
	//	{
	//		if ( i < 0 )
	//			return false;
	// 
	//		if ( i > m_LastAlloc.index )
	//			return false;
	// 
	//		return LeftChild( i ) != i;
	//	}
	// 
	//	I Find( const T& Search ) const
	//	{
	//		I Current = m_Root;
	//		while ( Current != I( -1 ) )
	//		{
	//			if ( m_LessFunc( Search, Element( Current ) ) )
	//				Current = LeftChild( Current );
	//			else if ( m_LessFunc( Element( Current ), Search ) )
	//				Current = RightChild( Current );
	//			else
	//				break;
	//		}
	// 
	//		return Current;
	//	}
	// 
	//	I FirstInorder() const
	//	{
	//		I i = m_Root;
	//		while ( LeftChild( i ) != I( -1 ) )
	//			i = LeftChild( i );
	// 
	//		return i;
	//	}
	// 
	//	I NextInorder( I i ) const
	//	{
	//		if ( RightChild( i ) != I( -1 ) )
	//		{
	//			i = RightChild( i );
	//			while ( LeftChild( i ) != I( -1 ) )
	//				i = LeftChild( i );
	// 
	//			return i;
	//		}
	// 
	//		I p = Parent( i );
	//		while ( IsRightChild( i ) )
	//		{
	//			i = p;
	//			if ( i == I( -1 ) )
	//				break;
	// 
	//			p = Parent( i );
	//		}
	// 
	//		return p;
	//	}
	// 
	//protected:
	//	const UtlRBTreeLinks_t<I>& Links( I i ) const
	//	{
	//		static UtlRBTreeLinks_t<I> s_Sentinel = { I( -1 ), I( -1 ), I( -1 ), I( 1 ) };
	//		return i == I( -1 ) ? s_Sentinel : *reinterpret_cast<const UtlRBTreeLinks_t<I>*>( &m_Elements.Element( i ) );
	//	}
	// 
	//	L m_LessFunc;
	// 
	//	M m_Elements;
	//	I m_Root;
	//	I m_NumElements;
	//	I m_FirstFree;
	//	typename M::Iterator_t m_LastAlloc;
	// 
	//	UtlRBTreeNode_t<T, I>* m_pElements = nullptr;
	//};



	//template <typename K, typename T, typename I = int>
	//class CUtlMap
	//{
	//public:
	//	using LessFunc_t = bool( __cdecl* )( const K&, const K& );
	// 
	//	T& operator[]( I i )
	//	{
	//		return m_Tree.Element( i ).Element;
	//	}
	// 
	//	K& Key( I i )
	//	{
	//		return m_Tree.Element( i ).Key;
	//	}
	// 
	//	const K& Key( I i ) const
	//	{
	//		return m_Tree.Element( i ).Key;
	//	}
	// 
	//	I MaxElement() const
	//	{
	//		return m_Tree.MaxElement();
	//	}
	// 
	//	bool IsValidIndex( I i ) const
	//	{
	//		return m_Tree.IsValidIndex( i );
	//	}
	// 
	//	I Find( const K& Key ) const
	//	{
	//		Node_t Dummy;
	//		Dummy.Key = Key;
	//		return m_Tree.Find( Dummy );
	//	}
	// 
	//	I FirstInorder() const
	//	{
	//		return m_Tree.FirstInorder();
	//	}
	// 
	//	I NextInorder( I i ) const
	//	{
	//		return m_Tree.NextInorder( i );
	//	}
	// 
	//	struct Node_t
	//	{
	//		K Key;
	//		T Element;
	//	};
	// 
	//	class CKeyLess
	//	{
	//	public:
	//		explicit CKeyLess( LessFunc_t lessFunc ) : m_LessFunc( lessFunc ) {}
	// 
	//		bool operator!() const
	//		{
	//			return !m_LessFunc;
	//		}
	// 
	//		bool operator()( const Node_t& left, const Node_t& right ) const
	//		{
	//			//return m_LessFunc( left.Key, right.Key );
	//			//LessFunc1(left.Key, right.Key);
	//			return left.Key > right.Key;
	//		}
	// 
	//		LessFunc_t m_LessFunc;
	//	};
	// 
	//protected:
	//	CUtlRBTree<Node_t, I, CKeyLess> m_Tree;
	//};

	template <typename K, typename T, typename I = int>
	class CUtlHashMapLarge : public CUtlMap<K,T,I> // I'm pretty sure this doesn't inhernit from CUtlMap but for what I'm doing it's good enough
	{
	public:
		I Find( const K& Key ) const // Temporary. Valve uses MurmurHash3.
		{
			for ( int (i) = 0; (i) < (*this).MaxElement(); ++(i) )
			{
				if ( !(*this).IsValidIndex( i ) ) continue;
				if ( this->Key( i ) == Key )
					return i;
			}

			return -1;
		}
	};

}
