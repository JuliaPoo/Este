#pragma once

#include <unordered_map>
#include "Este\sync.hpp"

template <class K, class V>
class thread_safe_map {
public:
	void erase(const K key)
	{
		auto w = this->_map_lock.writer_aquire();
		this->map.erase(key);
		this->_map_lock.writer_release(w);
	}

	void add(const K key, const V val)
	{
		auto w = this->_map_lock.writer_aquire();
		this->map[key] = val;
		this->_map_lock.writer_release(w);
	}

	bool isExists(const K key)
	{
		auto r = this->_map_lock.reader_aquire();
		bool ret = this->map.find(key) != this->map.end();
		this->_map_lock.reader_release(r);
		return ret;
	}

	V& get(const K key)
	{
		auto r = this->_map_lock.reader_aquire();
		auto it = this->map.find(key);
		this->_map_lock.reader_release(r);
		if (it == this->map.end())
			RAISE_EXCEPTION("`get` Key %p doesn't exist!", &key);
		return it->second;
	}

private:
	std::tr1::unordered_map<K, V> map;
	Sync::RW _map_lock;

};