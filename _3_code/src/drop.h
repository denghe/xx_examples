#pragma once
#include <cfg.h>

namespace Drop {

	template <class T> concept HasMember_id = requires(T) { T::id; };
	template <class T> concept HasMember_weight = requires(T) { T::weight; };

	struct ItemWeight {
		int32_t id;
		std::string name, desc;
		int32_t weight;
	};

	struct ItemWeights {
		xx::Listi32<ItemWeight> data;

		void Init(xx::Rnd& rnd, int32_t count, xx::FromTo<int32_t> weightRange) {
			data.Clear();
			data.Reserve(count);
			for (int32_t i = 0; i < count; ++i) {
				auto& iw = data.Emplace();
				iw.id = i;
				iw.weight = rnd.Next<int32_t>(weightRange.from, weightRange.to);
			}
		}

		// maybe useful?
		void Shuffle(xx::Rnd& rnd) {
			auto len = data.len;
			if (!len) return;
			auto buf = data.buf;
			for (int32_t i = 0, tar = 1; ++i != len; ++tar) {
				if (int32_t offset = rnd.Next(tar); offset != tar) {
					std::swap(buf[i], buf[offset]);
				}
			}
		}

	};

	struct Generator {
		xx::Listi32<std::pair<int32_t, int32_t>> idWeights;
		int32_t totalWeight{};

		void Init(xx::Listi32<ItemWeight> const& iws, xx::Rnd& rnd, int32_t count) {
			// simulate load config
			assert(count > 0 && count < iws.len);
			idWeights.Clear();
			idWeights.Resize(count);
			for (int32_t i = 0; i < count; ++i) {
				auto& iw = iws[i];
				idWeights[i] = { iw.id, iw.weight };
			}

			// prepare
			auto len = idWeights.len;
			if (len < 2) {
				totalWeight = idWeights[0].second;
				return;
			}
			for (int32_t i = 1; i < len; ++i) {
				idWeights[i].second += idWeights[i - 1].second;
			}
			totalWeight = idWeights.Back().second;
		}

		int32_t Calc1(int32_t weight) {
			for (auto& [id, w] : idWeights) {
				if (w >= weight) return id;
			}
			return -1;	// error
		}

		int32_t Calc1(xx::Rnd& rnd) {
			auto rv = rnd.Next<int32_t>(1, totalWeight);
			return Calc1(rv);
		}

		// todo: guess search
	};


	inline static void Test1(xx::Rnd& rnd, int32_t n, int32_t allItemCount, int32_t numDropItems, xx::FromTo<int32_t> weightRange) {

		ItemWeights iws;
		iws.Init(rnd, allItemCount, weightRange);

		Generator dg;
		dg.Init(iws.data, rnd, numDropItems);

		for (int32_t i = 0; i < 10; i++) {
			auto secs = xx::NowEpochSeconds();
			int64_t idCount{};
			for (int32_t j = 0; j < n; j++) {
				idCount += dg.Calc1(rnd);
			}
			xx::CoutN("n = ", n, " Drop::Test1 elapsed secs = ", xx::NowEpochSeconds(secs), " idCount = ", idCount);
		}
	}

	inline static void Test2(xx::Rnd& rnd, int32_t n, int32_t allItemCount, int32_t numDropItems, xx::FromTo<int32_t> weightRange) {

		ItemWeights iws;
		iws.Init(rnd, allItemCount, weightRange);

		Generator dg;
		dg.Init(iws.data, rnd, numDropItems);

		xx::Listi32<int32_t> rndWeights;
		rndWeights.Resize(n);
		for (int32_t i = 0; i < n; i++) {
			rndWeights[i] = rnd.Next<int32_t>(1, dg.totalWeight);
		}

		for (int32_t i = 0; i < 10; i++) {
			auto secs = xx::NowEpochSeconds();
			int64_t idCount{};
			for (int32_t j = 0; j < n; j++) {
				idCount += dg.Calc1(rndWeights[j]);
			}
			xx::CoutN("n = ", n, " Drop::Test2 elapsed secs = ", xx::NowEpochSeconds(secs), " idCount = ", idCount);
		}
	}

};
