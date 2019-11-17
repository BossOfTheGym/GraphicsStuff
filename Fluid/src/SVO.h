#ifndef SVO_H
#define SVO_H

#include "primitives.h"

#include <map>
#include <vector>


//next power of 2
template<class IntType, auto bits = 8 * sizeof(IntType)>
IntType npo2(IntType num)
{
	num -= 1;

	decltype(bits) bit = 1;
	while(bit < bits)
	{
		num = num | (num >> bit);
		bit <<= 1;
	}

	return num + 1;
}

template<class IntType>
IntType blog(IntType num)
{
	IntType count{};
	while (num)
	{
		num >>= 1;
		count++;
	}
	return count;
}


//simple sparse octree imitation
template<class Value>
class SparseVoxelOctree
{
public:
	using Int  = int;
	using Hash = int;
	using Map  = std::map<Hash, Value>;
	using Point   = Vec3;
	using Indices = Vec3i;


public:
	SparseVoxelOctree(const AABB& space, Int split) 
		: m_map()
		, m_space(space)
		, m_spaceDelta()
	{
		m_split  = split;
		m_extent = npo2(split);
		m_depth  = blog(m_extent);//std::ceil(std::log2(m_extent));
		m_depth2 = m_depth << 1;

		m_ratio = float(m_extent) / m_split;

		auto& [fc, sc] = m_space.corners;
		auto delta = sc - fc;
		m_spaceDelta = delta / float(split);
		sc = fc + delta * m_ratio;
	}

	SparseVoxelOctree(const SparseVoxelOctree&) = default;
	SparseVoxelOctree(SparseVoxelOctree&&)      = default;

	~SparseVoxelOctree() = default;

	SparseVoxelOctree& operator = (const SparseVoxelOctree&) = default;
	SparseVoxelOctree& operator = (SparseVoxelOctree&&)      = default;


public:
	// TODO : add templates
	Indices index(const Point& item) const
	{
		Point temp = item - m_space.corners[0];

		Indices indices{};
		
		indices[0] = Hash(temp[0] / m_spaceDelta[0]);
		indices[1] = Hash(temp[1] / m_spaceDelta[1]);
		indices[2] = Hash(temp[2] / m_spaceDelta[2]);

		return indices;
	}

	Indices index(const Hash& item) const
	{
		Hash hash = item;

		Hash mask = (1 << m_depth) - 1;

		Hash x = hash & mask;

		hash >>= m_depth;
		Hash y = (hash & mask);

		hash >>= m_depth;
		Hash z = (hash & mask);

		return Indices{x, y, z};
	}

	Indices index(const Indices& item) const
	{
		return item;
	}


	// TODO : add templates
	Point coord(const Point& point) const
	{
		return coord(index(point));
	}

	Point coord(const Hash& hash) const
	{
		return coord(index(hash));
	}

	Point coord(const Indices& indices) const
	{
		Point temp{};

		temp[0] = indices[0] * m_spaceDelta[0];
		temp[1] = indices[1] * m_spaceDelta[1];
		temp[2] = indices[2] * m_spaceDelta[2];

		temp += m_space.corners[0];

		return temp;
	}


	// TODO : add templates
	Point voxel(const Point& point) const
	{
		return coord(point) + Point(m_spaceDelta / 2.0f);
	}

	Point voxel(const Hash& hash) const
	{
		return coord(hash) + Point(m_spaceDelta / 2.0f);
	}

	Point voxel(const Indices& indices) const
	{
		return coord(indices) + Point(m_spaceDelta / 2.0f);
	}


	// TODO : add templates
	Hash hash(const Point& item) const
	{
		return hash(index(item));
	}

	Hash hash(const Hash& hash) const
	{
		return hash;
	}

	Hash hash(const Indices& indices) const
	{
		Hash xc = indices.x;
		Hash yc = indices.y;
		Hash zc = indices.z;

		return (zc) | (yc << m_depth) | (xc << m_depth2);
	}


