#include "Neighbors.hpp"

namespace chm {
	std::vector<uint>::const_iterator Neighbors::begin() const {
		return this->beginIter;
	}

	void Neighbors::clear() {
		*this->count = 0;
		this->endIter = this->beginIter;
	}

	std::vector<uint>::const_iterator Neighbors::end() const {
		return this->endIter;
	}

	void Neighbors::fillFrom(const FarHeap& h) {
		std::vector<uint>::iterator iter = this->beginIter;
		const auto len = h.len();
		*this->count = uint(len);
		this->endIter = iter + len;
		*iter = h[0].id;

		for(size_t i = 1; i < len; i++) {
			iter++;
			*iter = h[i].id;
		}
	}

	void Neighbors::fillFrom(const FarHeap& h, Node& nearest) {
		std::vector<uint>::iterator iter = this->beginIter;
		const auto len = h.len();
		*this->count = uint(len);
		this->endIter = iter + len;

		{
			const auto& node = h[0];
			*iter = node.id;
			nearest.distance = node.distance;
			nearest.id = node.id;
		}

		for(size_t i = 1; i < len; i++) {
			iter++;
			const auto& node = h[i];
			*iter = node.id;

			if(node.distance < nearest.distance) {
				nearest.distance = node.distance;
				nearest.id = node.id;
			}
		}
	}

	uint Neighbors::get(const size_t i) const {
		return *(this->beginIter + i);
	}

	uint Neighbors::len() const {
		return *this->count;
	}

	Neighbors::Neighbors(const std::vector<uint>::iterator& count)
		: beginIter(count + 1), count(count), endIter(this->beginIter + *this->count) {}

	void Neighbors::push(const uint id) {
		*this->endIter++ = id;
		(*this->count)++;
	}
}
