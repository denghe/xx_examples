#pragma once

namespace xx {

	template<typename T>
	struct Map2d {
		XYi size{};
		std::unique_ptr<T[]> cells;

		XX_INLINE void Init(XYi size_) {
			assert(!cells);
			assert(size_.x > 0 && size_.y);
			size = size_;
			auto cellsLen = size_.x * size_.y;
			cells = std::make_unique_for_overwrite<T[]>(cellsLen);
		}
		XX_INLINE void Init(int32_t width, int32_t height) {
			Init({ width, height });
		}

		// out of range: return nullptr
		XX_INLINE T* TryAt(XYi pos) const {
			if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y) return nullptr;
			return (T*)&cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T* TryAt(int32_t x, int32_t y) const {
			return TryAt({ x, y });
		}

		XX_INLINE T* At(XYi pos) const {
			assert(pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y);
			return (T*)&cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T* At(int32_t x, int32_t y) const {
			return At({ x, y });
		}

		XX_INLINE T& operator[](XYi pos) const {
			assert(pos.x >= 0 && pos.x < size.x && pos.y >= 0 && pos.y < size.y);
			return (T&)cells[pos.y * size.x + pos.x];
		}
		XX_INLINE T& operator[](int32_t x, int32_t y) const {
			return operator[]({ x, y });
		}
	};

}