	// TODO : add templates
	bool has(const Point& item) const
	{
		return m_map.find(hash(item)) != m_map.end();
	}

	bool has(const Hash& item) const
	{
		return m_map.find(hash(item)) != m_map.end();
	}

	bool has(const Indices& item) const
	{
		return m_map.find(hash(item)) != m_map.end();
	}


	// TODO : add templates
	Value& operator [] (const Point& item)
	{
		return m_map[hash(item)];
	}

	Value& operator [] (const Hash& item)
	{
		return m_map[hash(item)];
	}

	Value& operator [] (const Indices& item)
	{
		return m_map[hash(item)];
	}


public:
	const Point& spaceDelta() const
	{
		return m_spaceDelta;
	}

	Int split() const
	{
		return m_split;
	}


public:
	// TODO : add templates
	void add(const Point& item, const Value& value)
	{
		m_map[hash(item)] = value;
	}

	void add(const Hash& item, const Value& value)
	{
		m_map[hash(item)] = value;
	}

	void add(const Indices& item, const Value& value)
	{
		m_map[hash(item)] = value;
	}


	// TODO : add templates
	Value& get(const Point& item)
	{
		auto i = m_map.find(hash(item));

		return *i;
	}

	Value& get(const Hash& item)
	{
		auto i = m_map.find(hash(item));

		return *i;
	}

	Value& get(const Indices& item)
	{
		auto i = m_map.find(hash(item));

		return *i;
	}


	// TODO : add templates
	const Value& get(const Point& item) const
	{
		auto i = m_map.find(hash(item));

		return *i;
	}

	const Value& get(const Hash& item) const
	{
		auto i = m_map.find(hash(item));

		return *i;
	}

	const Value& get(const Indices& item) const
	{
		auto i = m_map.find(hash(item));

		return *i;
	}


	// TODO : add templates
	void remove(const Point& item)
	{
		m_map.erase(hash(item));
	}

	void remove(const Hash& item)
	{
		m_map.erase(hash(item));
	}

	void remove(const Indices& indices)
	{
		m_map.erase(hash(indices));
	}


public:
	auto begin()
	{
		return m_map.begin();
	}

	auto end()
	{
		return m_map.end();
	}


	auto begin() const
	{
		return m_map.begin();
	}

	auto end() const
	{
		return m_map.end();
	}


	auto size() const
	{
		return m_map.size();
	}


	void clear()
	{
		m_map.clear();
	}


private:
	Map m_map; // stores hashes of 

	AABB m_space;       // space[a,b]x[c,d] that will be split
	Point m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

	float m_ratio; //ratio of m_extent to m_split

	Int m_split;  // count of pieces that space was split to
	Int m_extent; // next power of 2 of m_split
	Int m_depth;  // depth of an octree
	Int m_depth2; // 2 * depth
};


template<class Value>
class FullVoxelOctree
{
public:
	using Int  = int;
	using Hash = int;
	using Map  = std::vector<Value>;
	using Point   = Vec3;
	using Indices = Vec3i;


public:
	FullVoxelOctree(const AABB& space, Int split, Value empty)
		: m_map()
		, m_empty(empty)
		, m_space(space)
		, m_spaceDelta()
	{
		m_split = split;
		m_biasX = split * split;
		m_biasY = split;

		m_map.resize(split * split * split);
		for (auto& val : m_map)
		{
			val = empty;
		}

		auto& [fc, sc] = m_space.corners;
		m_spaceDelta = (sc - fc) / float(m_split);
	}

	FullVoxelOctree(const FullVoxelOctree&) = default;
	FullVoxelOctree(FullVoxelOctree&&)      = default;

	FullVoxelOctree& operator = (const FullVoxelOctree&) = default;
	FullVoxelOctree& operator = (FullVoxelOctree&&)      = default;

	~FullVoxelOctree() = default;


public:
	// TODO : add templates
	Indices index(const Point& item) const
	{
		Point temp = item - m_space.corners[0];

		Indices indices{};

		indices[0] = Hash(temp[0] / m_spaceDelta[0]);
		indices[1] = Hash(temp[1] / m_spaceDelta[1]);
		indices[2] = Hash(temp[2] / m_spaceDelta[2]);

		return indices;
	}

	Indices index(const Hash& item) const
	{
		Hash hash = item;

		Hash x = hash / m_biasX;
		hash %= m_biasX;

		Hash y = hash / m_biasY;
		hash %= m_biasY;

		Hash z = hash;

		return Indices{x, y, z};
	}

	Indices index(const Indices& item) const
	{
		return item;
	}


	// TODO : add templates
	Point coord(const Point& point) const
	{
		return coord(index(point));
	}

	Point coord(const Hash& hash) const
	{
		return coord(index(hash));
	}

	Point coord(const Indices& indices) const
	{
		Point temp{};

		temp[0] = indices[0] * m_spaceDelta[0];
		temp[1] = indices[1] * m_spaceDelta[1];
		temp[2] = indices[2] * m_spaceDelta[2];

		temp += m_space.corners[0];

		return temp;
	}


	// TODO : add templates
	Point voxel(const Point& point) const
	{
		return coord(point) + Point(m_spaceDelta / 2.0f);
	}

	Point voxel(const Hash& hash) const
	{
		return coord(hash) + Point(m_spaceDelta / 2.0f);
	}

	Point voxel(const Indices& indices) const
	{
		return coord(indices) + Point(m_spaceDelta / 2.0f);
	}


	// TODO : add templates
	Hash hash(const Point& item) const
	{
		return hash(index(item));
	}

	Hash hash(const Hash& hash) const
	{
		return hash;
	}

	Hash hash(const Indices& indices) const
	{
		Hash xc = indices.x;
		Hash yc = indices.y;
		Hash zc = indices.z;

		return (zc) + (yc * m_biasY) + (xc * m_biasX);
	}


	// TODO : add templates
	bool has(const Point& item) const
	{
		return m_map[hash(item)] != m_empty;
	}

	bool has(const Hash& item) const
	{
		return m_map[hash(item)] != m_empty;
	}

	bool has(const Indices& item) const
	{
		return m_map[hash(item)] != m_empty;
	}


	// TODO : add templates
	Value& operator [] (const Point& item)
	{
		return m_map[hash(item)];
	}

	Value& operator [] (const Hash& item)
	{
		return m_map[hash(item)];
	}

	Value& operator [] (const Indices& item)
	{
		return m_map[hash(item)];
	}


public:
	const Point& spaceDelta() const
	{
		return m_spaceDelta;
	}

	Int split() const
	{
		return m_split;
	}

	Value empty() const
	{
		return m_empty;
	}


public:
	
	// TODO : add templates
	const Value& operator [] (const Point& item) const
	{
		return m_map[hash(item)];
	}

	const Value& operator [] (const Hash& item) const
	{
		return m_map[hash(item)];
	}

	const Value& operator [] (const Indices& item) const
	{
		return m_map[hash(item)];
	}


public:
	auto begin()
	{
		return m_map.begin();
	}

	auto end()
	{
		return m_map.end();
	}


	auto begin() const
	{
		return m_map.begin();
	}

	auto end() const
	{
		return m_map.end();
	}


	auto size() const
	{
		return m_map.size();
	}


	void clear()
	{
		for (auto& elem : m_map)
		{
			elem = Value();
		}
	}


private:
	Map m_map; // stores hashes of 

	AABB m_space;       // space[a,b]x[c,d] that will be split
	Point m_spaceDelta; // extents of a single piece to that space[a,b]x[c,d] was split

	float m_ratio; //ratio of m_extent to m_split
  
	Value m_empty; // empty value

	Int m_split; // count of pieces that space was split to
	Int m_biasX; // index = m_biasX * x + m_bias * y + z
	Int m_biasY; // index = m_biasX * x + m_bias * y + z
};


#endif